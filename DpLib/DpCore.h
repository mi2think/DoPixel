/********************************************************************
	created:	2014/09/07
	created:	7:9:2014   21:41
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpCore.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpCore
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Core
*********************************************************************/

#ifndef __DP_CORE__
#define __DP_CORE__

#include "DpScopeGuard.h"

#include <cassert>
#include <cstdarg>
#include <cstring>

namespace DoPixel
{
	namespace Core
	{
#define SAFE_DELETE(p)		do { if (p != nullptr) { delete p; p = nullptr } } while(0)
#define SAFE_DELETEARRAY(p) do { if (p != nullptr) { delete[] p; p = nullptr; } } while(0)
#define SAFE_RELEASE(p)		do { if (p != nullptr) { p->Release(); p = nullptr; } } while(0)


		inline void DebugTrace(const char* format, ...)
		{
			static char buffer[1024];
			va_list ap;
			va_start(ap, format);
			vsprintf_s(buffer, format, ap);
			va_end(ap);
			buffer[1023] = 0;
			printf(buffer);
		}

#define DEBUG_TRACE		DebugTrace
#define DEBUG_VS_TRACE	OutputDebugString

		inline const char* avar(const char* format, ...)
		{
			static char buffer[1024];
			va_list ap;
			va_start(ap, format);
			vsprintf_s(buffer, format, ap);
			va_end(ap);
			buffer[1023] = 0;
			return buffer;
		};

		// string copy
		inline void strncpy_s(char* pDest, const char* pSrc, int destSize)
		{
			if (!pDest || !pSrc)
				return;
			int len = strlen(pSrc) + 1;
			if (len > destSize)
				len = destSize;
			memcpy(pDest, pSrc, len);
			*(pDest + len - 1) = 0;
		}
	}
}

#endif
