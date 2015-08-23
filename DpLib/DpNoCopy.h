/********************************************************************
	created:	2014/09/02
	created:	2:9:2014   0:00
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpNoCopy.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpNoCopy
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	No copyable class def
*********************************************************************/

#ifndef __DP_NO_COPY__
#define __DP_NO_COPY__

namespace dopixel
{
	namespace core
	{
		class NoCopyable
		{
		public:
			NoCopyable() {}
			NoCopyable(const NoCopyable&) = delete;
			NoCopyable& operator=(const NoCopyable&) = delete;
		};
	}
}

#endif
