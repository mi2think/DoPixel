/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:47
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_vector4.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_vector4
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Vector4
*********************************************************************/
#include "DpVector4.h"
#include "DpUnitTest.h"
using namespace dopixel;
using namespace dopixel::math;

DPTEST(Vector4)
{
	Vector4f v1(2.3f,4.1f,8.4f,1);
	Vector4f v2(45.87f,72.31f,1.41f,1);

	Vector4f v3 = v1 + v2;
	EXPECT_EQ(v3, Vector4f(48.17f,76.41f,9.81f,2));

	Vector4f v4 = v1;
	v4 += v2;
	EXPECT_EQ(v4, v3);

	Vector4f v5 = v3;
	v5 *= 3.79f;
	EXPECT_EQ(v5,Vector4f(182.564285f, 289.593872f, 37.1798973f, 7.58f));

	Vector4f v6 = v3 * 3.79f;
	EXPECT_EQ(v6, v5);

	Vector4f v7(2.5f,41,5,1);
	Vector4f v8 = v7 / 0.71f;
	EXPECT_EQ(v8, Vector4f(3.52112f,57.74647f,7.042253f,1.40845070f));
	
	Vector4f v9 = v7;
	v9 /= 0.71f;
	EXPECT_EQ(v8, v9);

	Vector4f v10 = v9;
	v10.Zero();
	EXPECT_EQ(v10, Vector4f(0,0,0,0));

	Vector4f v11(2,1,3,1);
	v11.Normalize();
	EXPECT_EQ(v11, Vector4f(0.516397f,0.258198f,0.7745966f, 0.258198f));
}
