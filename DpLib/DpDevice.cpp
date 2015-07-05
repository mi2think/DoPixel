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

			// Sort p0, p1, p2 in ascending y order
			Point _p0 = p0;
			Point _p1 = p1;
			Point _p2 = p2;

			if (_p1.y < _p0.y)
			{
				Point p = _p0;
				_p0 = _p1;
				_p1 = p;
			}

			if (_p2.y < _p0.y)
			{
				Point p = _p0;
				_p0 = _p2;
				_p2 = p;
			}

			if (_p2.y < _p1.y)
			{
				Point p = _p1;
				_p1 = _p2;
				_p2 = p;
			}

			// Cull
			if (_p2.y < clipRect.top || _p0.y > clipRect.bottom)
				return;
			if (_p0.x < clipRect.left && _p1.x < clipRect.left && _p2.x < clipRect.left)
				return;
			if (_p0.x > clipRect.right && _p1.x > clipRect.right && _p2.x > clipRect.right)
				return;

			if (FCMP(_p0.y, _p1.y))
			{
				fnDrawFlatTop(_p0, _p1, _p2);
			}
			else if (FCMP(_p1.y, _p2.y))
			{
				fnDrawFlatBottom(_p0, _p1, _p2);
			}
			else
			{
				// Note: p0, p1, p2 have sorted
				// (xnew - x1) / (x3 - x1) = (y2 - y1) / (y3 - y1)

				float xnew = _p0.x + (_p2.x - _p0.x) * (_p1.y - _p0.y) / (_p2.y - _p0.y);

				fnDrawFlatBottom(_p0, Point(xnew, _p1.y), _p1);
				fnDrawFlatTop(_p1, Point(xnew, _p1.y), _p2);
			}
		}

		void Device::DrawTriangle(const Point& p0, const Point& p1, const Point& p2, const Color& color0, const Color& color1, const Color& color2) const
		{
			// TODO: draw triangle with gouraud shading
		}
	}
}
