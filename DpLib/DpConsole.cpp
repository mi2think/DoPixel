/********************************************************************
	created:	2014/06/14
	created:	14:6:2014   22:03
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpConsole.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpConsole
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Console
*********************************************************************/

#include "DpConsole.h"
#include <windows.h>

namespace dopixel
{
	void SetConsoleColor(ConsoleColor color)
	{
		HANDLE hWnd = GetStdHandle(STD_OUTPUT_HANDLE);
		switch (color)
		{
		default:
		case COLOR_WHITE:
			SetConsoleTextAttribute(hWnd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;
		case COLOR_RED:
			SetConsoleTextAttribute(hWnd, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case COLOR_GREEN:
			SetConsoleTextAttribute(hWnd, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case COLOR_BLUE:
			SetConsoleTextAttribute(hWnd, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		}
	}

	OStreamHelper os_cout;
}
