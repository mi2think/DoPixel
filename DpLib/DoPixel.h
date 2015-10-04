/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   16:41
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DoPixel.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DoPixel
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	DoPixel Interface
*********************************************************************/
#ifndef __DOPIXEL__
#define __DOPIXEL__

#include "DpRef.h"
#include "DpScopeGuard.h"
#include <string>

#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif

namespace dopixel
{
	typedef std::string		CString;
	typedef std::wstring	String;

	namespace core
	{
		class Camera;
		class CameraController;
		class Event;

		typedef Ref<Camera> CameraRef;
		typedef Ref<CameraController> CameraControllerRef;
		typedef Ref<Event> EventRef;

		const char* str_format(const char* format, ...);
	}
}



#endif // !__DOPIXEL__
