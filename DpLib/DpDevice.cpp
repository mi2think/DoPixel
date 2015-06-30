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
			auto fnDrawFlatTop = [&](const Point& p0, const Point& p1, const Point& p2) -> void
			{
				float kl = (p2.x - p0.x) / (p2.y - p0.y);
				float kr = (p2.x - p1.x) / (p2.y - p1.y);
				float xs = p0.x;
				float xe = p1.x;

				for (float y = p0.y; y <= p2.y; ++y)
				{
					DrawLine(Point(xs, y), Point(xe, y), color);
					xs += kl;
					xe += kr;
				}
			};

			auto fnDrawFlatBottom = [&](const Point& p0, const Point& p1, const Point& p2) -> void
			{
				float kl = (p2.x - p0.x) / (p2.y - p0.y);
				float kr = (p1.x - p0.x) / (p1.y - p0.y);
				float xs = p0.x;
				float xe = p0.x;

				for (float y = p0.y; y <= p2.y; ++y)
				{
					DrawLine(Point(xs, y), Point(xe, y), color);
					xs += kl;
					xe += kr;
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

			int x0 = Round(_p0.x);
			int x1 = Round(_p1.x);
			int x2 = Round(_p2.x);
			int y0 = Round(_p0.y);
			int y1 = Round(_p1.y);
			int y2 = Round(_p2.y);

			if (x0 == x1 && x1 == x2 || y0 == y1 && y1 == y2)
				return;

			// Cull
			if (_p2.y < clipRect.top || _p0.y > clipRect.bottom)
				return;
			if (_p0.x < clipRect.left && _p1.x < clipRect.left && _p2.x < clipRect.left)
				return;
			if (_p0.x > clipRect.right && _p1.x > clipRect.right && _p2.x > clipRect.right)
				return;

			if (y0 == y1)
			{
				fnDrawFlatTop(_p0, _p1, _p2);
			}
			else if (y1 == y2)
			{
				fnDrawFlatBottom(_p0, _p1, _p2);
			}
			else
			{
				float xnew = _p2.x + (_p0.x - _p2.x) * (_p2.y - _p1.y) / (_p2.y - _p0.y);

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
