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

		template<typename PixelShader, typename T>
		void DrawTriangle(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t);
	private:
		template<typename PixelShader, typename T>
		void DrawTriangleTop(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t);

		template<typename PixelShader, typename T>
		void DrawTriangleBottom(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t);
	private:
		unsigned char* frameBuf_;
		int width_;
		int height_;
		int pitch_;
		float* zbuf_;
	};

	template<typename PixelShader, typename T>
	void Rasterizer::DrawTriangle(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t)
	{
		math::Vector3f _p0(p0);
		math::Vector3f _p1(p1);
		math::Vector3f _p2(p2);

		// sort by y
		if (_p0.y > _p1.y)
			swap_t(_p0, _p1);
		if (_p0.y > _p2.y)
			swap_t(_p0, _p2);
		if (_p1.y > _p2.y)
			swap_t(_p1, _p2);

		if (equal_t(_p0.y, _p1.y))
		{
			if (equal_t(_p1.y, _p2.y))
				return;
			if (equal_t(_p0.x, _p1.x))
				return;
			if (_p0.x > _p1.x)
				swap_t(_p0, _p1);
			DrawTriangleTop<PixelShader, T>(_p0, _p1, _p2, t);
		}
		else if (equal_t(_p1.y, _p2.y))
		{
			if (equal_t(_p1.x, _p2.x))
				return;
			if (_p1.x > _p2.x)
				swap_t(_p1, _p2);
			DrawTriangleBottom<PixelShader, T>(_p0, _p1, _p2, t);
		}
		else
		{
			float k = (_p1.y - _p0.y) / (_p2.y - _p0.y);
			math::Vector3f _p3 = (1 - k) * _p0 + k * _p2;
			_p3.y = _p1.y; // avoid error
			if (equal_t(_p1.x, _p3.x))
				return;
			if (_p1.x > _p3.x)
				swap_t(_p1, _p3);
			DrawTriangleBottom<PixelShader, T>(_p0, _p1, _p3, t);
			DrawTriangleTop<PixelShader, T>(_p1, _p3, _p2, t);
		}
	}

	template<typename PixelShader, typename T>
	void Rasterizer::DrawTriangleTop(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t)
	{
		// y start, y end
		int ys = MAX((int)ceil(p0.y), 0);
		int ye = MIN((int)ceil(p2.y) - 1, height_ - 1);
		// slope for x in left/right edge
		float slx = (p2.x - p0.x) / (p2.y - p0.y);
		float srx = (p2.x - p1.x) / (p2.y - p1.y);
		// x left, x right
		float xl = p0.x + slx * (ys - p0.y);
		float xr = p1.x + srx * (ys - p1.y);
		// buf
		unsigned char* buf = frameBuf_ + ys * pitch_;

		PixelShader ps;
		for (int y = ys; y <= ye; ++y)
		{
			// x start, x end
			int xs = MAX((int)ceil(xl), 0);
			int xe = MIN((int)ceil(xr) - 1, width_ - 1);

			for (int x = xs; x <= xe; ++x)
			{
				((unsigned int*)buf)[x] = ps(t);
			}
			xl += slx;
			xr += srx;
			buf += pitch_;
		}
	}

	template<typename PixelShader, typename T>
	void Rasterizer::DrawTriangleBottom(const math::Vector3f& p0, const math::Vector3f& p1, const math::Vector3f& p2, const T& t)
	{
		// y start, y end
		int ys = MAX((int)ceil(p0.y), 0);
		int ye = MIN((int)ceil(p1.y) - 1, height_ - 1);
		// slope for x in left/right edge
		float slx = (p1.x - p0.x) / (p1.y - p0.y);
		float srx = (p2.x - p0.x) / (p2.y - p0.y);
		// x left, x right
		float xl = p0.x + slx * (ys - p0.y);
		float xr = p0.x + srx * (ys - p0.y);
		// buf
		unsigned char* buf = frameBuf_ + ys * pitch_;

		PixelShader ps;
		for (int y = ys; y <= ye; ++y)
		{
			// x start, x end
			int xs = MAX((int)ceil(xl), 0);
			int xe = MIN((int)ceil(xr) - 1, width_ - 1);

			for (int x = xs; x <= xe; ++x)
			{
				((unsigned int*)buf)[x] = ps(t);
			}
			xl += slx;
			xr += srx;
			buf += pitch_;
		}
	}

	// pixel shader
	struct PSFlat
	{
		unsigned int operator()(const Color& color) const
		{
			return color.value;
		}
	};
}

#endif