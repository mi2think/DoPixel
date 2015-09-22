/********************************************************************
	created:	2015/09/21
	created:	21:9:2015   8:24
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_event.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_event
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Test Event
*********************************************************************/
#include "DpUnitTest.h"
#include "DpEvent.h"

using namespace dopixel;
using namespace dopixel::dptest;
using namespace dopixel::core;

enum
{
	USER_EVENT1 = Event::USER + 1,
	USER_EVENT2 = Event::USER + 2
};

extern const char s1[] = "UserEvent";

class MyEvent1 : public BaseEvent<USER_EVENT1, s1>
{
public:
	MyEvent1(int x, int y)
		: x_(x)
		, y_(y)
	{}

	int GetX() const { return x_; }
	int GetY() const { return y_; }
private:
	int x_;
	int y_;
};

void OnMyEvent1(const Event& event)
{
	const MyEvent1* myEvent = event_cast<MyEvent1>(event);
	EXPECT_TRUE(myEvent != nullptr);
	EXPECT_EQ(myEvent->GetX(), 1);
	EXPECT_EQ(myEvent->GetY(), 2);
	EXPECT_EQ(myEvent->ToString(), s1);
}

extern const char s2[] = "MyEvent2";

class MyEvent2 : public BaseEvent<USER_EVENT2, s2>
{
public:
	MyEvent2(const char* id) : id_(id) {}

	const char* GetID() const { return id_; }
private:
	const char* id_;
};

typedef Ref<MyEvent2> MyEvent2Ref;

void OnMyEvent2(const EventRef& event)
{
	const MyEvent2Ref myEvent2 = event.As<MyEvent2>();
	EXPECT_TRUE(strcmp("Hello world", myEvent2->GetID()) == 0);
}

DPTEST(Event)
{
	MyEvent1 myEvent1(1, 2);
	OnMyEvent1(myEvent1);

	EventRef myEvent2(new MyEvent2("Hello world"));
	OnMyEvent2(myEvent2);
}
