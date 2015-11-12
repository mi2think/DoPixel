/********************************************************************
	created:	2014/07/04
	created:	4:7:2014   0:07
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpDevice.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpDevice
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Device, For Rasterization
*********************************************************************/

#ifndef __DP_DEVICE__
#define __DP_DEVICE__

#include "DpGeometry.h"
#include "DpEnums.h"
#include "DpVertex.h"
using namespace dopixel::math;

namespace dopixel
{
	// 32-bit z buffer
	struct ZBuffer
	{
		float* buffer;
		int width;
		int height;
		int attr;

		ZBuffer() : buffer(nullptr), width(0), height(0), attr(0) {}
		void Create(int width, int height);
		void Clear(float value);
		void Delete();
	};

	class Color;
	class Texture;

	// render content
	class Device
	{
	public:
		Device();
		~Device();

		void Create(int width, int height);

		void Init(unsigned char* buffer, int pitch) { frameBuffer = buffer; this->pitch = pitch; }

		void BeginScene();

		void EndScene();

		void WritePixel(int x, int y, const Color& color) const { unsigned char* p = frameBuffer + (x + y * pitch) * bitsPerPixel; *((unsigned int*)p) = color.value; }

		RectI SetClipRect(const RectI& rect) { RectI rc = clipRect; clipRect = rect; return rc; }

		void SetRenderState(RenderState rs, int value);

		void SetTexture(Texture* texture) { this->texture = texture; }

		// Clip line, using Cohen-Sutherland, return true if clipRect contain 
		bool ClipLine(Point& pc0, Point& pc1, const Point& p0, const Point& p1) const;

		// Draw a Line, using DDA
		void DrawLineDDA(const Point& p0, const Point& p1, const Color& color);

		// Draw a Line, using Bresenham, has better performance
		void DrawLine(const Point& p0, const Point& p1, const Color& color);

		// Draw a triangle with now render state
		void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	private:
		void draw_wireframe_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_flat_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_gouraud_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_textured_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

		void draw_flat_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_gouraud_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_textured_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

		void draw_flat_inv_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_gouraud_inv_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		void draw_textured_inv_zb_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	private:
		int width;
		int height;

		unsigned char* frameBuffer;
		int pitch;
		int bitsPerPixel;
		RectI clipRect;

		int fillMode;
		int shadeMode;
		int zEnable;
		int zFunc;
		int zWriteEnable;
		Texture* texture;

		ZBuffer zbuffer;
	};
}

#endif
