/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:48
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_quaternion.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_quaternion
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Quaternion
*********************************************************************/
#include "DpQuaternion.h"
#include "DpVector3.h"
#include "DpUnitTest.h"
using namespace dopixel;
using namespace dopixel::math;

Vector3f RotateVector(const Vector3f& v, const Vector3f& axis, float angle)
{
	Vector3f _v = QuaternionRotateVector(v, axis, angle);
	return _v;
}


DPTEST(Quaternion)
{
	{
		Vector3f axis(0, 1, 0);
		Vector3f v(1, 0, 0);

		Vector3f _v = RotateVector(v, axis, angle2radian(90));
		EXPECT_EQ(_v, Vector3f(0, 0, -1));
	}

	{
		Vector3f axis(0, 1, 0);
		Vector3f v(1, 0, 0);

		Vector3f _v = RotateVector(v, axis, angle2radian(45));
		//EXPECT_EQ(_v, Vector3f(0, 0, -1));
	}

	{
		Vector3f axis(0, 1, 0);
		Vector3f v(1, 0, 0);

		Vector3f _v = RotateVector(v, axis, angle2radian(135));
		//EXPECT_EQ(_v, Vector3f(0, 0, -1));
	}
}
