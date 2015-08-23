/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:46
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_vector2.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_vector2
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Vector2
*********************************************************************/
#include "DpVector2.h"
#include "DpUnitTest.h"
using namespace dopixel::math;
using namespace dopixel::dptest;

DPTEST(Vector2)
{
	Vector2f v1(2.5f, 3.0f);
	Vector2f v2(5.4f, 6.7f);

	Vector2f v3 = v1 + v2;
	EXPECT_EQ(v3, Vector2f(7.9f,9.7f));

	Vector2f v4 = v1;
	v4 += v2;
	EXPECT_EQ(v4, v3);

	Vector2f v5 = v1;
	v5 -= v2;
	EXPECT_EQ(v5, Vector2f(-2.9f,-3.7f));

	Vector2f v6 = v1 - v2;
	EXPECT_EQ(v5, v6);

	Vector2f v7 = -v1;
	EXPECT_EQ(v7, Vector2f(-2.5f,-3.0f));

	Vector2f v8 = v3 * 0.24f;
	EXPECT_EQ(v8, Vector2f(1.896f,2.328f));

	Vector2f v9 = v3;
	v9 *= 0.24f;
	EXPECT_EQ(v8, v9);

	Vector2f v10 = v8;
	v10 /= 0.78f;
	EXPECT_EQ(v10, Vector2f(2.43077f,2.98462f));

	Vector2f v11 = v8 / 0.78f;
	EXPECT_EQ(v10, v11);

	Vector2f v12 = v10;
	v12.Normalize();
	EXPECT_EQ(v12, Vector2f(0.631495f,0.77538f));

	EXPECT_TRUE(v12.IsNormalized());
	EXPECT_FALSE(v10.IsNormalized());

	Vector2f v13 = v9;
	float len_v13 = v13.Length();
	EXPECT_TRUE(fabs(3.00239904f - len_v13) < EPSILON_E5);

	float dp = DotProduct(v1, v2);
	EXPECT_TRUE(fabs(dp - 33.6f) < EPSILON_E5);
}
