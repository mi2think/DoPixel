/********************************************************************
	created:	2015/11/29
	created:	29:11:2015   12:55
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRasterizer.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRasterizer
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Rasterizer
*********************************************************************/
#include "DpRasterizer.h"

namespace dopixel
{
	Rasterizer::Rasterizer()
		: frameBuf_(nullptr)
		, width_(0)
		, height_(0)
		, pitch_(0)
		, zbuf_(nullptr)
		, textureSampler_(nullptr)
	{
	}

	Rasterizer::Rasterizer(unsigned char* buf, int width, int height, int pitch, float* zbuf)
		: frameBuf_(buf)
		, width_(width)
		, height_(height)
		, pitch_(pitch)
		, zbuf_(zbuf)
		, textureSampler_(nullptr)
	{
	}

	void Rasterizer::SetBuffer(unsigned char* buf, int width, int height, int pitch, float* zbuf)
	{
		frameBuf_ = buf;
		width_ = width;
		height_ = height;
		pitch_ = pitch;
		zbuf_ = zbuf;
	}

	void Rasterizer::SetTextureSampler(TextureSampler* textureSampler)
	{
		textureSampler_ = textureSampler;
	}

	void Rasterizer::DrawPixel(int x, int y, const Color& color)
	{
		ASSERT(x >= 0 && x < width_);
		ASSERT(y >= 0 && x < height_);

		((unsigned int*)(frameBuf_ + y * pitch_))[x] = color.value;
	}

	void Rasterizer::DrawLine(int x0, int y0, int x1, int y1, const Color& color)
	{
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);

		// adjust x, y pos
		int xstart = x0;
		int ystart = y0;

		int xadd = x1 > x0 ? 1 : -1;
		int yadd = y1 > y0 ? 1 : -1;

		if (dx > dy)
		{
			// d start = 2dy - dx
			int d = 2 * dy - dx;
			int incre = 2 * dy;
			int incrne = 2 * (dy - dx);

			for (int i = 0; i <= dx; ++i)
			{
				if (xstart >= 0 && xstart < width_ &&
					ystart >= 0 && ystart < height_)
				{
					DrawPixel(xstart, ystart, color);
				}

				if (d <= 0)
				{
					// choose e
					d += incre;
					xstart += xadd;
				}
				else
				{
					// choose ne
					d += incrne;
					xstart += xadd;
					ystart += yadd;
				}
			}
		}
		else
		{
			// symmetry on y = x, or y = -x

			// d start = 2dx - dy
			int d = 2 * dx - dy;
			int incre = 2 * dx;
			int incrne = 2 * (dx - dy);

			for (int i = 0; i <= dy; ++i)
			{
				if (xstart >= 0 && xstart < width_ &&
					ystart >= 0 && ystart < height_)
				{
					DrawPixel(xstart, ystart, color);
				}

				if (d <= 0)
				{
					// choose e
					d += incre;
					ystart += yadd;
				}
				else
				{
					// choose ne
					d += incrne;
					xstart += xadd;
					ystart += yadd;
				}
			}
		}
	}

	void Rasterizer::DrawFrameTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const Color& color)
	{
		DrawLine(x0, y0, x1, y1, color);
		DrawLine(x0, y0, x2, y2, color);
		DrawLine(x1, y1, x2, y2, color);
	}
}