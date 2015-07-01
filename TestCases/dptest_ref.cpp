/********************************************************************
	created:	2015/05/23
	created:	23:5:2015   21:52
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_ref.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_ref
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Ref
*********************************************************************/
#include "DpUnitTest.h"
#include "DpRef.h"

using namespace DoPixel::DpTest;
using namespace DoPixel::Core;

class Base
{
public:
	virtual ~Base();

	virtual int GetId() const { return 1; }
};

class Derived : public Base
{
public:
	virtual int GetId() const override { return 2; }
};


DPTEST(Ref)
{
	Ref<int> r1(new int(1));
	EXPECT_TRUE(r1.Count() == 1);
}