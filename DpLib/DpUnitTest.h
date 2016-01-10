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

#include "DoPixel.h"

namespace dopixel
{
	class TestCase
	{
	public:
		TestCase(const char* testCaseName);
		virtual ~TestCase() = 0;

		virtual void Run() = 0;

		int& GetPassed() { return nPassed_; }
		int& GetFailed() { return nFailed_; }
		const char* GetTestCaseName() const { return testCaseName_; }
	protected:
		int nPassed_;
		int nFailed_;
		const char* testCaseName_;
	};

	class UnitTest
	{
	public:
		static UnitTest& GetInstance() { static UnitTest unitTest; return unitTest; }

		UnitTest();
		~UnitTest();

		TestCase* RegisterTestCase(TestCase* testCase);
		TestCase* GetCurrentTestCase() const { return currTestCase_; }

		void Run();

		static void MarkCurrentTestCase(bool passed);
	protected:
		int nPassed_;
		int nFailed_;
		std::vector<TestCase*> testCases_;
		TestCase* currTestCase_;
	};

	#define TESTCASE_NAME(testCaseName) testCaseName##TEST

	#define DPTEST(testCaseName) \
			class TESTCASE_NAME(testCaseName) : public TestCase \
			{ \
			public: \
				TESTCASE_NAME(testCaseName) (const char* testCaseName) : TestCase(testCaseName) {} \
				virtual void Run(); \
			private: \
				static const TestCase* testCase; \
			}; \
			const TestCase* TESTCASE_NAME(testCaseName) \
				::testCase = UnitTest::GetInstance().RegisterTestCase(new TESTCASE_NAME(testCaseName)(#testCaseName));\
			void TESTCASE_NAME(testCaseName)::Run()

	// compare helper

	#define DPTEST_CMP_FUNC(op_name, op) CmpHelper##op_name

	#define DPTEST_CMP_HELPER(op_name, op) \
		template <typename T, typename U> \
		void DPTEST_CMP_FUNC(op_name, op)(const T& a, const U& b) \
			{ \
				TestCase* currTestCase = UnitTest::GetInstance().GetCurrentTestCase(); \
				const char* testCastName = currTestCase->GetTestCaseName(); \
				if (a op b) \
				{ \
					UnitTest::MarkCurrentTestCase(true); \
					GLOGS.SetLevel(Log::Info); \
					GLOGS << "!>" << testCastName << " SUCCESS:\t" << a << " " #op " " << b << "\n"; \
				} \
				else \
				{ \
					UnitTest::MarkCurrentTestCase(false); \
					GLOGS.SetLevel(Log::Error); \
					GLOGS << "!>" << testCastName << " FAILED:\t" << a << " " #op " " << b << "\n"; \
				} \
			}

	#define DPTEST_TEST_BOOL(cond, context, b) \
			{ \
				TestCase* currTestCase = UnitTest::GetInstance().GetCurrentTestCase(); \
				const char* testCastName = currTestCase->GetTestCaseName(); \
				if (cond == b) \
				{ \
					UnitTest::MarkCurrentTestCase(true); \
					GLOGS.SetLevel(Log::Info); \
					GLOGS << "!>" << testCastName << " SUCCESS:\t" #context " " << "\n"; \
				} \
				else \
				{ \
					UnitTest::MarkCurrentTestCase(false); \
					GLOGS.SetLevel(Log::Error); \
					GLOGS << "!>" << testCastName << " FAILED:\t" #context " " << "\n"; \
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
