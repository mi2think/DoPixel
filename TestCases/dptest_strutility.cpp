/********************************************************************
	created:	2014/11/13
	created:	13:11:2014   23:35
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_strutility.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_strutility
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test 
*********************************************************************/
#include "DoPixel.h"
#include "DpUnitTest.h"
using namespace dopixel;

DPTEST(STRUTLIS)
{
	const char* str = "afefrgrefdsgh123adghDRHSA33333....";
	char sz[256];
	
	// str_strip
	str_strip(sz, str, "ef");
	EXPECT_TRUE(strcmp(sz, "afrgrdsgh123adghDRHSA33333....") == 0);

	str_strip(sz, str, "AB");
	EXPECT_TRUE(strcmp(sz, str) == 0);

	str_strip(sz, str, "a");
	EXPECT_TRUE(strcmp(sz, "fefrgrefdsgh123dghDRHSA33333....") == 0);

	str_strip(sz, str, ".");
	EXPECT_TRUE(strcmp(sz, "afefrgrefdsgh123adghDRHSA33333") == 0);

	str_strip(sz, str, "....");
	EXPECT_TRUE(strcmp(sz, "afefrgrefdsgh123adghDRHSA33333") == 0);

	str_strip(sz, str, "afefrgrefdsgh123adghDRHSA33333....");
	EXPECT_TRUE(strcmp(sz, "") == 0);

	std::string str2 = "adfsaklfjegkjsdlfla-33r5w";
	std::string out;
	
	str_strip(out, str2, "lf");
	EXPECT_TRUE(out == std::string("adfsakjegkjsdla-33r5w"));

	str_strip(out, str2, "akg");
	EXPECT_TRUE(out == std::string("adfsaklfjegkjsdlfla-33r5w"));

	str_strip(out, str2, "adfs");
	EXPECT_TRUE(out == std::string("aklfjegkjsdlfla-33r5w"));

	str_strip(out, str2, "w");
	EXPECT_TRUE(out == std::string("adfsaklfjegkjsdlfla-33r5"));

	// str_replace
	std::string str3 = "sakg;la'gkjdajfdalagkjadkla;qpdjg";
	str_replace(out, str3, "la", "XY");
	EXPECT_TRUE(out == "sakg;XY'gkjdajfdaXYgkjadkXY;qpdjg");

	str_replace(out, str3, "s", "SSS");
	EXPECT_TRUE(out == "SSSakg;la'gkjdajfdalagkjadkla;qpdjg");

	str_replace(out, str3, "g", "GGG");
	EXPECT_TRUE(out == "sakGGG;la'GGGkjdajfdalaGGGkjadkla;qpdjGGG");

	str_replace(out, str3, "gold", "Money");
	EXPECT_TRUE(out == "sakg;la'gkjdajfdalagkjadkla;qpdjg");

	str_replace(out, str3, "sakg;la'gkjdajfdalagkjadkla;qpdjg", "Hello World!");
	EXPECT_TRUE(out == "Hello World!");

	str_replace(out, str3, "sakg;la'gkjdajfdalagkjadkla;qpdjg", "");
	EXPECT_TRUE(out == "");

	// str_trim left
	std::string str4 = "\t   akg music & k3003";
	str_trim(str4, TRIM_LEFT);
	EXPECT_TRUE(str4 == "akg music & k3003");

	// str_trim right
	std::string str5 = "akg music & k3003\t\t\t  \t";
	str_trim(str5, TRIM_RIGHT);
	EXPECT_TRUE(str5 == "akg music & k3003");

	bool b = str_is_valid_float("0.2345f");
	EXPECT_TRUE(b);

	// str_split
	std::string str6 = "LookAtOffset:(0,0,0);LookFromOffset:(0,0,0);CameraType:1";
	vector<string> vec;
	str_split(vec, str6, ';');
	EXPECT_TRUE(vec.size() == 3);
	EXPECT_TRUE(vec[1] == "LookFromOffset:(0,0,0)");

	// str_strip_anychar
	const char* str7 = "add...a<1,2> <2,3> <3,4>";
	str_strip_anychar(sz, str7, "<>.");
	EXPECT_TRUE(strcmp(sz, "adda1,2 2,3 3,4") == 0);

	const char* str8 = "..<a>add...a<1,2> <2,3> <3,4>";
	str_strip_anychar(sz, str8, "<ad>.");
	EXPECT_TRUE(strcmp(sz, "1,2 2,3 3,4") == 0);

	std::string str9 = str8;
	str_strip_anychar(out, str9, "<ad>.");
	EXPECT_TRUE(out == "1,2 2,3 3,4");

	// str_replace_anychar
	string str10 = "<11,43> <0,9> <12,44>";
	str_replace_anychar(out, str10, "<,>", " ");
	EXPECT_TRUE(out == " 11 43   0 9   12 44 ");
}
