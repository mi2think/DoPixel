/********************************************************************
	created:	2014/11/22
	created:	22:11:2014   16:14
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_fileparser.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_fileparser
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test file parser
*********************************************************************/
#include "DpUnitTest.h"
#include "DpFileParser.h"
using namespace dopixel;

#pragma warning(push,3)
#pragma warning(disable:4706)

DPTEST(FileParser)
{
	FileParser parser;

	parser.Open("test1.txt");
	std::string str;
	bool b = false;

	EXPECT_TRUE((b = parser.GetLine(str)) && str == "object name: tank");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "num vertices: 4");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "vertex list:");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "0 10 20");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "5 90 10");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "3 4 3");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "1 2 3");
	EXPECT_TRUE((b = parser.GetLine(str)) && str == "end object");
	// The Last line, read ""
	EXPECT_FALSE((b = parser.GetLine(str)));

	// Pattern match
	bool match1 = parser.PatternMatch("vertex  34.234 56.34 12.4", "['vertex'] [f] [f] [f]");
	EXPECT_TRUE(match1);

	bool match2 = parser.PatternMatch("vertex  aaaa 56.34 12.4", "['vertex'] [s<5] [f] [f]");
	EXPECT_TRUE(match2);
}
#pragma warning(pop)