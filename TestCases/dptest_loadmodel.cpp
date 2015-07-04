/********************************************************************
	created:	2014/06/29
	created:	29:6:2014   7:55
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_loadmodel.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_loadmodel
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Load Model
*********************************************************************/

#include "DpUnitTest.h"
#include "DpLoadModel.h"
#include "DpFileStream.h"
#include "DpObject.h"
using namespace DoPixel::DpTest;
using namespace DoPixel::Core;


void LoadPLG()
{
	char* sz = "slider 21 19";
	char ch[64] = { 0 };
	int x, y;

	sscanf_s(sz, "%s %d %d", ch, sizeof(ch), &x, &y);

	Object obj;
	bool b = LoadObjectFromPLG(obj, "tank1.plg", Vector4f(1, 1, 1, 1), Vector4f(0, 0, 0, 1), Vector4f(1, 1, 1, 1), 0);
	EXPECT_TRUE(b);

	EXPECT_EQ(obj.numVertices, 21);
	EXPECT_EQ(obj.numPolys, 19);

	EXPECT_EQ(obj.vListLocal[17].v, Vector4f(30, 10, 100, 1));
}

void LoadASC()
{
	Object obj;
	LoadObjectFrom3DSASC(obj, "cube01.asc", Vector4f(1, 1, 1, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 0);
}

void LoadCOB()
{
	Object obj;
	LoadObjectFromCOB(obj, "caligari_cob_format_example01.cob", Vector4f(1, 1, 1, 1), Vector4f(0, 0, 0, 1), Vector4f(0, 0, 0, 1), 0);
}

DPTEST(LoadModel)
{
	LoadPLG();

	LoadASC();

// 	LoadCOB();
}