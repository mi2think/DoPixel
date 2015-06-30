/********************************************************************
	created:	2014/11/13
	created:	13:11:2014   23:35
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_strutility.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_strutility
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test StrUtility
*********************************************************************/
#include "DpStrUtility.h"
#include "DpUnitTest.h"
using namespace DoPixel::DpTest;
using namespace DoPixel::Core;

DPTEST(StrUtility)
{
	const char* str = "afefrgrefdsgh123adghDRHSA33333....";
	char sz[256];
	
	// StrUtility::StrStrip
	StrUtility::StrStrip(sz, str, "ef");
	EXPECT_TRUE(strcmp(sz, "afrgrdsgh123adghDRHSA33333....") == 0);

	StrUtility::StrStrip(sz, str, "AB");
	EXPECT_TRUE(strcmp(sz, str) == 0);

	StrUtility::StrStrip(sz, str, "a");
	EXPECT_TRUE(strcmp(sz, "fefrgrefdsgh123dghDRHSA33333....") == 0);

	StrUtility::StrStrip(sz, str, ".");
	EXPECT_TRUE(strcmp(sz, "afefrgrefdsgh123adghDRHSA33333") == 0);

	StrUtility::StrStrip(sz, str, "....");
	EXPECT_TRUE(strcmp(sz, "afefrgrefdsgh123adghDRHSA33333") == 0);

	StrUtility::StrStrip(sz, str, "afefrgrefdsgh123adghDRHSA33333....");
	EXPECT_TRUE(strcmp(sz, "") == 0);

	std::string str2 = "adfsaklfjegkjsdlfla-33r5w";
	std::string out;
	
	StrUtility::StrStrip(out, str2, "lf");
	EXPECT_TRUE(out == std::string("adfsakjegkjsdla-33r5w"));

	StrUtility::StrStrip(out, str2, "akg");
	EXPECT_TRUE(out == std::string("adfsaklfjegkjsdlfla-33r5w"));

	StrUtility::StrStrip(out, str2, "adfs");
	EXPECT_TRUE(out == std::string("aklfjegkjsdlfla-33r5w"));

	StrUtility::StrStrip(out, str2, "w");
	EXPECT_TRUE(out == std::string("adfsaklfjegkjsdlfla-33r5"));

	// StrUtility::StrReplace
	std::string str3 = "sakg;la'gkjdajfdalagkjadkla;qpdjg";
	StrUtility::StrReplace(out, str3, "la", "XY");
	EXPECT_TRUE(out == "sakg;XY'gkjdajfdaXYgkjadkXY;qpdjg");

	StrUtility::StrReplace(out, str3, "s", "SSS");
	EXPECT_TRUE(out == "SSSakg;la'gkjdajfdalagkjadkla;qpdjg");

	StrUtility::StrReplace(out, str3, "g", "GGG");
	EXPECT_TRUE(out == "sakGGG;la'GGGkjdajfdalaGGGkjadkla;qpdjGGG");

	StrUtility::StrReplace(out, str3, "gold", "Money");
	EXPECT_TRUE(out == "sakg;la'gkjdajfdalagkjadkla;qpdjg");

	StrUtility::StrReplace(out, str3, "sakg;la'gkjdajfdalagkjadkla;qpdjg", "Hello World!");
	EXPECT_TRUE(out == "Hello World!");

	StrUtility::StrReplace(out, str3, "sakg;la'gkjdajfdalagkjadkla;qpdjg", "");
	EXPECT_TRUE(out == "");

	// StrUtility::StrTrimLeft
	std::string str4 = "\t   akg music & k3003";
	StrUtility::StrTrimLeft(str4);
	EXPECT_TRUE(str4 == "akg music & k3003");

	// StrUtility::StrTrimRight
	std::string str5 = "akg music & k3003\t\t\t  \t";
	StrUtility::StrTrimRight(str5);
	EXPECT_TRUE(str5 == "akg music & k3003");

	float f;
	bool b = StrUtility::ToFloat(f, "0.2345f");
	EXPECT_TRUE(b);

	// StrUtility::StrSplit
	std::string str6 = "LookAtOffset:(0,0,0);LookFromOffset:(0,0,0);CameraType:1";
	std::vector<std::string> vec;
	StrUtility::StrSplit(vec, str6, ';');
	EXPECT_TRUE(vec.size() == 3);
	EXPECT_TRUE(vec[1] == "LookFromOffset:(0,0,0)");
}
