/********************************************************************
	created:	2014/06/29
	created:	29:6:2014   7:55
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_loadplg.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_loadplg
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Load PLG
*********************************************************************/

#include "DpUnitTest.h"
#include "DpLoadPLG.h"
#include "DpFileStream.h"
#include "DpObject.h"
using namespace DoPixel::DpTest;
using namespace DoPixel::Core;

DPTEST(LoadPLG)
{
	char* sz = "slider 21 19";
	char ch[64] = { 0 };
	int x, y;

	sscanf_s(sz, "%s %d %d", ch, sizeof(ch), &x, &y);

	Object obj;
	int l = LoadObjectFromPLG(obj, "tank1.plg", Vector4f(1,1,1,1), Vector4f(0,0,0,1));
	EXPECT_EQ(l, 0);

	EXPECT_EQ(obj.numVertices, 21);
	EXPECT_EQ(obj.numPolys, 19);

	EXPECT_EQ(obj.vListLocal[17], Vector4f(30, 10, 100, 1));
}