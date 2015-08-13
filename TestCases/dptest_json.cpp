/********************************************************************
	created:	2015/08/05
	created:	5:8:2015   23:50
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_json.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_json
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Json Test
*********************************************************************/
#include "DpUnitTest.h"
#include "DpJson.h"

using namespace DoPixel::DpTest;
using namespace DoPixel::Core;

void simple_dpjson_serialize()
{
	// char*
	{
		JsonDoc js;
		js << JVAR_IN_MANUAL("hello", "world");
		EXPECT_TRUE(js.GetJsonString() == "{\"hello\":\"world\"}");

		std::string s;
		js >> JVAR_OUT_MANUAL("hello", s);
		EXPECT_TRUE(s == "world");
	}

	// std::string
	{
		std::string my = "baby!";
		JsonDoc js;
		js << JVAR_IN(my);
		EXPECT_TRUE(js.GetJsonString() == "{\"my\":\"baby!\"}");

		std::string s;
		js >> JVAR_OUT_MANUAL("my", s);
		EXPECT_TRUE(s == my);
	}

	// bool
	{
		bool AmIBoy = true;
		JsonDoc js;
		js << JVAR_IN(AmIBoy);
		EXPECT_TRUE(js.GetJsonString() == "{\"AmIBoy\":true}");

		bool b;
		js >> JVAR_OUT_MANUAL("AmIBoy", b);
		EXPECT_TRUE(b == AmIBoy);
	}

	// int
	{
		int id = -1;
		JsonDoc js;
		js << JVAR_IN(id);
		EXPECT_TRUE(js.GetJsonString() == "{\"id\":-1}");

		int id_test = 0;
		js >> JVAR_OUT_MANUAL("id", id_test);
		EXPECT_TRUE(id == id_test);
	}

	// unsigned int
	{
		unsigned int score = 100;
		JsonDoc js;
		js << JVAR_IN(score);
		EXPECT_TRUE(js.GetJsonString() == "{\"score\":100}");
	}

	// long long
	{
		long long maxNum = -50123456789;
		JsonDoc js;
		js << JVAR_IN(maxNum);
		EXPECT_TRUE(js.GetJsonString() == "{\"maxNum\":-50123456789}");
	}

	// unsigned long long
	{
		unsigned long long maxNum = 50123456789;
		JsonDoc js;
		js << JVAR_IN(maxNum);
		EXPECT_TRUE(js.GetJsonString() == "{\"maxNum\":50123456789}");
	}
}


DPTEST(json)
{
	simple_dpjson_serialize();
}