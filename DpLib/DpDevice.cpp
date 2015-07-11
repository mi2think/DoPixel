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
			auto fnDrawFlatTop = [&color, this](const Point& p0, const Point& p1, const Point& p2) -> void
			{
				//float kl = (p2.x - p0.x) / (p2.y - p0.y);
				//float kr = (p2.x - p1.x) / (p2.y - p1.y);
				//float xs = p0.x;
				//float xe = p1.x;

				//for (float y = p0.y; y <= p2.y; ++y)
				//{
				//	DrawLine(Point(xs, y), Point(xe, y), color);
				//	xs += kl;
				//	xe += kr;
				//}

				float x1 = p0.x;
				float y1 = p0.y;
				float x2 = p1.x;
				//float y2 = p1.y;
				float x3 = p2.x;
				float y3 = p2.y;
				
				if (x1 > x2)
				{
					float x_temp = x1;
					x1 = x2;
					x2 = x_temp;
				}

				float height = y3 - y1;
				float dx_left = (x3 - x1) / height;
				float dx_right = (x3 - x2) / height;

				// starting points
				float xs = x1;
				float xe = x2;

				int iy1, iy3;

				if (y1 < clipRect.top)
				{
					// compute new xs and ys
					xs = xs + dx_left * (clipRect.top - y1);
					xe = xe + dx_right * (clipRect.top - y1);

					// reset y1
					y1 = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					// bump xs and xe appropriately
					xs = xs + dx_left * (iy1 - y1);
					xe = xe + dx_right * (iy1 - y1);
				}

				if (y3 > clipRect.bottom)
				{
					// clip y
					y3 = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3 - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = int(ceil(y3) - 1);
				}

				// test if x clipping is needed
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					//draw the triangle
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
					{
						for (int loop_x = (int)xs; loop_x <= (int)xe; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, color);
						}

						xs += dx_left;
						xe += dx_right;
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
					{
						float left = xs;
						float right = xe;

						xs += dx_left;
						xe += dx_right;

						// clip line
						if (left < clipRect.left)
						{
							left = (float)clipRect.left;
							if (right < clipRect.left)
								continue;
						}

						if (right > clipRect.right)
						{
							right = (float)clipRect.right;
							if (left > clipRect.right)
								continue;
						}

						for (int loop_x = (int)left; loop_x <= (int)right; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, color);
						}
					}
				}
			};

			auto fnDrawFlatBottom = [&color, this](const Point& p0, const Point& p1, const Point& p2) -> void
			{
				//float kl = (p2.x - p0.x) / (p2.y - p0.y);
				//float kr = (p1.x - p0.x) / (p1.y - p0.y);
				//float xs = p0.x;
				//float xe = p0.x;

				//for (float y = p0.y; y <= p2.y; ++y)
				//{
				//	DrawLine(Point(xs, y), Point(xe, y), color);
				//	xs += kl;
				//	xe += kr;
				//}

				float x1 = p0.x;
				float y1 = p0.y;
				float x2 = p1.x;
				//float y2 = p1.y;
				float x3 = p2.x;
				float y3 = p2.y;

				if (x3 > x2)
				{
					float x_temp = x3;
					x3 = x2;
					x2 = x_temp;
				}

				float height = y3 - y1;
				float dx_left = (x3 - x1) / height;
				float dx_right = (x2 - x1) / height;

				// starting points
				float xs = x1;
				float xe = x1;

				int iy1, iy3;

				if (y1 < clipRect.top)
				{
					// compute new xs and ys
					xs = xs + dx_left * (clipRect.top - y1);
					xe = xe + dx_right * (clipRect.top - y1);

					// reset y1
					y1 = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1;
				}
				else
				{
					//make sure top left fill convention is observed
					iy1 = (int)ceil(y1);

					// bump xs and xe appropriately
					xs = xs + dx_left * (iy1 - y1);
					xe = xe + dx_right * (iy1 - y1);
				}

				if (y3 > clipRect.bottom)
				{
					// clip y
					y3 = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3 - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = int(ceil(y3) - 1);
				}

				// test if x clipping is needed
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					//draw the triangle
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
					{
						for (int loop_x = (int)xs; loop_x <= (int)xe; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, color);
						}

						xs += dx_left;
						xe += dx_right;
					}
				}
				else
				{
					// clip x
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
					{
						float left = xs;
						float right = xe;
						
						xs += dx_left;
						xe += dx_right;

						// clip line
						if (left < clipRect.left)
						{
							left = (float)clipRect.left;
							if (right < clipRect.left)
								continue;
						}

						if (right > clipRect.right)
						{
							right = (float)clipRect.right;
							if (left > clipRect.right)
								continue;
						}
						
						for (int loop_x = (int)left; loop_x <= (int)right; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, color);
						}
					}
				}
			};

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

			if (FCMP(v0.y, v1.y))
			{
				fnDrawFlatTop(v0, v1, v2);
			}
			else if (FCMP(v1.y, v2.y))
			{
				fnDrawFlatBottom(v0, v1, v2);
			}
			else
			{
				// Note: v0, v1, v2 have sorted
				// (xnew - x1) / (x3 - x1) = (y2 - y1) / (y3 - y1)

				float xnew = v0.x + (v2.x - v0.x) * (v1.y - v0.y) / (v2.y - v0.y);

				fnDrawFlatBottom(v0, Point(xnew, v1.y), v1);
				fnDrawFlatTop(v1, Point(xnew, v1.y), v2);
			}
		}

		void Device::DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color0, const Color& color1, const Color& color2) const
		{
			if (FCMP(p0.x, p1.x) && FCMP(p1.x, p2.x) || FCMP(p0.y, p1.y) && FCMP(p1.y, p2.y))
				return;

			auto fnDrawFlatTop = [this](const Point& v0, const Point& v1, const Point& v2, const Color& color0, const Color& color1, const Color& color2)
			{
				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				//float y2 = v1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				float r1 = color0.r;
				float g1 = color0.g;
				float b1 = color0.b;
				float r2 = color1.r;
				float g2 = color1.g;
				float b2 = color1.b;
				float r3 = color2.r;
				float g3 = color2.g;
				float b3 = color2.b;

				float height = y3 - y1;
				float dx_left = (x3 - x1) / height;
				float dx_right = (x3 - x2) / height;
				
				float di_r_left  = (r3 - r1) / height;
				float di_g_left  = (g3 - g1) / height;
				float di_b_left  = (b3 - b1) / height;
				float di_r_right = (r3 - r2) / height;
				float di_g_right = (g3 - g2) / height;
				float di_b_right = (b3 - b2) / height;

				// starting points
				float xs = x1;
				float xe = x2;

				float is_r = r1;
				float is_g = g1;
				float is_b = b1;
				float ie_r = r2;
				float ie_g = g2;
				float ie_b = b2;

				int iy1, iy3;

				// check y1
				float dy = 0.0f;
				if (y1 < clipRect.top)
				{
					dy = clipRect.top - y1;

					// reset y1
					y1 = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1;
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
					y3 = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3 - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = int(ceil(y3) - 1);
				}

				// check x
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
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
						int xend = int(ceil(xe) - 1);

						// color start
						float dx2 = xstart - xs;
						float istart_r = is_r + dx2 * di_r;
						float istart_g = is_g + dx2 * di_g;
						float istart_b = is_b + dx2 * di_b;

						for (int loop_x = xstart; loop_x <= xend; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b));
							
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
						float dx_clip = 0.0f;

						if (xs_clip < clipRect.left)
						{
							dx_clip = clipRect.left - xs_clip;
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
							xend = int(ceil(xe) - 1);
						else
							xend = (int)xe_clip;

						// color start
						float dx2 = xstart - xs;
						float istart_r = is_r + dx2 * di_r;
						float istart_g = is_g + dx2 * di_g;
						float istart_b = is_b + dx2 * di_b;

						for (int loop_x = xstart; loop_x <= xend; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b));

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
			};

			auto fnDrawFlatBottom = [this](const Point& v0, const Point& v1, const Point& v2, const Color& color0, const Color& color1, const Color& color2)
			{
				float x1 = v0.x;
				float y1 = v0.y;
				float x2 = v1.x;
				//float y2 = v1.y;
				float x3 = v2.x;
				float y3 = v2.y;

				float r1 = color0.r;
				float g1 = color0.g;
				float b1 = color0.b;
				float r2 = color1.r;
				float g2 = color1.g;
				float b2 = color1.b;
				float r3 = color2.r;
				float g3 = color2.g;
				float b3 = color2.b;

				float height = y3 - y1;
				float dx_left = (x2 - x1) / height;
				float dx_right = (x3 - x1) / height;

				float di_r_left  = (r2 - r1) / height;
				float di_g_left  = (g2 - g1) / height;
				float di_b_left  = (b2 - b1) / height;
				float di_r_right = (r3 - r1) / height;
				float di_g_right = (g3 - g1) / height;
				float di_b_right = (b3 - b1) / height;

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

					// reset y1
					y1 = (float)clipRect.top;

					//make sure top left fill convention is observed
					iy1 = (int)y1;
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
					y3 = (float)clipRect.bottom;

					// make sure top left fill convention is observed
					iy3 = int(y3 - 1);
				}
				else
				{
					// make sure top left fill convention is observed
					iy3 = int(ceil(y3) - 1);
				}

				// check x
				if (x1 >= clipRect.left && x1 <= clipRect.right &&
					x2 >= clipRect.left && x2 <= clipRect.right &&
					x3 >= clipRect.left && x3 <= clipRect.right)
				{
					for (int loop_y = iy1; loop_y <= iy3; ++loop_y)
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
						int xend = int(ceil(xe) - 1);

						// color start
						float dx2 = xstart - xs;
						float istart_r = is_r + dx2 * di_r;
						float istart_g = is_g + dx2 * di_g;
						float istart_b = is_b + dx2 * di_b;

						for (int loop_x = xstart; loop_x <= xend; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b));

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
						float dx_clip = 0.0f;

						if (xs_clip < clipRect.left)
						{
							dx_clip = clipRect.left - xs_clip;
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
							xend = int(ceil(xe) - 1);
						else
							xend = (int)xe_clip;

						// color start
						float dx2 = xstart - xs;
						float istart_r = is_r + dx2 * di_r;
						float istart_g = is_g + dx2 * di_g;
						float istart_b = is_b + dx2 * di_b;

						for (int loop_x = xstart; loop_x <= xend; ++loop_x)
						{
							this->WritePixel(loop_x, loop_y, Color((unsigned char)istart_r, (unsigned char)istart_g, (unsigned char)istart_b));

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
			};

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

			if (FCMP(v0.y, v1.y))
			{
				if (v1.x < v0.x)
				{
					Swap(v1, v0);
					Swap(c1, c0);
				}

				fnDrawFlatTop(v0, v1, v2, c0, c1, c2);
			}
			else if (FCMP(v1.y, v2.y))
			{
				if (v2.x < v1.x)
				{
					Swap(v2, v1);
					Swap(c2, c1);
				}

				fnDrawFlatBottom(v0, v1, v2, c0, c1, c2);
			}
			else
			{
			//	fnDrawGeneral(v0, v1, v2);
			}
		}
	}
}
