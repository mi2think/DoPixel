/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:47
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpKeyState.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpKeyState
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Key State
*********************************************************************/
#ifndef __DP_KEY_STATE__
#define __DP_KEY_STATE__

#include "DpKeyEvent.h"

namespace dopixel
{
	namespace core
	{
		class KeyState
		{
		public:
			static bool GetKeyState(int key);
			static void SetKeyState(int key, bool pressed);
			static void OnKeyPress(const KeyPressEvent& event);
			static void OnKeyRelease(const KeyReleaseEvent& event);
		};
	}
}

#endif
