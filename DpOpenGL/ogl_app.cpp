/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   19:56
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_app.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_app
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl app
*********************************************************************/
#include "ogl_app.h"

#include <windows.h>

namespace ogl
{
	long long GetCurrentTimeMillis()
	{
		return GetTickCount();
	}

	App::App()
		: frameCount_(0)
		, frameTime_(0)
		, fps_(0)
	{
		frameTime_ = startTime_ = GetCurrentTimeMillis();
	}

	void App::CalcFPS()
	{
		++frameCount_;

		long long time = GetCurrentTimeMillis();
		if (time > frameTime_ + 1000)
		{
			frameTime_ = time;
			fps_ = frameCount_;
			frameCount_ = 0;
		}
	}

	void App::RenderFPS()
	{

	}

	float App::GetRunningTime()
	{
		float runningTime = (float)((double)GetCurrentTimeMillis() - (double)startTime_) / 1000.0f;
		return runningTime;
	}
}