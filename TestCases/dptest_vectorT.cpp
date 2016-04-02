/********************************************************************
	created:	2016/04/02
	created:	2:4:2016   9:33
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_vectorT.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_vectorT
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Test VectorT
*********************************************************************/
#include "DpVector2.h"
#include "DpVector3.h"
#include "DpVector4.h"
#include "DpVectorT.h"
#include "DpUnitTest.h"
using namespace dopixel;
using namespace dopixel::math;

void testVector2T()
{
	Vector2f v1(2.5f, 3.0f);
	Vector2f v2(5.4f, 6.7f);

	Vector3f v3(1.0f, 2.0f, 3.0f);
	Vector3f v4(1.0f, 2.0f, 3.0f);

	{
		Vector2T<float, Vector3f> v2t1(5.0f, v3);
		v2t1 *= 5.0f;
		EXPECT_TRUE(math::Equal(v2t1.t0, 25.0f));
		EXPECT_TRUE(v2t1.t1 == Vector3f(5.0f, 10.0f, 15.0f));
	}

	{
		Vector2T<Vector2f, Vector3f> v2t1(v1, v3);
		Vector2T<Vector2f, Vector3f> v2t2(v2, v4);

		Vector2T<Vector2f, Vector3f> v2t3 = v2t1 + v2t2;
		EXPECT_TRUE(v2t3.t0 == Vector2f(7.9f, 9.7f));
		EXPECT_TRUE(v2t3.t1 == Vector3f(2.0f, 4.0f, 6.0f));
	}

}

void testVector3T()
{

}

void testVector4T()
{

}


DPTEST(VectorT)
{
	testVector2T();

	testVector3T();

	testVector4T();
}
