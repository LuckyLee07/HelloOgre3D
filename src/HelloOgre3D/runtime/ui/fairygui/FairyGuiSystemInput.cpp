#include "ui/fairygui/FairyGuiSystemInternal.h"

bool FairyGuiSystem::InjectMouseMove(int x, int y)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const float previousX = m_hasLastMousePosition ? m_lastMouseX : mouseX;
	const float previousY = m_hasLastMousePosition ? m_lastMouseY : mouseY;

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, previousX, previousY);
	m_pRoot->getInputProcessor()->touchMove(&touch, nullptr);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	return IsMouseOnUi(mouseX, mouseY) || m_leftMouseDownOnUi;
}

bool FairyGuiSystem::InjectMouseDown(int x, int y, int button)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const bool mouseOnUi = IsMouseOnUi(mouseX, mouseY);
	LogInputHit("down", x, y, mouseX, mouseY, m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera()));

	if (button != 0)
	{
		m_lastMouseX = mouseX;
		m_lastMouseY = mouseY;
		m_hasLastMousePosition = true;
		m_leftMouseDownOnUi = mouseOnUi;
		return mouseOnUi;
	}

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, mouseX, mouseY);
	m_pRoot->getInputProcessor()->touchDown(&touch, nullptr);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	m_leftMouseDownOnUi = mouseOnUi;
	return mouseOnUi;
}

bool FairyGuiSystem::InjectMouseUp(int x, int y, int button)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const float previousX = m_hasLastMousePosition ? m_lastMouseX : mouseX;
	const float previousY = m_hasLastMousePosition ? m_lastMouseY : mouseY;
	const bool consumed = IsMouseOnUi(mouseX, mouseY) || m_leftMouseDownOnUi;
	fairygui::GObject* hitTarget = m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera());
	LogInputHit("up", x, y, mouseX, mouseY, hitTarget);

	if (button != 0)
	{
		if (consumed && hitTarget != nullptr && hitTarget != m_pRoot)
			hitTarget->bubbleEvent(button == 2 ? fairygui::UIEventType::MiddleClick : fairygui::UIEventType::RightClick);
		m_lastMouseX = mouseX;
		m_lastMouseY = mouseY;
		m_hasLastMousePosition = true;
		m_leftMouseDownOnUi = false;
		return consumed;
	}

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, previousX, previousY);
	m_pRoot->getInputProcessor()->touchUp(&touch, nullptr);
	if (consumed)
		FocusTextInput(FindTextInputTarget(hitTarget));
	else
		ClearObjectHandleFocus();

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	m_leftMouseDownOnUi = false;
	return consumed;
}

bool FairyGuiSystem::InjectMouseWheel(int x, int y, int wheelDelta)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr || wheelDelta == 0)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	fairygui::GObject* target = m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera());
	const bool mouseOnUi = target != nullptr && target != m_pRoot;
	LogInputHit("wheel", x, y, mouseX, mouseY, target);

	if (mouseOnUi)
	{
		cocos2d::EventMouse event;
		event.setCursorPosition(mouseX, mouseY);
		event.setScrollData(0.0f, static_cast<float>(wheelDelta));
		m_pRoot->getInputProcessor()->mouseWheel(&event);
	}

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	return mouseOnUi;
}

bool FairyGuiSystem::InjectLogicalMouseMove(int x, int y)
{
	return InjectMouseMove(ToRawInputX(x), ToRawInputY(y));
}

bool FairyGuiSystem::InjectLogicalMouseDown(int x, int y, int button)
{
	return InjectMouseDown(ToRawInputX(x), ToRawInputY(y), button);
}

bool FairyGuiSystem::InjectLogicalMouseUp(int x, int y, int button)
{
	return InjectMouseUp(ToRawInputX(x), ToRawInputY(y), button);
}

bool FairyGuiSystem::InjectLogicalMouseWheel(int x, int y, int wheelDelta)
{
	return InjectMouseWheel(ToRawInputX(x), ToRawInputY(y), wheelDelta);
}

bool FairyGuiSystem::InjectKeyPressed(int keyCode, int keyText)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		return false;
	}

	input->dispatchEvent(fairygui::UIEventType::KeyDown, nullptr, cocos2d::Value(keyCode));
	if (keyCode == OIS_KC_TAB)
		return false;
	if (keyCode == OIS_KC_ESCAPE)
	{
		ClearObjectHandleFocus();
		return true;
	}
	return ApplyTextInputKey(input, keyCode, keyText);
}

bool FairyGuiSystem::InjectKeyReleased(int keyCode, int keyText)
{
	(void)keyText;
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		return false;
	}

	input->dispatchEvent(fairygui::UIEventType::KeyUp, nullptr, cocos2d::Value(keyCode));
	return true;
}

bool FairyGuiSystem::FocusTextInput(fairygui::GTextInput* input)
{
	if (input == nullptr)
	{
		ClearObjectHandleFocus();
		return false;
	}

	const int ownerHandle = FindOwnerHandleForObject(input);
	const int inputHandle = GetOrCreateObjectAlias(ownerHandle, input);
	if (inputHandle <= 0)
		return false;

	if (m_focusedObjectHandle != inputHandle)
	{
		ClearObjectHandleFocus();
		m_focusedObjectHandle = inputHandle;
		input->dispatchEvent(fairygui::UIEventType::Enter);
	}
	if (m_textInputCarets.find(inputHandle) == m_textInputCarets.end())
		m_textInputCarets[inputHandle] = GetUtf8CodepointCount(input->getText());
	UpdateNativeImeCandidatePosition();
	return true;
}

bool FairyGuiSystem::ApplyTextInputKey(fairygui::GTextInput* input, int keyCode, int keyText)
{
	if (input == nullptr)
		return false;

	const int inputHandle = m_focusedObjectHandle;
	std::string text = input->getText();
	size_t caret = GetUtf8CodepointCount(text);
	std::map<int, size_t>::iterator caretIt = m_textInputCarets.find(inputHandle);
	if (caretIt != m_textInputCarets.end())
		caret = ClampUtf8Caret(text, caretIt->second);

	bool changed = false;
	if (keyCode == OIS_KC_HOME)
	{
		caret = 0;
	}
	else if (keyCode == OIS_KC_END)
	{
		caret = GetUtf8CodepointCount(text);
	}
	else if (keyCode == OIS_KC_LEFT)
	{
		if (caret > 0)
			--caret;
	}
	else if (keyCode == OIS_KC_RIGHT)
	{
		const size_t count = GetUtf8CodepointCount(text);
		if (caret < count)
			++caret;
	}
	else if (keyCode == OIS_KC_BACK)
	{
		changed = RemoveUtf8CodepointBeforeCaret(text, caret);
	}
	else if (keyCode == OIS_KC_DELETE)
	{
		changed = RemoveUtf8CodepointAtCaret(text, caret);
	}
	else if (keyCode == OIS_KC_RETURN || keyCode == OIS_KC_NUMPADENTER)
	{
		m_textInputCarets[inputHandle] = caret;
		input->dispatchEvent(fairygui::UIEventType::Submit);
		return true;
	}
	else if (IsPrintableKeyText(keyText))
	{
		changed = InsertUtf8CodepointAtCaret(text, caret, static_cast<unsigned int>(keyText));
	}

	if (changed)
	{
		input->setText(text);
		input->dispatchEvent(fairygui::UIEventType::Changed);
	}
	m_textInputCarets[inputHandle] = caret;
	return true;
}

bool FairyGuiSystem::ApplyTextInputUtf8Text(fairygui::GTextInput* input, const std::string& committedText)
{
	if (input == nullptr || committedText.empty())
		return false;

	const int inputHandle = m_focusedObjectHandle;
	std::string text = input->getText();
	size_t caret = GetUtf8CodepointCount(text);
	std::map<int, size_t>::iterator caretIt = m_textInputCarets.find(inputHandle);
	if (caretIt != m_textInputCarets.end())
		caret = ClampUtf8Caret(text, caretIt->second);

	if (!InsertUtf8TextAtCaret(text, caret, committedText))
		return false;

	input->setText(text);
	input->dispatchEvent(fairygui::UIEventType::Changed);
	m_textInputCarets[inputHandle] = caret;
	return true;
}

void FairyGuiSystem::EndImeComposition(bool countEnd)
{
	if (countEnd && (m_imeStats.compositionActive || !m_imeStats.compositionText.empty() || m_imeStats.candidateOpen))
		++m_imeStats.compositionEndCount;
	m_imeStats.compositionActive = false;
	m_imeStats.candidateOpen = false;
	m_imeStats.compositionText.clear();
	m_imeStats.candidateCount = 0;
	m_imeStats.candidateSelection = -1;
}

bool FairyGuiSystem::InjectImeCompositionText(const std::string& text)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		EndImeComposition(true);
		return false;
	}

	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.compositionActive = !text.empty();
	m_imeStats.compositionText = text;
	++m_imeStats.compositionUpdateCount;
	UpdateNativeImeCandidatePosition();
	return true;
}

bool FairyGuiSystem::InjectImeCommitText(const std::string& text)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		EndImeComposition(true);
		return false;
	}

	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.commitText = text;
	++m_imeStats.compositionCommitCount;
	EndImeComposition(false);
	return ApplyTextInputUtf8Text(input, text);
}

bool FairyGuiSystem::ClearImeCompositionText()
{
	CancelNativeImeComposition();
	EndImeComposition(true);
	return true;
}

std::string FairyGuiSystem::GetImeDebugString() const
{
	std::ostringstream stream;
	stream << "active=" << (m_imeStats.compositionActive ? 1 : 0)
		<< " candidate=" << (m_imeStats.candidateOpen ? 1 : 0)
		<< " focus=" << m_imeStats.focusedHandle
		<< " compUpdates=" << m_imeStats.compositionUpdateCount
		<< " commits=" << m_imeStats.compositionCommitCount
		<< " ends=" << m_imeStats.compositionEndCount
		<< " candOpen=" << m_imeStats.candidateOpenCount
		<< " candClose=" << m_imeStats.candidateCloseCount
		<< " candChange=" << m_imeStats.candidateChangeCount
		<< " candCount=" << m_imeStats.candidateCount
		<< " candSelection=" << m_imeStats.candidateSelection
		<< " pos=" << m_imeStats.compositionX << "," << m_imeStats.compositionY
		<< " comp=\"" << m_imeStats.compositionText << "\""
		<< " commit=\"" << m_imeStats.commitText << "\"";
	return stream.str();
}

void FairyGuiSystem::InstallNativeImeHook()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeImeHookInstalled || m_pRenderWindow == nullptr)
		return;

	HWND hwnd = nullptr;
	m_pRenderWindow->getCustomAttribute("WINDOW", &hwnd);
	if (hwnd == nullptr)
		return;

	WNDPROC currentProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));
	if (currentProc == FairyGuiNativeImeWndProc)
	{
		std::map<HWND, WNDPROC>::iterator procIt = g_nativeImePreviousWndProcByWindow.find(hwnd);
		if (procIt == g_nativeImePreviousWndProcByWindow.end() || procIt->second == nullptr)
			return;
		m_nativeWindowHandle = hwnd;
		m_previousWindowProc = reinterpret_cast<void*>(procIt->second);
		g_nativeImeSystemsByWindow[hwnd] = this;
		m_nativeImeHookInstalled = true;
		return;
	}

	m_nativeWindowHandle = hwnd;
	m_previousWindowProc = reinterpret_cast<void*>(currentProc);
	g_nativeImePreviousWndProcByWindow[hwnd] = currentProc;
	g_nativeImeSystemsByWindow[hwnd] = this;
	SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(FairyGuiNativeImeWndProc));
	m_nativeImeHookInstalled = true;
#endif
}

void FairyGuiSystem::RemoveNativeImeHook()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (!m_nativeImeHookInstalled || m_nativeWindowHandle == nullptr)
		return;

	HWND hwnd = m_nativeWindowHandle;
	std::map<HWND, FairyGuiSystem*>::iterator systemIt = g_nativeImeSystemsByWindow.find(hwnd);
	if (systemIt != g_nativeImeSystemsByWindow.end() && systemIt->second != this)
	{
		m_nativeWindowHandle = nullptr;
		m_previousWindowProc = nullptr;
		m_nativeImeHookInstalled = false;
		return;
	}

	std::map<HWND, WNDPROC>::iterator procIt = g_nativeImePreviousWndProcByWindow.find(hwnd);
	const LONG_PTR currentProc = GetWindowLongPtr(hwnd, GWLP_WNDPROC);
	if (procIt != g_nativeImePreviousWndProcByWindow.end() && currentProc == reinterpret_cast<LONG_PTR>(FairyGuiNativeImeWndProc))
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(procIt->second));

	if (systemIt != g_nativeImeSystemsByWindow.end())
		g_nativeImeSystemsByWindow.erase(systemIt);
	g_nativeImePreviousWndProcByWindow.erase(hwnd);
	m_nativeWindowHandle = nullptr;
	m_previousWindowProc = nullptr;
	m_nativeImeHookInstalled = false;
#endif
}

bool FairyGuiSystem::HandleNativeImeMessage(unsigned int message, unsigned long long wParam, long long lParam, long long& result)
{
	result = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (!m_initialized || m_nativeWindowHandle == nullptr)
		return false;

	if (message == WM_IME_STARTCOMPOSITION)
	{
		m_imeStats.compositionActive = true;
		m_imeStats.focusedHandle = m_focusedObjectHandle;
		UpdateNativeImeCandidatePosition();
		return false;
	}

	if (message == WM_IME_ENDCOMPOSITION)
	{
		EndImeComposition(true);
		return false;
	}

	if (message == WM_IME_NOTIFY)
	{
		HIMC context = ImmGetContext(m_nativeWindowHandle);
		const DWORD notify = static_cast<DWORD>(wParam);
		bool updateCandidatePosition = false;
		if (notify == IMN_OPENCANDIDATE)
		{
			m_imeStats.candidateOpen = true;
			++m_imeStats.candidateOpenCount;
			ReadImeCandidateInfo(context, m_imeStats.candidateCount, m_imeStats.candidateSelection);
			updateCandidatePosition = true;
		}
		else if (notify == IMN_CLOSECANDIDATE)
		{
			m_imeStats.candidateOpen = false;
			++m_imeStats.candidateCloseCount;
			m_imeStats.candidateCount = 0;
			m_imeStats.candidateSelection = -1;
		}
		else if (notify == IMN_CHANGECANDIDATE)
		{
			++m_imeStats.candidateChangeCount;
			ReadImeCandidateInfo(context, m_imeStats.candidateCount, m_imeStats.candidateSelection);
			updateCandidatePosition = true;
		}
		if (context != nullptr)
			ImmReleaseContext(m_nativeWindowHandle, context);
		if (updateCandidatePosition)
			UpdateNativeImeCandidatePosition();
		return false;
	}

	if (message != WM_IME_COMPOSITION)
		return false;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return false;

	const LPARAM flags = static_cast<LPARAM>(lParam);
	bool consumed = false;
	bool updateCandidatePosition = false;
	if ((flags & GCS_COMPSTR) != 0)
	{
		m_imeStats.focusedHandle = m_focusedObjectHandle;
		m_imeStats.compositionText = ReadImeCompositionString(context, GCS_COMPSTR);
		m_imeStats.compositionActive = !m_imeStats.compositionText.empty();
		++m_imeStats.compositionUpdateCount;
		updateCandidatePosition = true;
	}

	if ((flags & GCS_RESULTSTR) != 0)
	{
		const std::string resultText = ReadImeCompositionString(context, GCS_RESULTSTR);
		ImmReleaseContext(m_nativeWindowHandle, context);
		consumed = !resultText.empty() && InjectImeCommitText(resultText);
		result = 0;
		return consumed;
	}

	ImmReleaseContext(m_nativeWindowHandle, context);
	if (updateCandidatePosition)
		UpdateNativeImeCandidatePosition();
	return consumed;
#else
	(void)message;
	(void)wParam;
	(void)lParam;
	return false;
#endif
}

void FairyGuiSystem::CancelNativeImeComposition()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeWindowHandle == nullptr)
		return;
	if (!m_imeStats.compositionActive && m_imeStats.compositionText.empty() && !m_imeStats.candidateOpen)
		return;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return;
	ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
	ImmReleaseContext(m_nativeWindowHandle, context);
#endif
}

void FairyGuiSystem::UpdateNativeImeCandidatePosition()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeWindowHandle == nullptr)
		return;

	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
		return;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return;

	const cocos2d::Rect bounds = input->localToGlobal(cocos2d::Rect(cocos2d::Vec2::ZERO, input->getSize()));
	const float candidateY = m_screenHeight > 0 ? static_cast<float>(m_screenHeight) - bounds.getMinY() : bounds.getMinY();
	const int rawX = ToRawInputX(ClampClientCoordinate(bounds.getMinX(), m_screenWidth));
	const int rawY = ToRawInputY(ClampClientCoordinate(candidateY, m_screenHeight));
	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.compositionX = rawX;
	m_imeStats.compositionY = rawY;

	COMPOSITIONFORM composition = {};
	composition.dwStyle = CFS_POINT;
	composition.ptCurrentPos.x = rawX;
	composition.ptCurrentPos.y = rawY;
	ImmSetCompositionWindow(context, &composition);

	CANDIDATEFORM candidate = {};
	candidate.dwIndex = 0;
	candidate.dwStyle = CFS_CANDIDATEPOS;
	candidate.ptCurrentPos = composition.ptCurrentPos;
	ImmSetCandidateWindow(context, &candidate);
	ImmReleaseContext(m_nativeWindowHandle, context);
#endif
}

void FairyGuiSystem::ConvertMousePosition(int x, int y, float& outX, float& outY) const
{
	const float scaleX = GetInputScaleX();
	const float scaleY = GetInputScaleY();
	outX = static_cast<float>(x) / scaleX;
	const float logicalY = static_cast<float>(y) / scaleY;
	outY = m_screenHeight > 0 ? static_cast<float>(m_screenHeight) - logicalY : logicalY;
}

float FairyGuiSystem::GetInputScaleX() const
{
	return GetWindowInputScale(m_pRenderWindow, m_screenWidth, true);
}

float FairyGuiSystem::GetInputScaleY() const
{
	return GetWindowInputScale(m_pRenderWindow, m_screenHeight, false);
}

int FairyGuiSystem::ToRawInputX(int x) const
{
	return ToRawInputCoordinate(x, GetInputScaleX());
}

int FairyGuiSystem::ToRawInputY(int y) const
{
	return ToRawInputCoordinate(y, GetInputScaleY());
}

bool FairyGuiSystem::IsMouseOnUi(float x, float y) const
{
	if (m_pRoot == nullptr)
		return false;

	fairygui::GObject* target = m_pRoot->hitTest(cocos2d::Vec2(x, y), cocos2d::Camera::getVisitingCamera());
	return target != nullptr && target != m_pRoot;
}


