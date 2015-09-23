/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   20:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpEvent.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpEvent
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Event
*********************************************************************/
#include "DpEvent.h"

namespace dopixel
{
	namespace core
	{
		const char _BaseEventName[] = "";

		void EventBroadcaster::RegisterEventListener(IEventListener* listener)
		{
			if (listener == nullptr)
				return;

			auto it = std::find(listeners_.begin(), listeners_.end(), listener);
			if (it == listeners_.end())
			{
				listeners_.push_back(listener);
			}
		}

		void EventBroadcaster::UnregisterEventListener(IEventListener* listener)
		{
			if (listener == nullptr)
				return;

			auto it = std::find(listeners_.begin(), listeners_.end(), listener);
			if (it != listeners_.end())
			{
				listeners_.erase(it);
			}
		}

		bool EventBroadcaster::PostEvent(const Event& event)
		{
			bool handled = false;
			for (auto listener : listeners_)
			{
				handled |= listener->OnEvent(event);
			}
			return handled;
		}
	}
}