/********************************************************************
	created:	2014/07/04
	created:	4:7:2014   0:10
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpDevice.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpDevice
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Device, For Rasterization
*********************************************************************/
#include "DpDevice.h"
#include "DpCore.h"

namespace DoPixel
{
	namespace Core
	{
#define Round(x) ((int)((x) + 0.5))

		void Device::DrawLineDDA(const Point& p0, const Point& p1, const Color& color) const
		{
			Point pc0;
			Point pc1;
			if (!ClipLine(pc0, pc1, p0, p1))
				return;

			float dx = pc1.x - pc0.x;
			float dy = pc1.y - pc0.y;

			if (fabs(dx) > fabs(dy))
			{
				// x dir interpolation, length
				int lenght = abs(Round(p1.x) - Round(p0.x));
				// adjust x, y start pos
				int xstart = Round(pc0.x);
				float ystart = pc0.y + ((float)xstart - pc0.x) * dy / dx;

				// x, y add value
				int xadd = dx > 0.0f ? 1 : -1;
				float yadd = dy / fabs(dx);

				for (int i = 0; i <= lenght; ++i)
				{
					WritePixel(xstart, Round(ystart), color);
					xstart += xadd;
					ystart += yadd;
				}
			}
			else
			{
				// y dir interpolation, length
				int length = abs(Round(pc1.y) - Round(pc0.y));
				// adjust x, y start pos
				int ystart = Round(pc0.y);
				float xstart = pc0.x + ((float)ystart - pc0.y) * dx / dy;

				// x, y add value
				int yadd = dy > 0.0f ? 1 : -1;
				float xadd = dx / fabs(dy);

				for (int i = 0; i <= length; ++i)
				{
					WritePixel(Round(xstart), ystart, color);
					xstart += xadd;
					ystart += yadd;
				}
			}
		}

		void Device::DrawLine(const Point& p0, const Point& p1, const Color& color) const
		{
			Point pc0;
			Point pc1;
			if (!ClipLine(pc0, pc1, p0, p1))
				return;

			int x0 = Round(pc0.x);
			int y0 = Round(pc0.y);
			int x1 = Round(pc1.x);
			int y1 = Round(pc1.y);

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
					WritePixel(xstart, ystart, color);
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
					WritePixel(xstart, ystart, color);
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

		bool Device::ClipLine(Point& pc0, Point& pc1, const Point& p0, const Point& p1) const
		{
			// Cohen-Sutherland
			//	1001  | 1000  | 1010
			//	-------------------
			//	0001  | 0000  | 0010
			//	-------------------
			//	0101  | 0100  | 0110

#define CODE_CENTER		0x0000
#define CODE_TOP		0x0008
#define CODE_LEFT		0x0001
#define CODE_RIGHT		0x0002
#define CODE_BOTTOM		0x0004
#define CODE_TOPLEFT	(CODE_TOP | CODE_LEFT)
#define CODE_TOPRIGHT	(CODE_TOP | CODE_RIGHT)
#define CODE_BOTTOMLEFT (CODE_BOTTOM | CODE_LEFT)
#define CODE_BOTTOMRIGHT (CODE_BOTTOM | CODE_RIGHT)  

			auto fnCode = [this](const Point& pt) -> int
			{
				int code = 0;
				if (pt.x < clipRect.left)
					code |= CODE_LEFT;
				if (pt.x > clipRect.right)
					code |= CODE_RIGHT;
				if (pt.y < clipRect.top)
					code |= CODE_TOP;
				if (pt.y > clipRect.bottom)
					code |= CODE_BOTTOM;
				return code;
			};

			int code0 = fnCode(p0);
			int code1 = fnCode(p1);

			if (code0 == 0 && code1 == 0)
			{
				pc0 = p0;
				pc1 = p1;
				return true;
			}
			else if ((code0 & code1) != 0)
				return false;

			// (y - y0) / (y1 - y0) = (x - x0) / (x1 - x0)
			// x' = x0 + (x1 - x0) * (y - y0) / (y1 - y0)
			// y' = y0 + (y1 - y0) * (x - x0) / (x1 - x0)

			// (y - y1) / (y0 - y1) = (x - x1) / (x0 - x1)
			// x" = x1 + (x0 - x1) * (y - y1) / (y0 - y1)
			// y" = y1 + (y0 - y1) * (x - x1) / (x1 - x0)

			// pt is p0 or p1
			auto fnClip = [this](const Point& pt, const Point& p0, const Point& p1, int code) -> Point
			{
				Point pc;
				switch (code)
				{
				case CODE_CENTER:
					pc = pt;
					break;
				case CODE_TOP:
					pc.y = (float)clipRect.top;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					break;
				case CODE_BOTTOM:
					pc.y = (float)clipRect.bottom;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					break;
				case CODE_LEFT:
					pc.x = (float)clipRect.left;
					pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					break;
				case CODE_RIGHT:
					pc.x = (float)clipRect.right;
					pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					break;
				case CODE_TOPLEFT:
					// first clip top
					pc.y = (float)clipRect.top;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					// if the x is less than left, then clip left
					if (pc.x < clipRect.left || pc.x > clipRect.right)
					{
						pc.x = (float)clipRect.left;
						pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					}
					break;
				case CODE_TOPRIGHT:
					// first clip top
					pc.y = (float)clipRect.top;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					if (pc.x > clipRect.right || pc.x < clipRect.left)
					{
						pc.x = (float)clipRect.right;
						pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					}
					break;
				case CODE_BOTTOMLEFT:
					// first clip bottom
					pc.y = (float)clipRect.bottom;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					// if the x is less than left, then clip left
					if (pc.x < clipRect.left || pc.x > clipRect.right)
					{
						pc.x = (float)clipRect.left;
						pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					}
					break;
				case CODE_BOTTOMRIGHT:
					// first clip bottom
					pc.y = (float)clipRect.bottom;
					pc.x = pt.x + (p1.x - p0.x) * (pc.y - p0.y) / (p1.y - p0.y);
					if (pc.x > clipRect.right || pc.x < clipRect.left)
					{
						pc.x = (float)clipRect.right;
						pc.y = pt.y + (p1.y - p0.y) * (pc.x - p0.x) / (p1.x - p0.x);
					}
					break;
				}
				return pc;
			};

			pc0 = fnClip(p0, p0, p1, code0);
			pc1 = fnClip(p1, p1, p0, code1);

			if (!clipRect.PtInRect(pc0) || !clipRect.PtInRect(pc1))
				return false;

			return true;
		}

		void Device::DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color) const
		{
			if (FCMP(p0.x, p1.x) && FCMP(p1.x, p2.x) || FCMP(p0.y, p1.y) && FCMP(p1.y, p2.y))
				return;

			// Sort v0, v1, v2 in ascending y order
			Point v0 = p0;
			Point v1 = p1;
			Point v2 = p2;

			if (v1.y < v0.y)
				Swap(v1, v0);

			if (v2.y < v0.y)
				Swap(v0, v2);

			if (v2.y < v1.y)
				Swap(v2, v1);

			// Cull
			if (v2.y < clipRect.top || v0.y > clipRect.bottom ||
				v0.x < clipRect.left && v1.x < clipRect.left && v2.x < clipRect.left ||
				v0.x > clipRect.right && v1.x > clipRect.right && v2.x > clipRect.right)
				return;

			enum TypeTriangle { TypeFlatTop, TypeFlatBottom, TypeGeneral };
		
			TypeTriangle type;
			if (FCMP(v0.y, v1.y))
				type = TypeFlatTop;
			else if (FCMP(v1.y, v2.y))
				type = TypeFlatBottom;
			else
				type = TypeGeneral;

			if (type == TypeFlatTop || type == TypeFlatBottom)
			{
				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				//float y2 = p1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				float height = y3 - y1;

				float dx_left;
				float dx_right;

				// starting points
				float xs;
				float xe;

				if (type == TypeFlatTop)
				{
					if (x1 > x2)
						Swap(x1, x2);

					dx_left = (x3 - x1) / height;
					dx_right = (x3 - x2) / height;

					xs = x1;
					xe = x2;
				}
				else
				{
					if (x3 > x2)
						Swap(x2, x3);

					dx_left = (x3 - x1) / height;
					dx_right = (x2 - x1) / height;

					xs = x1;
					xe = x1;
				}

				int iy1, iy3;

				// check y1
				float dy = 0.0f;
				if (y1 < clipRect.top)
				{
					dy = clipRect.top - y1;

					// clip y1
					float y1_clip = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1_clip;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					dy = iy1 - y1;
				}
				// compute new xs and xe
				xs = xs + dx_left * dy;
				xe = xe + dx_right * dy;

				// check y3
				if (y3 > clipRect.bottom)
				{
					// clip y
					float y3_clip = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3_clip - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = (int)ceil(y3) - 1;
				}

				unsigned int pitchBits = pitch * bitsPerPixel;
				unsigned char* buffer = frameBuffer + iy1 * pitchBits;

				// test if x clipping is needed
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					//draw the triangle
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// point start
						int xstart = (int)ceil(xs);
						int xend = (int)ceil(xe) - 1;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = color.value;
						}

						xs += dx_left;
						xe += dx_right;
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// clip test
						float xs_clip = xs;
						float xe_clip = xe;

						if (xs_clip < clipRect.left)
						{
							xs_clip = (float)clipRect.left;
							if (xe_clip < clipRect.left)
								continue;
						}

						if (xe_clip > clipRect.right)
						{
							xe_clip = (float)clipRect.right;
							if (xs_clip > clipRect.right)
								continue;
						}

						// point start
						int xstart = 0;
						int xend = 0;

						if (FCMP(xs_clip, xs))
							xstart = (int)ceil(xs);
						else
							xstart = (int)xs_clip;

						if (FCMP(xe_clip, xe))
							xend = (int)ceil(xe) - 1;
						else
							xend = (int)xe_clip;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = color.value;
						}

						xs += dx_left;
						xe += dx_right;
					}
				}
			}
			else if (type == TypeGeneral)
			{
				// new point
				float xnew = v0.x + (v2.x - v0.x) * (v1.y - v0.y) / (v2.y - v0.y);
				float ynew = v1.y;

				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				float y2 = v1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				enum TypeNewPoint { TypeLHS, TypeRHS };
				TypeNewPoint type = xnew > x2 ? TypeRHS : TypeLHS;

				float height_left;
				float height_right;
				float dx_left;
				float dx_right;

				if (type == TypeRHS)
				{
					height_left = y2 - y1;
					height_right = y3 - y1;
					dx_left = (x2 - x1) / height_left;
					dx_right = (x3 - x1) / height_right;
				}
				else
				{
					height_left = y3 - y1;
					height_right = y2 - y1;
					dx_left = (x3 - x1) / height_left;
					dx_right = (x2 - x1) / height_right;
				}

				// starting points
				float xs = x1;
				float xe = x1;

				int iy1, iy3;

				// check y1
				float dy = 0.0f;
				if (y1 < clipRect.top)
				{
					dy = clipRect.top - y1;

					// clip y1
					float y1_clip = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1_clip;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					dy = iy1 - y1;
				}
				// compute new xs and xe
				xs = xs + dx_left * dy;
				xe = xe + dx_right * dy;

				// check y3
				if (y3 > clipRect.bottom)
				{
					// clip y
					float y3_clip = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3_clip - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = (int)ceil(y3) - 1;
				}

				// check yrestart
				int yrestart = (int)ceil(ynew) - 1;
				if (yrestart < iy1)
					yrestart = iy1;

				unsigned int pitchBits = pitch * bitsPerPixel;
				unsigned char* buffer = frameBuffer + iy1 * pitchBits;

				// test if x clipping is needed
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// point start
						int xstart = (int)ceil(xs);
						int xend = (int)ceil(xe) - 1;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = color.value;
						}

						xs += dx_left;
						xe += dx_right;

						// test for loop_y hitting second region, if so change interpolant
						if (loop_y == yrestart)
						{
							float height_new = (y3 - ynew);

							if (type == TypeLHS)
							{
								dx_right = (x3 - x2) / height_new;
								xe = x2;
								xe += dx_right;
							}
							else
							{
								dx_left = (x3 - x2) / height_new;
								xs = x2;
								xs += dx_left;
							}
						}
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// clip test
						float xs_clip = xs;
						float xe_clip = xe;

						if (xs_clip < clipRect.left)
						{
							xs_clip = (float)clipRect.left;
							if (xe_clip < clipRect.left)
								continue;
						}

						if (xe_clip > clipRect.right)
						{
							xe_clip = (float)clipRect.right;
							if (xs_clip > clipRect.right)
								continue;
						}

						// point start
						int xstart = 0;
						int xend = 0;

						if (FCMP(xs_clip, xs))
							xstart = (int)ceil(xs);
						else
							xstart = (int)xs_clip;

						if (FCMP(xe_clip, xe))
							xend = (int)ceil(xe) - 1;
						else
							xend = (int)xe_clip;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = color.value;
						}

						xs += dx_left;
						xe += dx_right;

						// test for loop_y hitting second region, if so change interpolant
						if (loop_y == yrestart)
						{
							float height_new = (y3 - ynew);

							if (type == TypeLHS)
							{
								dx_right = (x3 - x2) / height_new;

								xe = x2;

								xe += dx_right;
							}
							else
							{
								dx_left = (x3 - x2) / height_new;

								xs = x2;

								xs += dx_left;
							}
						}
					}
				}
			}
		}

		void Device::DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color0, const Color& color1, const Color& color2) const
		{
			if (FCMP(p0.x, p1.x) && FCMP(p1.x, p2.x) || FCMP(p0.y, p1.y) && FCMP(p1.y, p2.y))
				return;

			// Sort v0, v1, v2 in ascending y order
			Point v0 = p0;
			Point v1 = p1;
			Point v2 = p2;
			Color c0 = color0;
			Color c1 = color1;
			Color c2 = color2;

			if (v1.y < v0.y)
			{
				Swap(v1, v0);
				Swap(c1, c0);
			}

			if (v2.y < v0.y)
			{
				Swap(v0, v2);
				Swap(c0, c2);
			}

			if (v2.y < v1.y)
			{
				Swap(v2, v1);
				Swap(c2, c1);
			}

			// Cull
			if (v2.y < clipRect.top || v0.y > clipRect.bottom ||
				v0.x < clipRect.left && v1.x < clipRect.left && v2.x < clipRect.left ||
				v0.x > clipRect.right && v1.x > clipRect.right && v2.x > clipRect.right)
				return;

			enum TypeTriangle { TypeFlatTop, TypeFlatBottom, TypeGeneral };

			TypeTriangle type;
			if (FCMP(v0.y, v1.y))
				type = TypeFlatTop;
			else if (FCMP(v1.y, v2.y))
				type = TypeFlatBottom;
			else
				type = TypeGeneral;

			if (type == TypeFlatTop || type == TypeFlatBottom)
			{
				if (type == TypeFlatTop)
				{
					if (v1.x < v0.x)
					{
						Swap(v1, v0);
						Swap(c1, c0);
					}
				}
				else
				{
					if (v2.x < v1.x)
					{
						Swap(v2, v1);
						Swap(c2, c1);
					}
				}

				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				//float y2 = v1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				float r1 = c0.r;
				float g1 = c0.g;
				float b1 = c0.b;
				float r2 = c1.r;
				float g2 = c1.g;
				float b2 = c1.b;
				float r3 = c2.r;
				float g3 = c2.g;
				float b3 = c2.b;

				float height = y3 - y1;

				float dx_left;
				float dx_right;
				
				float di_r_left;
				float di_g_left;
				float di_b_left;
				float di_r_right;
				float di_g_right;
				float di_b_right;

				// starting points
				float xs;
				float xe;

				float is_r;
				float is_g;
				float is_b;
				float ie_r;
				float ie_g;
				float ie_b;

				if (type == TypeFlatTop)
				{
					dx_left = (x3 - x1) / height;
					dx_right = (x3 - x2) / height;

					di_r_left = (r3 - r1) / height;
					di_g_left = (g3 - g1) / height;
					di_b_left = (b3 - b1) / height;
					di_r_right = (r3 - r2) / height;
					di_g_right = (g3 - g2) / height;
					di_b_right = (b3 - b2) / height;

					xs = x1;
					xe = x2;

					is_r = r1;
					is_g = g1;
					is_b = b1;
					ie_r = r2;
					ie_g = g2;
					ie_b = b2;
				}
				else
				{
					dx_left = (x2 - x1) / height;
					dx_right = (x3 - x1) / height;

					di_r_left = (r2 - r1) / height;
					di_g_left = (g2 - g1) / height;
					di_b_left = (b2 - b1) / height;
					di_r_right = (r3 - r1) / height;
					di_g_right = (g3 - g1) / height;
					di_b_right = (b3 - b1) / height;

					xs = x1;
					xe = x1;

					is_r = r1;
					is_g = g1;
					is_b = b1;
					ie_r = r1;
					ie_g = g1;
					ie_b = b1;
				}

				int iy1, iy3;

				// check y1
				float dy = 0.0f;
				if (y1 < clipRect.top)
				{
					dy = clipRect.top - y1;

					// clip y1
					float y1_clip = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1_clip;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					dy = iy1 - y1;
				}
				// compute new xs and xe
				xs = xs + dx_left * dy;
				is_r = is_r + di_r_left * dy;
				is_g = is_g + di_g_left * dy;
				is_b = is_b + di_b_left * dy;

				xe = xe + dx_right * dy;
				ie_r = ie_r + di_r_right * dy;
				ie_g = ie_g + di_g_right * dy;
				ie_b = ie_b + di_b_right * dy;

				// check y3
				if (y3 > clipRect.bottom)
				{
					// clip y
					float y3_clip = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3_clip - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = (int)ceil(y3) - 1;
				}

				unsigned int pitchBits = pitch * bitsPerPixel;
				unsigned char* buffer = frameBuffer + iy1 * pitchBits;

				// check x
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// color step
						float di_r = 0.0f;
						float di_g = 0.0f;
						float di_b = 0.0f;

						float dx = xe - xs;
						if (dx > 0)
						{
							di_r = (ie_r - is_r) / dx;
							di_g = (ie_g - is_g) / dx;
							di_b = (ie_b - is_b) / dx;
						}
						else
						{
							di_r = ie_r - is_r;
							di_g = ie_g - is_g;
							di_b = ie_b - is_b;
						}

						// point start
						int xstart = (int)ceil(xs);
						int xend = (int)ceil(xe) - 1;

						// color start
						//float dx2 = xstart - xs;
						float istart_r = is_r;// +dx2 * di_r;
						float istart_g = is_g;// +dx2 * di_g;
						float istart_b = is_b;// +dx2 * di_b;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b).value;

							istart_r += di_r;
							istart_g += di_g;
							istart_b += di_b;
						}

						xs += dx_left;
						xe += dx_right;

						is_r += di_r_left;
						is_g += di_g_left;
						is_b += di_b_left;
						ie_r += di_r_right;
						ie_g += di_g_right;
						ie_b += di_b_right;
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
					{
						// clip test
						float xs_clip = xs;
						float xe_clip = xe;

						if (xs_clip < clipRect.left)
						{
							xs_clip = (float)clipRect.left;
							if (xe_clip < clipRect.left)
								continue;
						}

						if (xe_clip > clipRect.right)
						{
							xe_clip = (float)clipRect.right;
							if (xs_clip > clipRect.right)
								continue;
						}

						// color step
						float di_r = 0.0f;
						float di_g = 0.0f;
						float di_b = 0.0f;

						float dx = xe - xs;
						if (dx > 0)
						{
							di_r = (ie_r - is_r) / dx;
							di_g = (ie_g - is_g) / dx;
							di_b = (ie_b - is_b) / dx;
						}
						else
						{
							di_r = ie_r - is_r;
							di_g = ie_g - is_g;
							di_b = ie_b - is_b;
						}

						// point start
						int xstart = 0;
						int xend = 0;


						if (FCMP(xs_clip, xs))
							xstart = (int)ceil(xs);
						else
							xstart = (int)xs_clip;

						if (FCMP(xe_clip, xe))
							xend = (int)ceil(xe) - 1;
						else
							xend = (int)xe_clip;

						// color start
						//float dx2 = xstart - xs;
						float istart_r = is_r; //+ dx2 * di_r;
						float istart_g = is_g; //+ dx2 * di_g;
						float istart_b = is_b; //+ dx2 * di_b;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b).value;

							istart_r += di_r;
							istart_g += di_g;
							istart_b += di_b;
						}

						xs += dx_left;
						xe += dx_right;

						is_r += di_r_left;
						is_g += di_g_left;
						is_b += di_b_left;
						ie_r += di_r_right;
						ie_g += di_g_right;
						ie_b += di_b_right;
					}
				}
			}
			else
			{
				// new point
				float xnew = v0.x + (v2.x - v0.x) * (v1.y - v0.y) / (v2.y - v0.y);
				float ynew = v1.y;

				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				float y2 = v1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				float r1 = c0.r;
				float g1 = c0.g;
				float b1 = c0.b;
				float r2 = c1.r;
				float g2 = c1.g;
				float b2 = c1.b;
				float r3 = c2.r;
				float g3 = c2.g;
				float b3 = c2.b;

				enum TypeNewPoint { TypeLHS, TypeRHS };
				TypeNewPoint type = xnew > x2 ? TypeRHS : TypeLHS;

				float height_left;
				float height_right;
				float dx_left;
				float dx_right;;

				float di_r_left;
				float di_g_left;
				float di_b_left;
				float di_r_right;
				float di_g_right;
				float di_b_right;

				if (type == TypeRHS)
				{
					height_left = y2 - y1;
					height_right = y3 - y1;
					dx_left = (x2 - x1) / height_left;
					dx_right = (x3 - x1) / height_right;

					di_r_left = (r2 - r1) / height_left;
					di_g_left = (g2 - g1) / height_left;
					di_b_left = (b2 - b1) / height_left;
					di_r_right = (r3 - r1) / height_right;
					di_g_right = (g3 - g1) / height_right;
					di_b_right = (b3 - b1) / height_right;
				}
				else
				{
					height_left = y3 - y1;
					height_right = y2 - y1;
					dx_left = (x3 - x1) / height_left;
					dx_right = (x2 - x1) / height_right;

					di_r_left = (r3 - r1) / height_left;
					di_g_left = (g3 - g1) / height_left;
					di_b_left = (b3 - b1) / height_left;
					di_r_right = (r2 - r1) / height_right;
					di_g_right = (g2 - g1) / height_right;
					di_b_right = (b2 - b1) / height_right;
				}

				// starting points
				float xs = x1;
				float xe = x1;

				float is_r = r1;
				float is_g = g1;
				float is_b = b1;
				float ie_r = r1;
				float ie_g = g1;
				float ie_b = b1;

				int iy1, iy3;

				// check y1
				float dy = 0.0f;
				if (y1 < clipRect.top)
				{
					dy = clipRect.top - y1;

					float y1_clip = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1_clip;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					dy = iy1 - y1;
				}
				// compute new xs and ys
				xs = xs + dx_left * dy;
				is_r = is_r + di_r_left * dy;
				is_g = is_g + di_g_left * dy;
				is_b = is_b + di_b_left * dy;

				xe = xe + dx_right * dy;
				ie_r = ie_r + di_r_right * dy;
				ie_g = ie_g + di_g_right * dy;
				ie_b = ie_b + di_b_right * dy;

				// check y3
				if (y3 > clipRect.bottom)
				{
					// clip y
					float y3_clip = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3_clip - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = (int)ceil(y3) - 1;
				}

				// check yrestart
				int yrestart = int(ynew + 0.5f);
				if (yrestart < iy1)
					yrestart = iy1;

				unsigned int pitchBits = pitch * bitsPerPixel;
				unsigned char* buffer = frameBuffer + iy1 * pitchBits;

				// check x
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// color step
						float di_r = 0.0f;
						float di_g = 0.0f;
						float di_b = 0.0f;

						float dx = xe - xs;
						if (dx > 0)
						{
							di_r = (ie_r - is_r) / dx;
							di_g = (ie_g - is_g) / dx;
							di_b = (ie_b - is_b) / dx;
						}
						else
						{
							di_r = ie_r - is_r;
							di_g = ie_g - is_g;
							di_b = ie_b - is_b;
						}

						// point start
						int xstart = (int)ceil(xs);
						int xend = (int)ceil(xe) - 1;

						// color start
						//float dx2 = xstart - xs;
						float istart_r = is_r;// +dx2 * di_r;
						float istart_g = is_g;// +dx2 * di_g;
						float istart_b = is_b;// +dx2 * di_b;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b).value;

							istart_r += di_r;
							istart_g += di_g;
							istart_b += di_b;
						}

						xs += dx_left;
						xe += dx_right;

						is_r += di_r_left;
						is_g += di_g_left;
						is_b += di_b_left;
						ie_r += di_r_right;
						ie_g += di_g_right;
						ie_b += di_b_right;

						// test for loop_y hitting second region, if so change interpolant
						if (loop_y == yrestart)
						{
							float height_new = (y3 - ynew);

							if (type == TypeLHS)
							{
								dx_right = (x3 - x2) / height_new;

								di_r_right = (r3 - r2) / height_new;
								di_g_right = (g3 - g2) / height_new;
								di_b_right = (b3 - b2) / height_new;

								xe = x2;
								ie_r = r2;
								ie_g = g2;
								ie_b = b2;

								xe += dx_right;
								ie_r += di_r_right;
								ie_g += di_g_right;
								ie_b += di_b_right;
							}
							else
							{
								dx_left = (x3 - x2) / height_new;

								di_r_left = (r3 - r2) / height_new;
								di_g_left = (g3 - g2) / height_new;
								di_b_left = (b3 - b2) / height_new;

								xs = x2;
								is_r = r2;
								is_g = g2;
								is_b = b2;

								xs += dx_left;
								is_r += di_r_left;
								is_g += di_g_left;
								is_b += di_b_left;
							}
						}
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y, buffer += pitchBits)
					{
						// clip test
						float xs_clip = xs;
						float xe_clip = xe;

						if (xs_clip < clipRect.left)
						{
							xs_clip = (float)clipRect.left;
							if (xe_clip < clipRect.left)
								continue;
						}

						if (xe_clip > clipRect.right)
						{
							xe_clip = (float)clipRect.right;
							if (xs_clip > clipRect.right)
								continue;
						}

						// color step
						float di_r = 0.0f;
						float di_g = 0.0f;
						float di_b = 0.0f;

						float dx = xe - xs;
						if (dx > 0)
						{
							di_r = (ie_r - is_r) / dx;
							di_g = (ie_g - is_g) / dx;
							di_b = (ie_b - is_b) / dx;
						}
						else
						{
							di_r = ie_r - is_r;
							di_g = ie_g - is_g;
							di_b = ie_b - is_b;
						}

						// point start
						int xstart = 0;
						int xend = 0;

						if (FCMP(xs_clip, xs))
							xstart = (int)ceil(xs);
						else
							xstart = (int)xs_clip;

						if (FCMP(xe_clip, xe))
							xend = (int)ceil(xe) - 1;
						else
							xend = (int)xe_clip;

						// color start
						//float dx2 = xstart - xs;
						float istart_r = is_r;// +dx2 * di_r;
						float istart_g = is_g;// +dx2 * di_g;
						float istart_b = is_b;// +dx2 * di_b;

						unsigned char* p = buffer + xstart * bitsPerPixel;
						for (int loop_x = xstart; loop_x <= xend; ++loop_x, p += bitsPerPixel)
						{
							*((unsigned int*)p) = Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b).value;

							istart_r += di_r;
							istart_g += di_g;
							istart_b += di_b;
						}

						xs += dx_left;
						xe += dx_right;

						is_r += di_r_left;
						is_g += di_g_left;
						is_b += di_b_left;
						ie_r += di_r_right;
						ie_g += di_g_right;
						ie_b += di_b_right;

						// test for loop_y hitting second region, if so change interpolant
						if (loop_y == yrestart)
						{
							float height_new = (y3 - ynew);

							if (type == TypeLHS)
							{
								dx_right = (x3 - x2) / height_new;

								di_r_right = (r3 - r2) / height_new;
								di_g_right = (g3 - g2) / height_new;
								di_b_right = (b3 - b2) / height_new;

								xe = x2;
								ie_r = r2;
								ie_g = g2;
								ie_b = b2;

								xe += dx_right;
								ie_r += di_r_right;
								ie_g += di_g_right;
								ie_b += di_b_right;
							}
							else
							{
								dx_left = (x3 - x2) / height_new;

								di_r_left = (r3 - r2) / height_new;
								di_g_left = (g3 - g2) / height_new;
								di_b_left = (b3 - b2) / height_new;

								xs = x2;
								is_r = r2;
								is_g = g2;
								is_b = b2;

								xs += dx_left;
								is_r += di_r_left;
								is_g += di_g_left;
								is_b += di_b_left;
							}
						}
					}
				}
			}
		}
	}
}
