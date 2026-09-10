#include "InputReplay.h"

#include "GameManager.h"
#include "OgreCamera.h"
#include "OgreLogManager.h"
#include "OgreViewport.h"
#include "components/control/PlayerController.h"
#include "objects/AgentObject.h"
#include "systems/input/InputManager.h"
#include "systems/manager/ObjectManager.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	void Log(const std::string& message)
	{
		if (Ogre::LogManager::getSingletonPtr() != nullptr)
			Ogre::LogManager::getSingleton().logMessage("[InputReplay] synthetic=true " + message);
	}

	std::string Snapshot(GameManager& game, ObjectManager* objects)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(3) << " paused=" << (game.IsSimulationPaused() ? "true" : "false");
		if (game.getCamera() != nullptr)
		{
			const Ogre::Vector3 p = game.getCamera()->getDerivedPosition();
			const Ogre::Vector3 f = game.getCamera()->getDerivedDirection();
			stream << " camera=(" << p.x << "," << p.y << "," << p.z << ")"
				<< " cameraForward=(" << f.x << "," << f.y << "," << f.z << ")";
		}
		for (AgentObject* agent : objects->getAllAgents())
			if (agent != nullptr && agent->FindComponent<PlayerController>() != nullptr)
			{
				const Ogre::Vector3 p = agent->GetPosition();
				stream << " player=(" << p.x << "," << p.y << "," << p.z << ")";
				break;
			}
		return stream.str();
	}

	struct KeyName
	{
		const char* name;
		OIS::KeyCode key;
		unsigned int text;
	};

	const KeyName kKeys[] = {
		{"W", OIS::KC_W, 'w'}, {"A", OIS::KC_A, 'a'}, {"S", OIS::KC_S, 's'}, {"D", OIS::KC_D, 'd'},
		{"Q", OIS::KC_Q, 'q'}, {"E", OIS::KC_E, 'e'}, {"R", OIS::KC_R, 'r'}, {"F", OIS::KC_F, 'f'},
		{"G", OIS::KC_G, 'g'}, {"T", OIS::KC_T, 't'}, {"X", OIS::KC_X, 'x'}, {"I", OIS::KC_I, 'i'},
		{"O", OIS::KC_O, 'o'}, {"1", OIS::KC_1, '1'}, {"2", OIS::KC_2, '2'},
		{"RETURN", OIS::KC_RETURN, 13}, {"ENTER", OIS::KC_RETURN, 13},
		{"ESC", OIS::KC_ESCAPE, 27}, {"ESCAPE", OIS::KC_ESCAPE, 27}, {"TAB", OIS::KC_TAB, 9},
		{"SPACE", OIS::KC_SPACE, 32}, {"LSHIFT", OIS::KC_LSHIFT, 0}, {"RSHIFT", OIS::KC_RSHIFT, 0},
		{"UP", OIS::KC_UP, 0}, {"DOWN", OIS::KC_DOWN, 0}, {"LEFT", OIS::KC_LEFT, 0}, {"RIGHT", OIS::KC_RIGHT, 0},
		{"F1", OIS::KC_F1, 0}, {"F2", OIS::KC_F2, 0}, {"F3", OIS::KC_F3, 0},
		{"F4", OIS::KC_F4, 0}, {"F5", OIS::KC_F5, 0}, {"F6", OIS::KC_F6, 0}
	};

	enum EventKind { KeyDown, KeyUp, KeyPress, MouseDown, MouseUp, MouseMove, Wheel, Quit };
	struct Event
	{
		long long timeMs = 0;
		int line = 0;
		EventKind kind = Quit;
		OIS::KeyCode key = OIS::KC_UNASSIGNED;
		unsigned int text = 0;
		int x = 0;
		int y = 0;
		int value = 0;
		std::string description;
	};

	class Replay
	{
	public:
		Replay()
		{
			const char* path = std::getenv("HELLO_INPUT_REPLAY");
			if (path == nullptr || path[0] == 0) return;
			const char* waitForPlayer = std::getenv("HELLO_INPUT_REPLAY_WAIT_FOR_PLAYER");
			m_waitForPlayer = waitForPlayer == nullptr || std::string(waitForPlayer) != "0";
			Load(path);
		}

		bool Update(GameManager& game, ObjectManager* objects, InputManager* input, int deltaMs)
		{
			if (!m_enabled || m_complete || input == nullptr || objects == nullptr) return false;
			if (!m_started)
			{
				AgentObject* player = nullptr;
				if (m_waitForPlayer)
				{
					for (AgentObject* agent : objects->getAllAgents())
						if (agent != nullptr && agent->FindComponent<PlayerController>() != nullptr)
						{
							player = agent;
							break;
						}
					if (player == nullptr) return false;
				}
				m_started = true;
				m_elapsedMs = 0;
				Log("started player=" + (player != nullptr ? std::to_string(player->GetObjId()) : std::string("none"))
					+ " simMs=" + std::to_string(game.getTimeInMillis())
					+ " clock=update-delta-pauses-included");
			}
			else
				m_elapsedMs += std::max(0, deltaMs);

			while (m_next < m_events.size() && m_events[m_next].timeMs <= m_elapsedMs)
			{
				const Event& event = m_events[m_next++];
				Log("dispatch elapsedMs=" + std::to_string(m_elapsedMs)
					+ " simMs=" + std::to_string(game.getTimeInMillis())
					+ " line=" + std::to_string(event.line) + " event=" + event.description + Snapshot(game, objects));
				if (event.kind == Quit)
				{
					ReleaseHeld(input);
					m_complete = true;
					game.RequestQuit();
					Log("completed reason=quit");
					return true;
				}
				Dispatch(game, input, event);
			}
			if (m_next == m_events.size())
			{
				ReleaseHeld(input);
				m_complete = true;
				Log("completed reason=end-of-file");
			}
			return false;
		}

	private:
		void Load(const char* path)
		{
			std::ifstream file(path);
			if (!file)
			{
				Log("rejected reason=cannot-open path=" + std::string(path));
				return;
			}
			std::string line;
			int lineNumber = 0;
			while (std::getline(file, line))
			{
				++lineNumber;
				if (!line.empty() && line.back() == '\r') line.pop_back();
				if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
				const size_t comment = line.find('#');
				if (comment != std::string::npos) line.erase(comment);
				if (line.find_first_not_of(" \t\r") == std::string::npos) continue;
				Event event;
				event.line = lineNumber;
				if (line.size() > 256 || m_events.size() >= 4096 || !Parse(line, event)
					|| (!m_events.empty() && event.timeMs < m_events.back().timeMs)
					|| (!m_events.empty() && m_events.back().kind == Quit))
				{
					Log("rejected reason=invalid-format-or-order line=" + std::to_string(lineNumber));
					m_events.clear();
					return;
				}
				m_events.push_back(event);
			}
			if (file.bad() || m_events.empty())
			{
				Log("rejected reason=empty-or-read-error");
				m_events.clear();
				return;
			}
			m_enabled = true;
			Log("loaded events=" + std::to_string(m_events.size())
				+ " waiting=" + (m_waitForPlayer ? std::string("player") : std::string("update-loop"))
				+ " path=" + path);
		}

		bool Parse(const std::string& line, Event& event)
		{
			std::istringstream stream(line);
			std::string command;
			if (!(stream >> event.timeMs >> command) || event.timeMs < 0 || event.timeMs > 3600000) return false;
			if (command == "key_down" || command == "key_up" || command == "key_press")
			{
				std::string name;
				if (!(stream >> name)) return false;
				if (name.compare(0, 3, "KC_") == 0) name.erase(0, 3);
				for (const KeyName& entry : kKeys)
					if (name == entry.name) { event.key = entry.key; event.text = entry.text; break; }
				if (event.key == OIS::KC_UNASSIGNED) return false;
				event.kind = command == "key_down" ? KeyDown : (command == "key_up" ? KeyUp : KeyPress);
			}
			else if (command == "mouse_down" || command == "mouse_up" || command == "mouse_move" || command == "wheel")
			{
				if (!(stream >> event.x >> event.y) || event.x < 0 || event.y < 0 || event.x > 16384 || event.y > 16384) return false;
				if (command == "mouse_move") event.kind = MouseMove;
				else
				{
					if (!(stream >> event.value)) return false;
					if (command == "wheel")
					{
						if (event.value == 0 || event.value < -12000 || event.value > 12000) return false;
						event.kind = Wheel;
					}
					else
					{
						if (event.value < 0 || event.value > 2) return false;
						event.kind = command == "mouse_down" ? MouseDown : MouseUp;
					}
				}
			}
			else if (command == "quit") event.kind = Quit;
			else return false;
			std::string trailing;
			if (stream >> trailing) return false;
			event.description = line;
			return true;
		}

		void Dispatch(GameManager& game, InputManager* input, const Event& event)
		{
			// Dispatch through the same virtual listener entrypoints used by OIS capture.
			OIS::KeyListener* keys = input;
			OIS::MouseListener* mouse = input;
			if (event.kind == KeyDown || event.kind == KeyUp || event.kind == KeyPress)
			{
				const OIS::KeyEvent key(nullptr, event.key, event.text);
				if (event.kind != KeyUp) { keys->keyPressed(key); m_heldKeys.insert(event.key); }
				if (event.kind != KeyDown) { keys->keyReleased(key); m_heldKeys.erase(event.key); }
				return;
			}
			Ogre::Viewport* viewport = game.getCamera() != nullptr ? game.getCamera()->getViewport() : nullptr;
			if (viewport != nullptr)
			{
				m_mouse.width = static_cast<int>(viewport->getActualWidth());
				m_mouse.height = static_cast<int>(viewport->getActualHeight());
			}
			m_mouse.X.rel = event.x - m_mouse.X.abs;
			m_mouse.Y.rel = event.y - m_mouse.Y.abs;
			m_mouse.X.abs = event.x;
			m_mouse.Y.abs = event.y;
			m_mouse.Z.rel = event.kind == Wheel ? event.value : 0;
			m_mouse.Z.abs += m_mouse.Z.rel;
			if (event.kind == MouseDown) m_mouse.buttons |= 1 << event.value;
			if (event.kind == MouseUp) m_mouse.buttons &= ~(1 << event.value);
			const OIS::MouseEvent move(nullptr, m_mouse);
			if (event.kind == MouseDown) mouse->mousePressed(move, static_cast<OIS::MouseButtonID>(event.value));
			else if (event.kind == MouseUp) mouse->mouseReleased(move, static_cast<OIS::MouseButtonID>(event.value));
			else mouse->mouseMoved(move);
			m_mouse.X.rel = m_mouse.Y.rel = m_mouse.Z.rel = 0;
		}

		void ReleaseHeld(InputManager* input)
		{
			OIS::KeyListener* keys = input;
			for (OIS::KeyCode key : m_heldKeys) keys->keyReleased(OIS::KeyEvent(nullptr, key, 0));
			m_heldKeys.clear();
			OIS::MouseListener* mouse = input;
			for (int button = 0; button <= 2; ++button)
				if ((m_mouse.buttons & (1 << button)) != 0)
				{
					m_mouse.buttons &= ~(1 << button);
					mouse->mouseReleased(OIS::MouseEvent(nullptr, m_mouse), static_cast<OIS::MouseButtonID>(button));
				}
		}

		bool m_enabled = false;
		bool m_waitForPlayer = true;
		bool m_started = false;
		bool m_complete = false;
		long long m_elapsedMs = 0;
		size_t m_next = 0;
		std::vector<Event> m_events;
		std::set<OIS::KeyCode> m_heldKeys;
		OIS::MouseState m_mouse;
	};
}

bool InputReplay::Update(GameManager& game, ObjectManager* objects, InputManager* input, int deltaMs)
{
	static Replay replay;
	return replay.Update(game, objects, input, deltaMs);
}
