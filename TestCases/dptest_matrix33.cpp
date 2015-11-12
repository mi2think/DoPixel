/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   18:39
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_matrix33.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_matrix33
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Matrix33
*********************************************************************/

#include "DpMatrix33.h"
#include "DpUnitTest.h"
using namespace dopixel;
using namespace dopixel::math;

DPTEST(Matrix33)
{
	Matrix33f m1(1, -5, 3, 0, -2, 6, 7, 2, -4);
	Matrix33f m2(-8, 6, 1, 7, 0, -3, 2, 4, 5);

	EXPECT_NE(m1, m2);

	EXPECT_EQ(-m1, Matrix33f(-1,5,-3,0,2,-6,-7,-2,4));

	EXPECT_EQ(m1 + m2, Matrix33f(-7,1,4,7,-2,3,9,6,1));

	Matrix33f m9 = m1;
	m9 += m2;
	EXPECT_EQ(m9,m1 + m2);

	EXPECT_EQ(m2 - m1, Matrix33f(-9,11,-2,7,2,-9,-5,2,9));

	Matrix33f m10 = m2  * 1.5f;
	EXPECT_EQ(m10, Matrix33f(-12,9,1.5f,10.5f,0,-4.5f,3,6.0f,7.5f));

	Matrix33f m11 = m1 / 2;
	EXPECT_EQ(m11, Matrix33f(0.5f, -2.5f, 1.5f, 0, -1, 3, 3.5f, 1, -2));

	Matrix33f m3 = m1 * m2;

	EXPECT_EQ(m3, Matrix33f(-37, 18, 31, -2, 24, 36, -50, 26, -19));

	Matrix33f m4(3, -2, 0, 1, 4, -3, -1, 0, 2);
	float det = MatrixDeterminant(m4);
	EXPECT_TRUE(abs(det - 22) < EPSILON_E5);

	Matrix33f m5;
	MatrixTranspose(m5, m4);
	EXPECT_EQ(m5, Matrix33f(3, 1, -1, -2, 4, 0, 0, -3, 2));

	Matrix33f m6(-4, -3, 3, 0, 2, -2, 1, 4, -1);
	Matrix33f m7;
	MatrixInverse(m7, m6);

	os_cout << m7;

	Matrix33f m8 = m7 * m6;
	EXPECT_EQ(m8, Matrix33f(1, 0, 0, 0, 1, 0, 0, 0, 1));

	Matrix33f m12 = m1;
	m12 *= m2;
	EXPECT_EQ(m12, m3);

	Matrix33f m13;
	m13.Identity();
	EXPECT_EQ(m13, Matrix33f(1, 0, 0, 0, 1, 0, 0, 0, 1));
}
