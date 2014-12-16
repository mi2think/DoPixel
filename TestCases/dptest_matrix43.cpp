/********************************************************************
	created:	2014/06/21
	created:	21:6:2014   1:03
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_matrix43.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_matrix43
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Matrix43
*********************************************************************/

#include "DpMatrix43.h"
#include "DpUnitTest.h"
#include "DpVector3.h"
#include "DpQuaternion.h"
using namespace DoPixel::Math;
using namespace DoPixel::DpTest;

DPTEST(Matrix43)
{
	Matrix43f m1(1, -5, 3, 0, -2, 6, 7, 2, -4, 1, 2, 3);
	Matrix43f m2(-8, 6, 1, 7, 0, -3, 2, 4, 5, -1, 2, -2);

	EXPECT_NE(m1, m2);

	Matrix43f m3 = m1 + m2;
	EXPECT_EQ(m3, Matrix43f(-7, 1, 4, 7, -2, 3, 9, 6, 1, 0, 4, 1));

	EXPECT_EQ(-m3,  Matrix43f(7, -1, -4, -7, 2, -3, -9, -6, -1, 0, -4, -1));

	Matrix43f m4 = m1;
	m4 += m2;
	EXPECT_EQ(m3, m4);

	EXPECT_EQ(m2 - m1, Matrix43f(-9,11,-2,7,2,-9,-5,2,9,-2,0,-5));

	Matrix43f m5 = m4 * 1.5f;
	EXPECT_EQ(m5, Matrix43f(-10.5f,1.5f,6.0f,10.5f,-3.0f,4.5f,13.5f,9.0f,1.5f,0,6.0f,1.5f));

	Matrix43f m6 = m4;
	m6 *= 1.5f;
	EXPECT_EQ(m6, m5);

	Matrix43f m7 = m2 / 2.0f;
	EXPECT_EQ(m7, Matrix43f(-4.0f,3.0f,0.5f,3.5f,0,-1.5f,1.0f,2.0f,2.5f,-0.5f,1.0f,-1.0f));

	Matrix43f m8 = m2;
	m8 /= 2.0f;
	EXPECT_EQ(m7, m8);

	Matrix43f m9 = m1;
	m9.Identity();
	EXPECT_EQ(m9, Matrix43f(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0));

	Matrix43f m10 = m2;
	m10.ZeroTranslation();
	EXPECT_EQ(m10, Matrix43f(-8, 6, 1, 7, 0, -3, 2, 4, 5, 0,0,0));

	Matrix43f m11 = m1;
	m11.SetTranslation(Vector3f(1,-0.375f,8.09f));
	EXPECT_EQ(m11, Matrix43f(1, -5, 3, 0, -2, 6, 7, 2, -4, 1,-0.375f,8.09f));

	Vector3f v1 = m11.GetTranslation();
	EXPECT_EQ(v1, Vector3f(1,-0.375f,8.09f));

	Matrix43f m12(3, -2, 0, 1, 4, -3, -1, 0, 2, 1, 2, 4);
	float det = MatrixDeterminant(m12);
	EXPECT_TRUE(abs(det - 22) < eps);

	Matrix43f m13;
	MaxtrixTranslation(m13, v1);
	EXPECT_EQ(m13,  Matrix43f(1, 0, 0, 0, 1, 0, 0, 0, 1, 1,-0.375f,8.09f));

	Matrix43f m14;
	MatrixScaling(m14, Vector3f(0.5f, 0.5f, 0.5f));
	EXPECT_EQ(m14,  Matrix43f(0.5f, 0, 0, 0, 0.5f, 0, 0, 0, 0.5f, 0, 0, 0));


	// Rotation

	Matrix43f m15;
	MatrixRotationY(m15, 90 * M_PI / 180);
	Vector3f v2(1, 1, 0);
	Vector3f v3 = v2 * m15;
	EXPECT_EQ(v3, Vector3f(0, 1, -1));

	float f = sqrt(2.0f) / 2.0f;
	Quaternion q(0.0f, f, 0.0f, f);
	Matrix43f m16;
	MatrixRotationQuaternion(m16, q);

	EXPECT_EQ(m15, m16);
}
