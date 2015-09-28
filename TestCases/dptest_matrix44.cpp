/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:54
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_matrix44.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_matrix44
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	
*********************************************************************/
#include "DpMatrix44.h"
#include "DpUnitTest.h"
using namespace dopixel::math;
using namespace dopixel::dptest;

DPTEST(Matrix44)
{
	Matrix44f m1(1.21f,3.58f,6.3f,9.32f,
				 4.2f,1.7f,2.1f,4,
				 2.1f,3.9f,4,5,
				 2.3f,8,8,6);

	Matrix44f m2(5.2f,1.2f,3.9f,14,
				 1,5,3,1,
				 3,2.4f,6,5,
				 9,8,9,7);

	Matrix44f m3 = m1 + m2;
	Matrix44f m4(6.41f,4.78f,10.2f,23.32f,5.2f,6.7f,5.1f,5,5.1f,6.3f,10,10,11.3f,16,17,13);
	EXPECT_EQ(m3, m4);

	Matrix44f m5 = m1;
	m5 += m2;
	EXPECT_EQ(m5, m3);

	Matrix44f m6(6.4f,4.78f,10.2f,23.32f,5.2f,6.7f,5.1f,5,5.1f,6.3f,10,10,11.3f,16,17,13);
	EXPECT_TRUE(m6 != m5);

	Matrix44f m7 = m1 - m2;
	Matrix44f m8(-3.99f,2.38f,2.4f,-4.68f,3.2f,-3.3f,-0.9f,3,-0.9f,1.5f,-2,0,-6.7f,0,-1,-1);
	EXPECT_EQ(m7, m8);

	Matrix44f m9 = m1;
	m9 -= m2;
	EXPECT_EQ(m7, m9);

	Matrix44f m10 = m9;
	m10 *= 0.24f;
	Matrix44f m11(-0.9576f,0.5712f,0.576f,-1.1232f,
				  0.768f,-0.792f,-0.216f,0.72f,
				  -0.216f,0.36f,-0.48f,0,
				  -1.608f,0,-0.24f,-0.24f);
	EXPECT_EQ(m10, m11);

	Matrix44f m12 = m9 * 0.24f;
	EXPECT_EQ(m10, m12);

	Matrix44f m13 = m4;
	m13 /= 0.25f;

	Matrix44f m14(25.64f,19.12f,40.8f,93.28f,
		          20.8f, 26.8f, 20.4f, 20,
				  20.4f, 25.2f, 40, 40,
				  45.2f, 64, 68, 52);
	EXPECT_EQ(m13, m14);

	Matrix44f m15 = m4 / 0.25f;
	EXPECT_EQ(m13, m15);

	Matrix44f m16 = -m14;
	EXPECT_EQ(m16, Matrix44f(-25.64f,-19.12f,-40.8f,-93.28f,-20.8f,-26.8f,-20.4f,-20,-20.4f,-25.2f,-40,-40,-45.2f,-64,-68,-52));

	Matrix44f m17 = m1 * m2;
	Matrix44f m18(112.652f,109.032f,137.139f,117.26f,
				  65.84f,50.58f,70.08f,99,
				  71.82f,71.62f,88.89f,88.3f,
				  97.96f,109.96f,134.97f,122.2f);
	EXPECT_EQ(m17, m18);

	Matrix44f m19;
	MatrixMultiply(m19, m1, m2);
	EXPECT_EQ(m19, m18);

	Matrix44f m20 = m19;
	m20.Identity();
	EXPECT_EQ(m20, Matrix44f(1,0,0,0,
							 0,1,0,0,
							 0,0,1,0,
							 0,0,0,1));

	Matrix44f m21 = 0.24f * m9;
	EXPECT_EQ(m21, m12);

	float det = MatrixDeterminant(m18);
	EXPECT_EQ(det, 124972.813f);

	// Transpose
	Matrix44f m22(1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10, 11, 12,
		13, 14, 15, 16);
	Matrix44f m23;
	MatrixTranspose(m23, m22);
	EXPECT_EQ(m23, Matrix44f(1, 5, 9, 13,
		2, 6, 10, 14,
		3, 7, 11, 15,
		4, 8, 12, 16));

	Matrix44f m24 = m22;
	m24.Transpose();
	EXPECT_EQ(m24, m23);

	{
		Matrix44f m25;
		MatrixRotationZYX(m25, angle2radian(30), angle2radian(45), angle2radian(60));
		m25.Transpose();

		// OpenGL way
		Matrix44f x, y, z;
		MatrixRotationX(x, angle2radian(30));
		x.Transpose();
		MatrixRotationY(y, angle2radian(45));
		y.Transpose();
		MatrixRotationZ(z, angle2radian(60));
		z.Transpose();

		Matrix44f xy;
		MatrixMultiply(xy, x, y);

		Matrix44f xyz;
		MatrixMultiply(xyz, xy, z);

		EXPECT_EQ(xyz, m25);
	}

}

