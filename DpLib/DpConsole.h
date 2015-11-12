/********************************************************************
	created:	2014/06/14
	created:	14:6:2014   2:31
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpConsole.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpConsole
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Console
*********************************************************************/
#ifndef __DP_CONSOLE__
#define __DP_CONSOLE__

#include <iostream>

namespace dopixel
{
	enum ConsoleColor
	{
		COLOR_WHITE,
		COLOR_RED,
		COLOR_GREEN,
		COLOR_BLUE,
	};

	void SetConsoleColor(ConsoleColor color);

	struct OStreamHelper
	{
		template<typename T>
		inline OStreamHelper& operator << (const T& t)
		{
			std::cout << t;
			return *this;
		}
	};

	inline OStreamHelper& operator << (OStreamHelper& os, OStreamHelper& (*p)(OStreamHelper& os))
	{
		p(os);
		return os;
	}

	inline OStreamHelper& Red(OStreamHelper& os)
	{
		SetConsoleColor(COLOR_RED);
		return os;
	}

	inline OStreamHelper& Green(OStreamHelper& os)
	{
		SetConsoleColor(COLOR_GREEN);
		return os;
	}

	inline OStreamHelper& Blue(OStreamHelper& os)
	{
		SetConsoleColor(COLOR_BLUE);
		return os;
	}

	inline OStreamHelper& White(OStreamHelper& os)
	{
		SetConsoleColor(COLOR_WHITE);
		return os;
	}

	inline OStreamHelper& endl(OStreamHelper& os)
	{
		os << "\n";
		return os;
	}

	extern OStreamHelper os_cout;
}

#endif
