#ifndef HELLO_GAME_INPUT_REPLAY_H
#define HELLO_GAME_INPUT_REPLAY_H

class GameManager;
class ObjectManager;
class InputManager;

namespace InputReplay
{
	// Optional internal OIS-listener replay. deltaMs continues while gameplay is paused.
	// No OS input is generated. Returns true only when a scheduled quit was dispatched.
	bool Update(GameManager& game, ObjectManager* objects, InputManager* input, int deltaMs);
}

#endif
