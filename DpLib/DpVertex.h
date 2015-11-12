/********************************************************************
	created:	2015/09/14
	created:	14:9:2015   0:45
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpVertex.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpVertex
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Vertex def
*********************************************************************/
#ifndef __DP_VERTEX__
#define __DP_VERTEX__

#include "DpVector2.h"
#include "DpVector4.h"
#include "DpColor.h"

using namespace dopixel::math;

namespace dopixel
{
	struct Vertex
	{
		enum Attr
		{
			Attr_None = 0x0,
			Attr_Point = 0x1,
			Attr_Normal = 0x2,
			Attr_Texture = 0x4,
			Attr_Lit = 0x8,
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

		Vertex() {}
		Vertex(const Vertex& v)
			: p(v.p)
			, color(v.color)
			, n(v.n)
			, uv0(v.uv0)
			, litColor(v.litColor)
			, i(v.i)
			, attr(v.attr)
		{
		}
		Vertex& operator=(const Vertex& v)
		{
			p = v.p;
			color = v.color;
			n = v.n;
			uv0 = v.uv0;
			litColor = v.litColor;
			i = v.i;
			attr = v.attr;
			return *this;
		}

		Vertex Interpolate(const Vertex& v1, float t) const
		{
			// vt = v0 + (v1 - v0) * t, t:[0, 1]
			Vertex vt;
			vt.p = this->p.Interpolate(v1.p, t);
			vt.color = this->color.Interpolate(v1.color, t);
			vt.n = this->n.Interpolate(v1.n, t);
			vt.uv0 = this->uv0.Interpolate(v1.uv0, t);
			vt.attr = this->attr;
			return vt;
		}
	};
}

#endif
