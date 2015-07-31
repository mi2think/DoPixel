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
#include "DpVector4.h"
#include "DpGeometry.h"
#include "DpColor.h"
using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		enum FillMode
		{
			Fill_Wireframe,
			Fill_Solid,
		};

		enum ShadeMode
		{
			Shade_Flat,
			Shade_Gouraud,
			Shade_Phong,
		};

		enum RenderState
		{
			RS_FillMode,
			RS_ShadeMode,
		};

		struct Vertex
		{
			enum Attr
			{
				Attr_None = 0x0,
				Attr_Point = 0x1,
				Attr_Normal = 0x2,
				Attr_Texture = 0x4,
				Attr_Light = 0x8,
			};

			union
			{
				struct
				{
					float x, y, z, w;		// position
					Color color;			// color
					float nx, ny, nz, nw;	// normal
					float u0, v0;			// texture coord

					Color litColor;			// if has Attr_Light	

					float i;	// final vertex intensity after lighting
					int attr;	// Attr
				};

				struct
				{
					Vector4f p;		// position
					Color	 color;	// color
					Vector4f n;		// normal
					Vector2f uv0;	// texture coord

					Color	 litColor;	// if has Attr_Light
				};
			};
		};

		class Color;

		class Device
		{
		public:
			Device();
			
			void Init(unsigned char* buffer, int pitch) { frameBuffer = buffer; this->pitch = pitch; }

			void WritePixel(int x, int y, const Color& color) const { unsigned char* p = frameBuffer + (x + y * pitch) * bitsPerPixel; *((unsigned int*)p) = color.value; }
			
			RectI SetClipRect(const RectI& rect) { RectI rc = clipRect; clipRect = rect; return rc; }

			void SetRenderState(RenderState rs, int value);

			// Clip line, using Cohen-Sutherland, return true if clipRect contain 
			bool ClipLine(Point& pc0, Point& pc1, const Point& p0, const Point& p1) const;

			// Draw a Line, using DDA
			void DrawLineDDA(const Point& p0, const Point& p1, const Color& color) const;
			
			// Draw a Line, using Bresenham, has better performance
			void DrawLine(const Point& p0, const Point& p1, const Color& color) const;

			// Draw a triangle with now render state
			void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
		private:
			// Draw a flat triangle
			void DrawFlatTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) const;

			// Draw a gouraud triangle
			void DrawGouraudTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) const;
		private:

			unsigned char* frameBuffer;
			int pitch;
			int bitsPerPixel;
			RectI clipRect;
			
			int fillMode;
			int shadeMode;
		};
	}
}

#endif
