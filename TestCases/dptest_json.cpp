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

using namespace dopixel;

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

		unsigned int a = 0;
		js >> JVAR_OUT_MANUAL("score", a);
		EXPECT_TRUE(a == score);
	}

	// long long
	{
		long long maxNum = -50123456789;
		JsonDoc js;
		js << JVAR_IN(maxNum);
		EXPECT_TRUE(js.GetJsonString() == "{\"maxNum\":-50123456789}");

		long long a = 0;
		js >> JVAR_OUT_MANUAL("maxNum", a);
		EXPECT_TRUE(a == maxNum);
	}

	// unsigned long long
	{
		unsigned long long maxNum = 50123456789;
		JsonDoc js;
		js << JVAR_IN(maxNum);
		EXPECT_TRUE(js.GetJsonString() == "{\"maxNum\":50123456789}");

		unsigned long long a = 0;
		js >> JVAR_OUT_MANUAL("maxNum", a);
		EXPECT_TRUE(a == maxNum);
	}

	// float
	{

	}

	// array >> bool >> int >> unsigned int >> long long >> unsigned long long >> std::string >> float >> double
	{
		JsonDoc js;
		js << JVAR_IN_MANUAL("he", 15);
		JsonDoc js2;
		
		js2 << true;
		js2 << -1;
		js2 << 1;
		js2 << -50123456789;
		js2 << 50123456789;
		js2 << "hi";
		js2 << 3.12f;
		js2 << 3.14159265853;
		js2 << js;

		bool b = false;
		int i = -1;
		unsigned int u = 1;
		long long l = 0;
		unsigned long long ul = 0;
		std::string s;
		float f;
		double d;

		js2[0] >> b;
		js2[1] >> i;
		js2[2] >> u;
		js2[3] >> l;
		js2[4] >> ul;
		js2[5] >> s;
		js2[6] >> f;
		js2[7] >> d;

		EXPECT_TRUE(b == true && i == -1 && u == 1 && l == -50123456789 && ul == 50123456789 && s == "hi");
		EXPECT_TRUE(fabs(f - 3.12f) < 0.00001);
		EXPECT_TRUE(fabs(d - 3.14159265853) < 0.000000001);
	}


	// nested
	{
		JsonDoc js;
		js << JVAR_IN_MANUAL("hello", "world");

		JsonDoc jsArray;
		{
			int count = 5;
			for (int i = 0; i < count; ++i)
			{
				JsonDoc js2;
				js2 << JVAR_IN_MANUAL("a", i);

				jsArray << js2;
			}
		}
		js << JVAR_IN(jsArray);

		EXPECT_TRUE(js.GetJsonString() == "{\"hello\":\"world\",\"jsArray\":[{\"a\":0},{\"a\":1},{\"a\":2},{\"a\":3},{\"a\":4}]}");
	}

	// array
	{
		const char* jsonString = "{\"hello\":\"world\", \"num\":10, \"arrayNum\":[1, 2, 3] }";
		JsonDoc jsonDoc;
		if (jsonDoc.ParseJsonString(jsonString))
		{
			if (jsonDoc.HasMember("arrayNum"))
			{
				JsonValue arrayNum = jsonDoc.GetMember("arrayNum");
				int a[3] = { 0 };
				for (size_t i = 0; i < arrayNum.Size(); ++i)
				{
					arrayNum[i] >> a[i];
				}

				EXPECT_TRUE(a[0] == 1 && a[1] == 2 && a[2] == 3);
			}
		}
	}
}


DPTEST(json)
{
	simple_dpjson_serialize();
}