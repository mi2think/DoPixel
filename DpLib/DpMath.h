/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   10:23
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMath.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMath
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Some math Def
*********************************************************************/
#ifndef __DP_MATH__
#define __DP_MATH__

#include <limits>

namespace dopixel
{
	namespace math
	{
#define EPSILON_E5 (float)(1E-5)
#define M_PI 3.14159265358979323846
#define angle2radian(a) (float)((a) * M_PI / 180)
#define radian2angle(r) (float)((r) * 180 / M_PI)

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
		
		bool Equal(const float& a, const float& b);

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

		int RandRange(int min, int max);

		void SinCos(float& retSin, float& retCos, float radian);

		float SafeAcos(float f);

		float Cos(float x);
		float Acos(float x);
		float Sin(float x);
		float Atan2(float y, float x);

		float Sqrt(float x);
		float Abs(float x);
	}
}

#endif
