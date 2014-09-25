/********************************************************************
	created:	2014/09/02
	created:	2:9:2014   0:00
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpNoCopy.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpNoCopy
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	No copy class def
*********************************************************************/

#ifndef __DP_NO_COPY__
#define __DP_NO_COPY__

namespace DoPixel
{
	namespace Core
	{
		class NoCopy
		{
		public:
			NoCopy() {}
			NoCopy(const NoCopy&) = delete;
			NoCopy& operator=(const NoCopy&) = delete;
		};
	}
}

#endif
