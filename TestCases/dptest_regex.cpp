/********************************************************************
	created:	2014/11/23
	created:	23:11:2014   12:13
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_regex.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_regex
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test C++11 regex
*********************************************************************/
#include "DpUnitTest.h"
#include <regex>
#include <string>

using namespace dopixel;
using namespace std;

void test_special_char()
{
	// . : any char
	{
		string pattern = ".";
		string test_str = "hello world";
		
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "h");
	}
	
	// [] : char set
	{
		string pattern = "[A-Za-z]+";
		string test_str = "hello world";

		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "hello");
	}

	// {n} : counting
	{
		string pattern = "ab{2}";
		string test_str = "abab abbcd cdab";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "abb");
	}

	// () : sub pattern
	{
		string pattern = "(ab){2}";
		string test_str = "abab abbcd cdab";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "abab");
	}

	// {n,m} : counting, at least n times, at most m times
	{
		string pattern = "(ab){2,3}";
		string test_str = "ab abbcd cdab ababababa";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "ababab");
	}

	// + : one or more
	{
		string pattern = "((ab){2})+";
		string test_str = "ab abbcd cdab ababababa abababab";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "abababab");

		// "(ab){2}+"	: error
		// "[(ab){2}]+" : ab
	}

	// ^ : not , in range
	{
		string pattern = "[ ]a[^ ]+";
		string test_str = "cba def a hhjk axy";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str().substr(1) == "axy");
		
		// results.str() : " axy"
	}

	// ^ : it means at the begin of ...,  in not range
	{
		string pattern = "^abc[0-9]+";
		string test_str = "abc12 abc ab acc abc123dd abc22";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			os_cout << results.str();
	}

	// | : or
	{
		string pattern = "(a|c){2}b[0-9]+";
		string test_str = "012 xyz acc123 acba accb0 ccb456";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "ccb0");
	}
	
	// ? : 0 or 1
	{
		string pattern = "ac?[0-9]*";
		string test_str = "aa abc a ac123";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "a");
	}

	// \d : decimal number
	{
		string pattern = "\\d{4}";
		string test_str = "012 abc21 45 1122 0ab";
		regex r(pattern);
		smatch results;
		if (regex_search(test_str, results, r))
			EXPECT_TRUE(results.str() == "1122");
	}
}

void test_regex_func()
{
	// match list
	{
		string pattern = "\\d+";
		string test_str = "abcd12 dd453 dde_a124 sj678";
		regex r(pattern);
		vector<string> expect{ "12", "453", "124", "678" };
		sregex_iterator it(test_str.begin(), test_str.end(), r);
		auto it2 = expect.begin();
		while (it != sregex_iterator())
		{
			EXPECT_TRUE(it->str() == *it2);
			++it;
			++it2;
		}
		EXPECT_TRUE(it2 == expect.end());
	}

	// regex replace
	{
		string pattern = "[0-9]+";
		string test_str = "abc 234 ee12 acd2kl0";
		regex r(pattern);

		os_cout << regex_replace(test_str, r, "_");
	}
}

void test_common_func()
{
	// float expression
	{
		//string pattern = "([+-]?[0-9]+((\\.?[0-9]*e[+-][0-9]+)|(\\.?[0-9]*)))";
		string pattern = "([-+]?[0-9]+[.]?[0-9]*([eE][-+]?[0-9]+)?)";
		regex r(pattern);
		smatch results;

		string test_float1 = "-28.12";
		if (regex_search(test_float1, results, r))
			EXPECT_TRUE(results.str() == "-28.12");

		string test_float2 = "8.34465e-007";
		if (regex_search(test_float2, results, r))
			EXPECT_TRUE(results.str() == "8.34465e-007")

			string test_float3 = "376521";
		if (regex_search(test_float3, results, r))
			EXPECT_TRUE(results.str() == "376521")
	}

	// string expression
	{
		string pattern = "[0-9a-zA-Z \\\\:._-]+";
		regex r(pattern);
		smatch results;

		string test_string1 = "\"D:\\WINXPP Program Files\\ts5\\Textures\\metal04.bmp\"";
		if (regex_search(test_string1, results, r))
		{
			EXPECT_TRUE(results.str() == "D:\\WINXPP Program Files\\ts5\\Textures\\metal04.bmp");
		}
	}

}

DPTEST(Regex)
{
	try
	{
		test_special_char();

		test_regex_func();

		test_common_func();
	}
	catch (regex_error e)
	{
		os_cout << e.what() << "\n";
	}
}