/********************************************************************
	created:	2015/09/04
	created:	4:9:2015   23:31
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTriangle.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTriangle
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Triangle
*********************************************************************/


/*
 *
 * INTERP_Z		- if defined, interpolate Z values
 * INTERP_RGB	- if defined, interpolate RGB values
 * INTERP_UV	- if defined, interpolate UV values
 *
 *
 *
 *
 *
 *
 *
 *
 */

//void Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	// helper class
	struct Gradients
	{
#if INTERP_RGB
		float drdx; float drdy;
		float dgdx; float dgdy;
		float dbdx; float dbdy;
#endif
#if INTERP_Z
		float z[3];					// 1/z for each vertex
		float dzdx; float dzdy;		// d(1/z)/dx, d(1/z)/dy
#endif
#if INTERP_UV
		float u[3];					// u/z for each vertex
		float v[3];					// v/z for each vertex
		float dudx; float dudy;		// d(u/z)/dx, d(u/z)/dy
		float dvdx; float dvdy;		// d(v/z)/dx, d(v/z)/dy
#endif

		Gradients(const Vertex** pVertices)
		{
#if INTERP_RGB || INTERP_Z || INTERP_UV
			const int counter = 3;

			// common sub compute
			float y0_y2 = pVertices[0]->y - pVertices[2]->y;	// y0 - y2
			float y1_y2 = pVertices[1]->y - pVertices[2]->y;	// y1 - y2
			float x0_x2 = pVertices[0]->x - pVertices[2]->x;	// x0 - x2
			float x1_x2 = pVertices[1]->x - pVertices[2]->x;	// x1 - x2

			// equation:
			// dx = (x1-x2)(y0-y2) + (x2-x0)(y1-y2)
			// dy = -dx
			float dx = x1_x2 * y0_y2 - x0_x2 * y1_y2;
			float oneOverdx = 1.0f / dx;
			float oneOverdy = -oneOverdx;

			// equation:
			// dc / dx = ((c1-c2)(y0-y2) - (c0-c2)(y1-y2)) / dx
			// dc / dy = ((c1-c2)(x0-x2) - (c0-c2)(x1-x2)) / dy
#if INTERP_RGB
			float r[3]; float g[3]; float b[3];
			for (int i = 0; i < counter; ++i)
			{
				const auto& color = pVertices[i]->color;
				r[i] = color.r;
				g[i] = color.g;
				b[i] = color.b;
			}
			drdx = oneOverdx * ((r[1] - r[2]) * y0_y2 - (r[0] - r[2]) * y1_y2);
			dgdx = oneOverdx * ((g[1] - g[2]) * y0_y2 - (g[0] - g[2]) * y1_y2);
			dbdx = oneOverdx * ((b[1] - b[2]) * y0_y2 - (b[0] - b[2]) * y1_y2);

			drdy = oneOverdy * ((r[1] - r[2]) * x0_x2 - (r[0] - r[2]) * x1_x2);
			dgdy = oneOverdy * ((g[1] - g[2]) * x0_x2 - (g[0] - g[2]) * x1_x2);
			dbdy = oneOverdy * ((b[1] - b[2]) * x0_x2 - (b[0] - b[2]) * x1_x2);
#endif
#if INTERP_Z
			z[0] = 1.0f / pVertices[0]->z;
			z[1] = 1.0f / pVertices[1]->z;
			z[2] = 1.0f / pVertices[2]->z;
			dzdx = oneOverdx * ((z[1] - z[2]) * y0_y2 - (z[0] - z[2]) * y1_y2);
			dzdy = oneOverdy * ((z[1] - z[2]) * x0_x2 - (z[0] - z[2]) * x1_x2);
#endif
#endif
		}
	};

	struct Edge
	{
		float x; float dxdy;		// current x and dx/dy
		int y; int height;			// current y and vertical count
#if INTERP_RGB
		float r; float drdy;		// current r, g, b
		float g; float dgdy;		// and dr/dy, dg/dy, db/dy
		float b; float dbdy;
#endif
#if INTERP_Z
		float z; float dzdy;		// current 1/z, d(1/z)/dy
#endif
#if INTERP_UV
		float u; float dudy;		// u/z, d(u/z)/dy
		float v; float dvdy;		// v/z, d(v/z)/dy
#endif
		Edge(const Gradients& gradients, const Vertex** pVertices, int top, int bottom)
		{
			const Vertex* vtop = *(pVertices + top);
			const Vertex* vbottom = *(pVertices + bottom);

			y = (int)ceil(vtop->y);
			int yend = (int)ceil(vbottom->y);
			height = yend - y;

			float y_pre_step = y - vtop->y;

			float real_width = vbottom->x - vtop->x;
			float real_height = vbottom->y - vtop->y;

			dxdy = real_width / real_height;
			x = vtop->x + y_pre_step * dxdy;
#if INTERP_RGB || INTERP_Z || INTERP_UV
			float x_pre_step = x - vtop->x;
#endif
#if INTERP_RGB
			drdy = gradients.drdy + dxdy * gradients.drdx;
			dgdy = gradients.dgdy + dxdy * gradients.dgdx;
			dbdy = gradients.dbdy + dxdy * gradients.dbdx;

			r = vtop->color.r + y_pre_step * drdy + x_pre_step * gradients.drdx;
			g = vtop->color.g + y_pre_step * dgdy + x_pre_step * gradients.drdx;
			b = vtop->color.b + y_pre_step * dbdy + x_pre_step * gradients.drdx;
#endif
#if INTERP_Z
			// 1/z step depend on both y step and x step
			dzdy = gradients.dzdy + dxdy * gradients.dzdx;
			z = gradients.z[top] + y_pre_step * gradients.dzdy + x_pre_step * gradients.dzdx;
#endif
#if INTERP_UV
#endif
		}

		int Step()
		{
			x += dxdy;
			++y;
			--height;

#if INTERP_RGB
			r += drdy;
			g += dgdy;
			b += dbdy;
#endif
#if INTERP_Z
			z += dzdy;
#endif
			return height;
		}
	};

	struct SCANLINE_LOOP
	{
		static void Loop(const Device& device, int height, const Gradients& gradients, Edge* edgeL, Edge* edgeR, const Vertex& v0)
		{
			unsigned int pitchBits = device.pitch * device.bitsPerPixel;
			unsigned char* buffer = device.frameBuffer + edgeL->y * pitchBits;

			while (height--)
			{
				int x_start = (int)ceil(edgeL->x);
				int width = (int)ceil(edgeR->x) - x_start;

#if INTERP_RGB || INTERP_Z || INTERP_UV
				float x_pre_step = x_start - edgeL->x;
#endif
#if INTERP_RGB
				float r = edgeL->r + x_pre_step * gradients.drdx;
				float g = edgeL->g + x_pre_step * gradients.dgdx;
				float b = edgeL->b + x_pre_step * gradients.dbdx;
#endif
#if INTERP_Z
				float z = edgeL->z + x_pre_step * gradients.dzdx;
#endif
				if (width > 0)
				{
					unsigned int* p = (unsigned int*)(buffer + x_start * device.bitsPerPixel);

					while (width--)
					{
#if INTERP_RGB
						// draw color
						*p = Color((unsigned char)r, (unsigned char)g, (unsigned char)b).value;

						r += gradients.drdx;
						g += gradients.dgdx;
						b += gradients.dbdx;
#endif
#if INTERP_Z
						z += gradients.dzdx;
#endif
#if (!INTERP_RGB) && (!INTERP_UV)
						*p = v0.color.value;
#endif
						++p;
					}
				}
				edgeL->Step();
				edgeR->Step();
				buffer += pitchBits;
			}
		}
	};

	// begin deal
	{
		const Vertex* pVertices[3] = { &v0, &v1, &v2 };

		int top, middle, bottom;
		float y0 = pVertices[0]->y;
		float y1 = pVertices[1]->y;
		float y2 = pVertices[2]->y;

		// sort vertices in y
		if (y0 < y1) {
			if (y2 < y0) {
				top = 2; middle = 0; bottom = 1;
			}
			else {
				top = 0;
				if (y1 < y2) {
					middle = 1; bottom = 2;
				}
				else {
					middle = 2; bottom = 1;
				}
			}
		}
		else {
			if (y2 < y1) {
				top = 2; middle = 1; bottom = 0;
			}
			else {
				top = 1;
				if (y0 < y2) {
					middle = 0; bottom = 2;
				}
				else {
					middle = 2; bottom = 0;
				}
			}
		}
		// middle is left or right?
		float xnew = pVertices[top]->x + (pVertices[bottom]->x - pVertices[top]->x) * (pVertices[middle]->y - pVertices[top]->y) / (pVertices[bottom]->y - pVertices[top]->y);
		bool middleIsLeft = (xnew > pVertices[middle]->x);

		Gradients gradients(pVertices);
		Edge edgeTB(gradients, pVertices, top, bottom);		// edge - top to bottom
		Edge edgeTM(gradients, pVertices, top, middle);		// edge - top to middle
		Edge edgeMB(gradients, pVertices, middle, bottom);	// edge - middle to bottom
		Edge* edgeL; // edge - left
		Edge* edgeR; // edge - right

		// triangle is clockwise, so if bottom > middle then middle is right
		if (middleIsLeft)
		{
			edgeL = &edgeTM;
			edgeR = &edgeTB;
		}
		else
		{
			edgeL = &edgeTB;
			edgeR = &edgeTM;
		}
		SCANLINE_LOOP::Loop(*this, edgeTM.height, gradients, edgeL, edgeR, v0);

		if (middleIsLeft)
		{
			edgeL = &edgeMB;
			edgeR = &edgeTB;
		}
		else
		{
			edgeL = &edgeTB;
			edgeR = &edgeMB;
		}
		SCANLINE_LOOP::Loop(*this, edgeMB.height, gradients, edgeL, edgeR, v0);
	}
}

#undef INTERP_Z
#undef INTERP_RGB
#undef INTERP_UV
