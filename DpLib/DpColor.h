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

#include "DpMath.h"

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

			void Set(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255)
			{
				r = _r;
				g = _g;
				b = _b;
				a = _a;
			}

			Color& operator*=(float k)
			{
				r = (unsigned char)Math::Clamp(r * k, 0.0f, 255.0f);
				g = (unsigned char)Math::Clamp(g * k, 0.0f, 255.0f);
				b = (unsigned char)Math::Clamp(b * k, 0.0f, 255.0f);
				return *this;
			}

			Color Interpolate(const Color& color1, float t) const
			{
				float a0 = a;
				float r0 = r;
				float g0 = g;
				float b0 = b;
				
				float a01 = float(color1.a) - a0;
				float r01 = float(color1.r) - r0;
				float g01 = float(color1.g) - g0;
				float b01 = float(color1.b) - b0;

				Color color;
				color.a = (unsigned char)Math::Clamp(a0 + a01 * t, 0.0f, 255.0f);
				color.r = (unsigned char)Math::Clamp(r0 + r01 * t, 0.0f, 255.0f);
				color.g = (unsigned char)Math::Clamp(g0 + g01 * t, 0.0f, 255.0f);
				color.b = (unsigned char)Math::Clamp(b0 + b01 * t, 0.0f, 255.0f);
				return color;
			}

			static const Color white;
			static const Color gray;
			static const Color black;
			static const Color red;
			static const Color green;
			static const Color blue;
		};

		inline Color operator*(const Color& color, float k)
		{
			Color c = color;
			c *= k;
			return c;
		}

		inline Color operator*(float k, const Color& color)
		{
			Color c = color;
			c *= k;
			return c;
		}
	}
}

#endif