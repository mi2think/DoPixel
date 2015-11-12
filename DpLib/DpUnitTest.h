/********************************************************************
	created:	2014/06/14
	created:	14:6:2014   1:58
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpUnitTest.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpUnitTest
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Unit Test
*********************************************************************/

#ifndef __DP_UNITTEST__
#define __DP_UNITTEST__

#include "DpConsole.h"
#include <vector>
using namespace dopixel;

namespace dopixel
{
	class TestCase
	{
	public:
		TestCase(const char* _testCaseName) : nPassed(0), nFailed(0), testCaseName(_testCaseName) {}

		virtual void Run() = 0;

		int nPassed;
		int nFailed;
		const char* testCaseName;
	};

	class UnitTest
	{
	public:
		static UnitTest* GetInstance() { static UnitTest unitTest; return &unitTest; }

		UnitTest() : nFailed(0), nPassed(0), currTestCase(NULL) {}

		~UnitTest()
		{
			for (auto e : testCaseVec)
			{
				delete e;
			}
		}

		TestCase* RegisterTestCase(TestCase* testCase)
		{
			testCaseVec.push_back(testCase);
			return testCase;
		}

		void Run()
		{
			for (auto e : testCaseVec)
			{
				currTestCase = e;
				e->Run();

				nPassed += e->nPassed;
				nFailed += e->nFailed;

				os_cout << White << ">>>>>>>> " << currTestCase->testCaseName << endl;
				os_cout << "*************************************" << endl;
			}

			os_cout << White << "Run Tests:" << nPassed + nFailed << endl;
			os_cout << Green << "Passed:" << nPassed << endl;
			if (nFailed > 0)
				os_cout << Red;
			os_cout << "Failed:" << nFailed << endl;
		}

		static void MarkCurrentTestCase(bool b)
		{
			TestCase* currTestCase = UnitTest::GetInstance()->currTestCase;
			b ? ++currTestCase->nPassed : ++currTestCase->nFailed;
		}
		TestCase* currTestCase;
	protected:
		int nPassed;
		int nFailed;
		std::vector<TestCase*> testCaseVec;
	};

#define TESTCASE_NAME(testCaseName) testCaseName##TEST

#define DPTEST(testCaseName) \
		class TESTCASE_NAME(testCaseName) : public TestCase \
		{ \
		public: \
			TESTCASE_NAME(testCaseName) (const char* _testCaseName) : TestCase(_testCaseName) {} \
			virtual void Run(); \
		private: \
			static const TestCase* testCase; \
		}; \
		const TestCase* TESTCASE_NAME(testCaseName) \
			::testCase = UnitTest::GetInstance()->RegisterTestCase(new TESTCASE_NAME(testCaseName)(#testCaseName));\
		void TESTCASE_NAME(testCaseName)::Run()

	// compare helper

#define DPTEST_CMP_FUNC(op_name, op) CmpHelper##op_name

#define DPTEST_CMP_HELPER(op_name, op) \
	template <typename T, typename U> \
	void DPTEST_CMP_FUNC(op_name, op)(const T& a, const U& b) \
		{ \
			TestCase* currTestCase = UnitTest::GetInstance()->currTestCase; \
			if (a op b) \
			{ \
				UnitTest::MarkCurrentTestCase(true); \
				os_cout << Green << "!>" << currTestCase->testCaseName << " SUCCESS:\t" << a << " " #op " " << b << endl; \
			} \
			else \
			{ \
				UnitTest::MarkCurrentTestCase(false); \
				os_cout << Red << "!>" << currTestCase->testCaseName << " FAILED:\t" << a << " " #op " " << b << endl; \
			} \
		}

#define DPTEST_TEST_BOOL(cond, context, b) \
		{ \
			TestCase* currTestCase = UnitTest::GetInstance()->currTestCase; \
			if (cond == b) \
			{ \
				UnitTest::MarkCurrentTestCase(true); \
				os_cout << Green << "!>" << currTestCase->testCaseName << " SUCCESS:\t" #context " " << endl; \
			} \
			else \
			{ \
				UnitTest::MarkCurrentTestCase(false); \
				os_cout << Red << "!>" << currTestCase->testCaseName << " FAILED:\t" #context " " << endl; \
			} \
		}

		// Compare functions
		DPTEST_CMP_HELPER(EQ, == )
		DPTEST_CMP_HELPER(NE, != )
		DPTEST_CMP_HELPER(LT, <)
		DPTEST_CMP_HELPER(LE, <= )
		DPTEST_CMP_HELPER(GT, >)
		DPTEST_CMP_HELPER(GE, >= )
		// ==
#define EXPECT_EQ(a, b) { DPTEST_CMP_FUNC(EQ, ==)(a, b); }
			// !=
#define EXPECT_NE(a, b) { DPTEST_CMP_FUNC(NE, !=)(a, b); }
			// <
#define EXPECT_LT(a, b) { DPTEST_CMP_FUNC(LT, <)(a, b); }
			// <=
#define EXPECT_LE(a, b) { DPTEST_CMP_FUNC(LE, <=)(a, b); }
			// >
#define EXPECT_GT(a, b) { DPTEST_CMP_FUNC(GT, >)(a, b); }
			// >=
#define EXPECT_GE(a, b) { DPTEST_CMP_FUNC(GE, >=)(a, b); }

#define EXPECT_TRUE(cond)  DPTEST_TEST_BOOL(cond, #cond, true)

#define EXPECT_FALSE(cond) DPTEST_TEST_BOOL(!(cond), #cond, true)
}

#endif
