/********************************************************************
	created:	2014/06/29
	created:	29:6:2014   7:14
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpColor.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpColor
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Color
*********************************************************************/

#ifndef __DP_COLOR__
#define __DP_COLOR__

namespace DoPixel
{
	namespace Core
	{
		#define RGB24(r, g, b) (((r) << 16) + ((g) << 8) + (b))
		#define RGB32(a, r, g, b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))

		class Color
		{
		public:
			union
			{
				struct  
				{
					unsigned char b;
					unsigned char g;
					unsigned char r;
					unsigned char a;
				};
				unsigned int value;
			};
			Color() {}
			Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) : r(_r), g(_g), b(_b), a(_a) {}
			Color(unsigned int v) : value(v) {}

			static const Color white;
			static const Color gray;
			static const Color black;
			static const Color red;
			static const Color green;
			static const Color blue;
		};
	}
}

#endif