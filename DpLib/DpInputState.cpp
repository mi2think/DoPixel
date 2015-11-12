/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:52
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpInputState.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpInputState
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Input State
*********************************************************************/
#include "DpInputState.h"

namespace dopixel
{
	static bool s_KeyState[KEY_CODES_COUNT] = { false };
	static bool s_MouseState[MOUSE_BUTTON_COUNT] = { false };

	bool InputState::GetKeyState(int key)
	{
		ASSERT(key < KEY_CODES_COUNT);
		return s_KeyState[key];
	}

	void InputState::SetKeyState(int key, bool pressed)
	{
		ASSERT(key < KEY_CODES_COUNT);
		s_KeyState[key] = pressed;
	}

	void InputState::OnKeyPress(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		SetKeyState(key, true);
	}

	void InputState::OnKeyRelease(const KeyReleaseEvent& event)
	{
		int key = event.GetKey();
		SetKeyState(key, false);
	}

	bool InputState::GetMouseState(MouseButtonType buttonType)
	{
		ASSERT(buttonType < MOUSE_BUTTON_COUNT);
		return s_MouseState[buttonType];
	}

	void InputState::SetMouseState(MouseButtonType buttonType, bool pressed)
	{
		ASSERT(buttonType < MOUSE_BUTTON_COUNT);
		s_MouseState[buttonType] = pressed;
	}

	void InputState::OnMousePress(const MousePressEvent& event)
	{
		MouseButtonType buttonType = event.GetButtonType();
		SetMouseState(buttonType, true);
	}

	void InputState::OnMouseRelease(const MouseReleaseEvent& event)
	{
		MouseButtonType buttonType = event.GetButtonType();
		SetMouseState(buttonType, false);
	}
}