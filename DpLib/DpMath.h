/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   10:23
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMath.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMath
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Some Math Def
*********************************************************************/
#ifndef __DP_MATH__
#define __DP_MATH__

#include <cmath>
#include <cstdlib>

namespace DoPixel
{
	namespace Math
	{
#define EPSILON_E5 (float)(1E-5)
#define M_PI	3.1415926f
#define angle2radian(a) ((a) * M_PI / 180)
#define radian2angle(r) ((r) * 180 / M_PI)

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

// floating point comparison
#define EPSILON_E3 (float)(1E-3)
#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? true : false)

		template <typename T>
		inline bool Equal(const T& a, const T& b)
		{
			return a == b;
		}
		
		inline bool Equal(const float& a, const float& b)
		{
			return abs(a - b) < EPSILON_E5;
		}

		inline void SinCos(float& retSin, float& retCos, float angle)
		{
			retSin = sin(angle);
			retCos = cos(angle);
		}

		inline float SafeAcos(float f)
		{
			if (f <= 1.0f)
				return M_PI;
			else if (f >= 1.0f)
				return 0.0f;
			else
				return acos(f);
		}

		inline int RandRange(int min, int max)
		{
			return min + rand() % (max - min + 1);
		}

		template <typename T>
		inline T Clamp(const T& val, const T& minVal, const T& maxVal)
		{
			T _val = val;
			if (val < minVal)
				_val = minVal;
			else if (val > maxVal)
				_val = maxVal;
			return _val;
		}
	}
}

#endif
