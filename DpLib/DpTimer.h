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

#include <windows.h>

namespace DoPixel
{
	namespace Core
	{
		class Timer
		{
		public:
			Timer() : cpuFreq(0), cpuTicksPerSec(0), pfunc(nullptr), currTime(0), lastTime(0), timestep(0) { Init(); }

			inline void Init();

			inline __int64 GetTime() const;

			inline __int64 GetMMTime(void) const;

			inline __int64 GetQueryPerformanceTime(void) const;

			// Get time step in sec
			inline float GetTimestep() const;

			inline float Tick();

			inline void Reset();

			typedef __int64 (Timer::*func)(void) const;
		private:
			__int64 cpuFreq;
			__int64 cpuTicksPerSec;
			func  pfunc;

			__int64 currTime;
			__int64 lastTime;
			float timestep;
		};

		inline void Timer::Init()
		{
			LARGE_INTEGER freq;
			if (QueryPerformanceFrequency(&freq))
			{
				LARGE_INTEGER t;
				QueryPerformanceCounter(&t);
				cpuTicksPerSec = freq.QuadPart;
				pfunc = &Timer::GetQueryPerformanceTime;
			}
			else
			{
				pfunc = &Timer::GetMMTime;
				cpuTicksPerSec = 1000;
			}
		}

		inline __int64 Timer::GetTime() const
		{
			return (this->*pfunc)();
		}

		inline __int64 Timer::GetMMTime() const
		{
			__int64 time = timeGetTime();
			return time;
		}

		inline __int64 Timer::GetQueryPerformanceTime() const
		{
			LARGE_INTEGER t;
			QueryPerformanceCounter(&t);
			__int64 time = t.QuadPart;
			return time;
		}

		inline float Timer::Tick()
		{
			currTime = GetTime();
			timestep = (float)(currTime - lastTime) / cpuTicksPerSec;
			return timestep;
		}

		inline void Timer::Reset()
		{
			lastTime = currTime;
		}

		inline float Timer::GetTimestep() const
		{
			return timestep;
		}
	}
}

#endif