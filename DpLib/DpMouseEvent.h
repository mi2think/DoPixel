/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   1:13
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpMouseEvent.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpMouseEvent
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Mouse Event
*********************************************************************/
#ifndef __DP_MOUSE_EVENT__
#define __DP_MOUSE_EVENT__

#include "DpEvent.h"

namespace dopixel
{
	namespace core
	{
		enum MouseButtonType
		{
			MOUSE_LBUTTON,
			MOUSE_RBUTTON,
			MOUSE_MBUTTON,

			MOUSE_BUTTON_COUNT,
		};

		class MouseMoveEvent : public BaseEvent<Event::MOUSE_MOVE>
		{
		public:
			MouseMoveEvent(int x, int y) : x_(x), y_(y) {}

			int GetX() const { return x_; }
			int GetY() const { return y_; }
		private:
			int x_;
			int y_;
		};

		class MousePressEvent : public BaseEvent<Event::MOUSE_PRESS>
		{
		public:
			MousePressEvent(int x, int y, MouseButtonType buttonType)
				: x_(x)
				, y_(y)
				, buttonType_(buttonType)
			{}

			int GetX() const { return x_; }
			int GetY() const { return y_; }
			MouseButtonType GetButtonType() const { return buttonType_; }
		private:
			int x_;
			int y_;
			MouseButtonType buttonType_;
		};

		class MouseReleaseEvent : public BaseEvent<Event::MOUSE_RELEASE>
		{
		public:
			MouseReleaseEvent(int x, int y, MouseButtonType buttonType)
				: x_(x)
				, y_(y)
				, buttonType_(buttonType)
			{}

			int GetX() const { return x_; }
			int GetY() const { return y_; }
			MouseButtonType GetButtonType() const { return buttonType_; }
		private:
			int x_;
			int y_;
			MouseButtonType buttonType_;
		};

		class MouseWheelEvent : public BaseEvent<Event::MOUSE_WHELL>
		{
		public:
			MouseWheelEvent(float wheelDelta) : wheelDelta_(wheelDelta) {}

			float GetWheelDelta() const { return wheelDelta_; }
		private:
			float wheelDelta_;
		};


	}
}




#endif
