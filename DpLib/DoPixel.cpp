/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:31
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DoPixel.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DoPixel
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	DoPixel
*********************************************************************/
#include "DoPixel.h"
#include <cstdarg>

namespace dopixel
{
	const char* str_format(const char* format, ...)
	{
		static char buffer[1024];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, format, ap);
		va_end(ap);
		buffer[1023] = 0;
		return buffer;
	}

}






