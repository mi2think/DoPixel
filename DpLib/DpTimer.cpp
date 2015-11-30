/********************************************************************
	created:	2015/11/30
	created:	30:11:2015   18:26
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTimer.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTimer
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Timer
*********************************************************************/
#include "DpTimer.h"

#include <windows.h>

namespace dopixel
{
	Timer::Timer()
		: cpuFreq_(0)
		, cpuTicksPerSec_(0)
		, pfunc_(nullptr)
		, currTime_(0)
		, lastTime_(0)
		, timestep_(0)
	{
		Init();
	}

	void Timer::Init()
	{
		LARGE_INTEGER freq;
		if (QueryPerformanceFrequency(&freq))
		{
			LARGE_INTEGER t;
			QueryPerformanceCounter(&t);
			cpuTicksPerSec_ = freq.QuadPart;
			pfunc_ = &Timer::GetQueryPerformanceTime;
		}
		else
		{
			pfunc_ = &Timer::GetMMTime;
			cpuTicksPerSec_ = 1000;
		}
	}

	__int64 Timer::GetTime() const
	{
		return (this->*pfunc_)();
	}

	__int64 Timer::GetMMTime() const
	{
		__int64 time = timeGetTime();
		return time;
	}

	__int64 Timer::GetQueryPerformanceTime() const
	{
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		__int64 time = t.QuadPart;
		return time;
	}

	float Timer::Tick()
	{
		currTime_ = GetTime();
		timestep_ = (float)(currTime_ - lastTime_) / cpuTicksPerSec_;
		return timestep_;
	}

	void Timer::Reset()
	{
		lastTime_ = currTime_;
	}

	float Timer::GetTimestep() const
	{
		return timestep_;
	}
}