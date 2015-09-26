/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:47
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpInputState.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpInputState
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Input State
*********************************************************************/
#ifndef __DP_KEY_STATE__
#define __DP_KEY_STATE__

#include "DpKeyEvent.h"
#include "DpMouseEvent.h"

namespace dopixel
{
	namespace core
	{
		class InputState
		{
		public:
			// Key State
			static bool GetKeyState(int key);
			static void SetKeyState(int key, bool pressed);
			static void OnKeyPress(const KeyPressEvent& event);
			static void OnKeyRelease(const KeyReleaseEvent& event);

			// Mouse State
			static bool GetMouseState(MouseButtonType buttonType);
			static void SetMouseState(MouseButtonType buttonType, bool pressed);
			static void OnMousePress(const MousePressEvent& event);
			static void OnMouseRelease(const MouseReleaseEvent& event);
		};


	}
}

#endif
