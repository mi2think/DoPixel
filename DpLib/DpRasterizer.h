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
#include "DpTextureSampler.h"

namespace dopixel
{
	class Rasterizer
	{
	public:
		Rasterizer();
		Rasterizer(unsigned char* buf, int width, int height, int pitch, float* zbuf);

		void SetBuffer(unsigned char* buf, int width, int height, int pitch, float* zbuf);
		void SetTextureSampler(TextureSampler* textureSampler);

		void DrawPixel(int x, int y, const Color& color);
		void DrawLine(int x0, int y0, int x1, int y1, const Color& color);
		void DrawFrameTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const Color& color);

		template<typename PixelShader, typename VectorT, typename T>
		void DrawTriangle(
			const math::Vector3f& p0, const VectorT& v0,
			const math::Vector3f& p1, const VectorT& v1,
			const math::Vector3f& p2, const VectorT& v2, const T& t);
	private:
		template<typename PixelShader, typename VectorT, typename T>
		void DrawTriangleTop(
			const math::Vector3f& p0, const VectorT& v0,
			const math::Vector3f& p1, const VectorT& v1,
			const math::Vector3f& p2, const VectorT& v2, const T& t);

		template<typename PixelShader, typename VectorT, typename T>
		void DrawTriangleBottom(
			const math::Vector3f& p0, const VectorT& v0,
			const math::Vector3f& p1, const VectorT& v1,
			const math::Vector3f& p2, const VectorT& v2, const T& t);
	private:
		unsigned char* frameBuf_;
		int width_;
		int height_;
		int pitch_;
		float* zbuf_;
		TextureSampler* textureSampler_;
	};

	template<typename PixelShader, typename VectorT, typename T>
	void Rasterizer::DrawTriangle(
		const math::Vector3f& p0, const VectorT& v0,
		const math::Vector3f& p1, const VectorT& v1,
		const math::Vector3f& p2, const VectorT& v2, const T& t)
	{
		math::Vector3f _p0(p0), _p1(p1), _p2(p2);
		VectorT _v0(v0), _v1(v1), _v2(v2);

		// sort by y
		if (_p0.y > _p1.y)
		{
			swap_t(_p0, _p1);
			swap_t(_v0, _v1);
		}
		if (_p0.y > _p2.y)
		{
			swap_t(_p0, _p2);
			swap_t(_v0, _v2);
		}
		if (_p1.y > _p2.y)
		{
			swap_t(_p1, _p2);
			swap_t(_v1, _v2);
		}

		if (equal_t(_p0.y, _p1.y))
		{
			if (equal_t(_p1.y, _p2.y))
				return;
			if (equal_t(_p0.x, _p1.x))
				return;
			if (_p0.x > _p1.x)
			{
				swap_t(_p0, _p1);
				swap_t(_v0, _v1);
			}
			DrawTriangleTop<PixelShader, VectorT, T>(_p0, _v0, _p1, _v1, _p2, _v2, t);
		}
		else if (equal_t(_p1.y, _p2.y))
		{
			if (equal_t(_p1.x, _p2.x))
				return;
			if (_p1.x > _p2.x)
			{
				swap_t(_p1, _p2);
				swap_t(_v1, _v2);
			}
			DrawTriangleBottom<PixelShader, VectorT, T>(_p0, _v0, _p1, _v1, _p2, _v2, t);
		}
		else
		{
			float k = (_p1.y - _p0.y) / (_p2.y - _p0.y);
			math::Vector3f _p3 = (1 - k) * _p0 + k * _p2;
			_p3.y = _p1.y; // avoid error
			VectorT _v3 = (1 - k) * _v0 + k * _v2;
			if (equal_t(_p1.x, _p3.x))
				return;
			if (_p1.x > _p3.x)
			{
				swap_t(_p1, _p3);
				swap_t(_v1, _v3);
			}
			DrawTriangleBottom<PixelShader, VectorT, T>(_p0, _v0, _p1, _v1, _p3, _v3, t);
			DrawTriangleTop<PixelShader, VectorT, T>(_p1, _v1, _p3, _v3, _p2, _v2, t);
		}
	}

	template<typename PixelShader, typename VectorT, typename T>
	void Rasterizer::DrawTriangleTop(
		const math::Vector3f& p0, const VectorT& v0,
		const math::Vector3f& p1, const VectorT& v1,
		const math::Vector3f& p2, const VectorT& v2, const T& t)
	{
		// y start, y end
		int ys = MAX((int)ceil(p0.y), 0);
		int ye = MIN((int)ceil(p2.y) - 1, height_ - 1);

		// sub express
		float yp2p0 = p2.y - p0.y;
		float yp2p1 = p2.y - p1.y;
		float ysp0 = ys - p0.y;
		float ysp1 = ys - p1.y;

		// slope for x in left/right edge
		float slx = (p2.x - p0.x) / yp2p0;
		float srx = (p2.x - p1.x) / yp2p1;
		// x left, x right
		float xl = p0.x + slx * ysp0;
		float xr = p1.x + srx * ysp1;

		// slope for v in left/right edge
		VectorT slv = (v2 - v0) / yp2p0;
		VectorT srv = (v2 - v1) / yp2p1;
		// v left, v right
		VectorT vl = v0 + slv * ysp0;
		VectorT vr = v1 + srv * ysp1;

		// buf
		unsigned char* buf = frameBuf_ + ys * pitch_;

		PixelShader ps;
		for (int y = ys; y <= ye; ++y)
		{
			// x start, x end
			int xs = MAX((int)ceil(xl), 0);
			int xe = MIN((int)ceil(xr) - 1, width_ - 1);

			VectorT step = (xr - xl <= EPSILON_E5) ? VectorT() : (vr - vl) / (xr - xl);
			VectorT vs = vl + step * (xs - xl);

			for (int x = xs; x <= xe; ++x)
			{
				((unsigned int*)buf)[x] = ps(vs, t, textureSampler_);
				vs += step;
			}
			
			xl += slx;
			xr += srx;
			vl += slv;
			vr += srv;
			buf += pitch_;
		}
	}

	template<typename PixelShader, typename VectorT, typename T>
	void Rasterizer::DrawTriangleBottom(
		const math::Vector3f& p0, const VectorT& v0,
		const math::Vector3f& p1, const VectorT& v1,
		const math::Vector3f& p2, const VectorT& v2, const T& t)
	{
		// y start, y end
		int ys = MAX((int)ceil(p0.y), 0);
		int ye = MIN((int)ceil(p1.y) - 1, height_ - 1);
		
		// sub express
		float yp1p0 = p1.y - p0.y;
		float yp2p0 = p2.y - p0.y;
		float ysp0 = ys - p0.y;

		// slope for x in left/right edge
		float slx = (p1.x - p0.x) / yp1p0;
		float srx = (p2.x - p0.x) / yp2p0;
		// x left, x right
		float xl = p0.x + slx * ysp0;
		float xr = p0.x + srx * ysp0;

		// slope for v in left/right edge
		VectorT slv = (v1 - v0) / yp1p0;
		VectorT srv = (v2 - v0) / yp2p0;
		// v left, v right
		VectorT vl = v0 + slv * ysp0;
		VectorT vr = v0 + srv * ysp0;

		// buf
		unsigned char* buf = frameBuf_ + ys * pitch_;

		PixelShader ps;
		for (int y = ys; y <= ye; ++y)
		{
			// x start, x end
			int xs = MAX((int)ceil(xl), 0);
			int xe = MIN((int)ceil(xr) - 1, width_ - 1);

			VectorT step = (xr - xl <= EPSILON_E5) ? VectorT() : (vr - vl) / (xr - xl);
			VectorT vs = vl + step * (xs - xl);

			for (int x = xs; x <= xe; ++x)
			{
				((unsigned int*)buf)[x] = ps(vs, t, textureSampler_);
				vs += step;
			}
			xl += slx;
			xr += srx;
			vl += slv;
			vr += srv;
			buf += pitch_;
		}
	}

	// pixel shader
	struct PSFlat
	{
		unsigned int operator()(float f, const Color& color, const TextureSampler* textureSampler) const
		{
			return color.value;
		}
	};

	struct PSGouraud
	{
		unsigned int operator()(const math::Vector3f& v, const Color& color, const TextureSampler* textureSampler) const
		{
			return Color(v).value;
		}
	};

	struct PSFlatTexture
	{
		unsigned int operator()(const math::Vector2f& uv, const math::Vector3f& color, const TextureSampler* textureSampler) const
		{
			math::Vector3f c = textureSampler->Sample(uv);
			c.x *= color.x;
			c.y *= color.y;
			c.z *= color.z;
			return Color(c).value;
		}
	};
}

#endif