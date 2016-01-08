/********************************************************************
	created:	2016/01/07
	created:	7:1:2016   23:33
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLog.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLog
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Log
*********************************************************************/
#include "DpLog.h"

#include <windows.h>
#include <iostream>
#include <cstdarg>

namespace dopixel
{
	Log g_Log;

	Log::Log() : hwnd_(nullptr)
	{}

	void Log::WriteBuf(int level, const char* fmt, ...)
	{
		switch (level)
		{
		case Info:
			// info is bright white
			SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case Warn:
			// warn is bright yellow
			SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case Error:
			// error is red
			SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case Fatal:
			// fatal is white on red
			SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY);
			break;
		default:
			// other is cyan
			SetTextColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		}

		static char buffer[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf_s(buffer, fmt, ap);
		va_end(ap);
		buffer[1023] = 0;

		if (level == Error || level == Fatal)
			std::cout << buffer << std::endl;
		else
			std::cerr << buffer << std::endl;
	}

	void Log::SetTextColor(unsigned short color)
	{
		if (hwnd_ == NULL)
		{
			hwnd_ = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		SetConsoleTextAttribute(hwnd_, color);
	}
}