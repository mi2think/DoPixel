/********************************************************************
	created:	2015/11/22
	created:	22:11:2015   11:54
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_vertexarray.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_vertexarray
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Vertex Array
*********************************************************************/
#include "DpVertexArray.h"
#include "DpUnitTest.h"
#include "DpVector3.h"

using namespace dopixel;
using namespace dopixel::math;

DPTEST(VertexArray)
{
	{
		VertexArray<float, 3> array1(5);

		EXPECT_EQ(array1.GetComponentCount(), 3);
		EXPECT_EQ(array1.GetVertexCount(), 5);
		EXPECT_EQ(array1.GetComponentStride(), 4);
		EXPECT_EQ(array1.GetDataStride(), 12);


		Vector3f* v = array1.DataAs<Vector3f>();
		for (int i = 0; i < array1.GetVertexCount(); ++i)
		{
			*(v + i) = Vector3f(i * 1.0f, i * 1.0f, i * 1.0f);
		}

		float* data = array1.GetData();
		for (int i = 0; i < array1.GetVertexCount(); ++i)
		{
			for (int j = 0; j < array1.GetComponentCount(); ++j)
			{
				float val = *(data + i * array1.GetComponentCount() + j);
				EXPECT_TRUE(math::Equal(val, i * 1.0f));
			}
		}

		VertexArray<float, 3>* clone = array1.Clone();
		EXPECT_EQ(array1.GetComponentCount(), clone->GetComponentCount());
		EXPECT_EQ(array1.GetVertexCount(), clone->GetVertexCount());
		EXPECT_EQ(array1.GetComponentStride(), clone->GetComponentStride());
		EXPECT_EQ(array1.GetDataStride(), clone->GetDataStride());

		int cmp = memcmp(clone->GetData(), data, array1.GetDataStride() * array1.GetVertexCount());
		EXPECT_TRUE(cmp == 0);

		delete clone;
	}


}
