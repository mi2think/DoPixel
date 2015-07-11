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

#include "DpVector2.h"
#include "DpGeometry.h"
#include "DpColor.h"
using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		class Color;

		class Device
		{
		public:
			Device() : frameBuffer(NULL), pitch(0), bitsPerPixel(4) {}
			
			void Init(unsigned char* buffer, int pitch) { frameBuffer = buffer; this->pitch = pitch; }

			void WritePixel(int x, int y, const Color& color) const { unsigned char* p = frameBuffer + (x + y * pitch) * bitsPerPixel; *((unsigned int*)p) = color.value; }
			
			RectI SetClipRect(const RectI& rect) { RectI rc = clipRect; clipRect = rect; return rc; }

			// Clip line, using Cohen-Sutherland, return true if clipRect contain 
			bool ClipLine(Point& pc0, Point& pc1, const Point& p0, const Point& p1) const;

			// Draw a Line, using DDA
			void DrawLineDDA(const Point& p0, const Point& p1, const Color& color) const;
			
			// Draw a Line, using Bresenham, has better performance
			void DrawLine(const Point& p0, const Point& p1, const Color& color) const;

			// Draw a triangle
			void DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color) const;

			// Draw a gouraud triangle
			void DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color0, const Color& color1, const Color& color2) const;
		private:
			unsigned char* frameBuffer;
			int pitch;
			int bitsPerPixel;
			RectI clipRect;
		};
	}
}

#endif
