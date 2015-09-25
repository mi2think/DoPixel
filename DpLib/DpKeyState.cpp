/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:52
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpKeyState.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpKeyState
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Key State
*********************************************************************/
#include "DpKeyState.h"

namespace dopixel
{
	namespace core
	{
		bool s_KeyState[KEY_CODES_COUNT] = { false };

		bool KeyState::GetKeyState(int key)
		{
			ASSERT(key < KEY_CODES_COUNT);
			return s_KeyState[key];
		}

		void KeyState::SetKeyState(int key, bool pressed)
		{
			ASSERT(key < KEY_CODES_COUNT);
			s_KeyState[key] = pressed;
		}

		void KeyState::OnKeyPress(const KeyPressEvent& event)
		{
			int key = event.GetKey();
			SetKeyState(key, true);
		}

		void KeyState::OnKeyRelease(const KeyReleaseEvent& event)
		{
			int key = event.GetKey();
			SetKeyState(key, false);
		}
	}
}