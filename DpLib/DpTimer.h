/********************************************************************
	created:	2014/09/08
	created:	8:9:2014   17:51
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpTimer.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpTimer
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Timer
*********************************************************************/
#ifndef __DP_TIMER__
#define __DP_TIMER__

namespace dopixel
{
	class Timer
	{
	public:
		Timer();

		void Init();
		void Reset();

		float GetTimestep() const;
		float Tick();
	private:
		__int64 GetTime() const;
		__int64 GetMMTime() const;
		__int64 GetQueryPerformanceTime() const;
		typedef __int64 (Timer::*func)(void) const;

		__int64 cpuFreq_;
		__int64 cpuTicksPerSec_;
		func pfunc_;

		__int64 currTime_;
		__int64 lastTime_;
		float timestep_;
	};
}

#endif