/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   9:53
	filename: 	F:\SkyDrive\3D\DpLib\TestCases\dptest_main.cpp
	file path:	F:\SkyDrive\3D\DpLib\TestCases
	file base:	dptest_main
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test main
*********************************************************************/
#include "DpUnitTest.h"
#include <cstdlib>

using namespace dopixel;


int main()
{
	UnitTest::GetInstance().Run();
	system("pause");

	return 0;
}
