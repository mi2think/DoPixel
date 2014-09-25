/********************************************************************
	created:	2014/09/02
	created:	2:9:2014   0:37
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpColor.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpColor
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Predefine Color
*********************************************************************/

#include "DpColor.h"

namespace DoPixel
{
	namespace Core
	{
		const Color Color::white	= Color(255, 255, 255);
		const Color Color::gray		= Color(100, 100, 100);
		const Color Color::black	= Color(0, 0, 0);
		const Color Color::red		= Color(255, 0, 0);
		const Color Color::green	= Color(0, 255, 0);
		const Color Color::blue		= Color(0, 0, 255);
	}
}