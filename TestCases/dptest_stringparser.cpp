/********************************************************************
	created:	2016/04/17
	created:	17:4:2016   11:57
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_stringparser.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_stringparser
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	string parser
*********************************************************************/
#include "DpUnitTest.h"
#include "DpStringParser.h"
#include "DpMath.h"
using namespace dopixel;
using namespace dopixel::math;

DPTEST(StringParser)
{
	StringParser parser;
	// Pattern match
	bool match1 = parser.PatternMatch("vertex  34.234 56.34 12.4", "['vertex'] [f] [f] [f]");
	EXPECT_TRUE(match1);
	EXPECT_TRUE(FCMP(parser.GetMatchedVal<float>(0), 34.234f));
	EXPECT_TRUE(FCMP(parser.GetMatchedVal<float>(1), 56.34f));
	EXPECT_TRUE(FCMP(parser.GetMatchedVal<float>(2), 12.4f));

	bool match2 = parser.PatternMatch("vertex  aaaa 56.34 12.4", "['vertex'] [s<5] [f] [f]");
	EXPECT_TRUE(match2);
	EXPECT_TRUE(FCMP(parser.GetMatchedVal<float>(0), 56.34f));
	EXPECT_TRUE(FCMP(parser.GetMatchedVal<float>(1), 12.4f));
	EXPECT_TRUE(parser.GetMatchedVal<std::string>(1) == "aaaa");
}