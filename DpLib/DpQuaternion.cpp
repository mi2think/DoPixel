/********************************************************************
	created:	2014/06/22
	created:	22:6:2014   18:00
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpQuaternion.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpQuaternion
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	For Dependence
*********************************************************************/
#include "DpQuaternion.h"

namespace dopixel
{
	namespace math
	{
		Quaternion QuaternionRotationMatrix(const Matrix43f& _m)
		{
			float j1 = _m.m11 + _m.m22 + _m.m33;
			float j2 = _m.m11 - _m.m22 - _m.m33;
			float j3 =-_m.m11 + _m.m22 - _m.m33;
			float j4 =-_m.m11 - _m.m22 + _m.m33;

			float big = j1;
			int bigIndex = 1;

			if (j2 > big)
			{
				bigIndex = 2;
				big = j2;
			}

			if (j3 > big)
			{
				bigIndex = 3;
				big = j3;
			}

			if (j4 > big)
			{
				bigIndex = 4;
				big = j4;
			}

			float bigVal = Sqrt(big + 1.0f) * 0.5f;
			float f = 0.25f / bigVal;

			Quaternion q;

			switch (bigIndex)
			{
			case  1:
				q.w = bigVal;
				q.x = (_m.m23 - _m.m32) * f;
				q.y = (_m.m31 - _m.m13) * f;
				q.z = (_m.m12 - _m.m21) * f;
				break;
			case  2:
				q.w = (_m.m23 - _m.m32) * f;
				q.x = bigVal;
				q.y = (_m.m12 + _m.m21) * f;
				q.z = (_m.m31 + _m.m13) * f;
				break;
			case  3:
				q.w = (_m.m31 - _m.m13) * f;
				q.x = (_m.m12 + _m.m21) * f;
				q.y = bigVal;
				q.z = (_m.m23 + _m.m32) * f;
				break;
			case  4:
				q.w = (_m.m12 - _m.m21) * f;
				q.x = (_m.m31 + _m.m13) * f;
				q.y = (_m.m23 + _m.m32) * f;
				q.z = bigVal;
				break;
			}

			return q;
		}
	}
}
