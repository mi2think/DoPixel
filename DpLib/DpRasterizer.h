/********************************************************************
	created:	2015/11/29
	created:	29:11:2015   12:54
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRasterizer.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRasterizer
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Rasterizer
*********************************************************************/
#ifndef __DP_RASTERIZER_H__
#define __DP_RASTERIZER_H__

#include "DoPixel.h"
#include "DpColor.h"

namespace dopixel
{
	class Rasterizer
	{
	public:
		Rasterizer();
		Rasterizer(unsigned char* buf, int width, int height, int pitch, float* zbuf);

		void SetBuffer(unsigned char* buf, int width, int height, int pitch, float* zbuf);

		void DrawPixel(int x, int y, const Color& color);
		void DrawLine(int x0, int y0, int x1, int y1, const Color& color);
		void DrawFrameTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const Color& color);
	private:
		unsigned char* frameBuf_;
		int width_;
		int height_;
		int pitch_;
		float* zbuf_;
	};
}

#endif