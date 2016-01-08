/********************************************************************
	created:	2014/06/14
	created:	14:6:2014   2:31
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpConsole.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpConsole
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Console
*********************************************************************/
#ifndef __DP_CONSOLE__
#define __DP_CONSOLE__

#include "DoPixel.h"
#include <sstream>

namespace dopixel
{
	class LogStream
	{
	public:
		LogStream() : level_(Log::Info) {}

		void SetLevel(int level) { level_ = level; }

		template<typename T>
		LogStream& operator<<(const T& t)
		{
			LOGSCOPE_NEWLINE(false);

			std::ostringstream oss;
			oss << t;
			g_Log.WriteBuf(level_, "%s", oss.str().c_str());
			return *this;
		}
	private:
		int level_;
	};

	extern LogStream g_LogS;
}

#endif
