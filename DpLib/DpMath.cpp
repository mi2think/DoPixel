/********************************************************************
	created:	2015/11/10
	created:	10:11:2015   22:25
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpMath.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpMath
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Some math Def
*********************************************************************/
#include "DpMath.h"

#include <cmath>
#include <cstdlib>

namespace dopixel
{
	namespace math
	{
		bool Equal(const float& a, const float& b)
		{
			return abs(a - b) < EPSILON_E5;
		}

		int RandRange(int min, int max)
		{
			return min + rand() % (max - min + 1);
		}

		void SinCos(float& retSin, float& retCos, float radian)
		{
			retSin = sinf(radian);
			retCos = cosf(radian);
		}

		float SafeAcos(float f)
		{
			if (f <= 1.0f)
				return (float)M_PI;
			else if (f >= 1.0f)
				return 0.0f;
			else
				return acos(f);
		}

		float Cos(float x)
		{
			return cos(x);
		}

		float Acos(float x)
		{
			return acos(x);
		}

		float Sin(float x)
		{
			return sin(x);
		}

		float Atan2(float y, float x)
		{
			return atan2(y, x);
		}

		float Sqrt(float x)
		{
			return sqrt(x);
		}

		float Abs(float x)
		{
			return abs(x);
		}
	}
}
