/********************************************************************
	created:	2014/06/22
	created:	22:6:2014   20:23
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpGeometry.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpGeometry
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Geometry primitive
*********************************************************************/

#ifndef __DP_GEOMETRY__
#define __DP_GEOMETRY__

#include "DpVector2.h"
#include "DpVector3.h"

namespace dopixel
{
	namespace math
	{
		struct Line2D
		{
			// n = [a, b], p ， n = d
			// n must be unit Vector2

			Vector2f n;
			float d;

			Line2D(const Vector2f& _n, float _d) : n(_n), d(_d) {}

			Vector2f NearestPoint(const Vector2f& q)
			{
				Vector2f p;
				p = q + (d - DotProduct(n, q)) * n;
				return p;
			}
		};

		struct Plane3D
		{
			// For a plane, a collection of p: n ， (p - p0) = 0
			// Then set d = -n ， p0,  n ， p + d = 0 

			Vector3f n; // normal to the plane
			float d;
			
			Plane3D() {}
			Plane3D(const Vector3f& n, const Vector3f& p, bool bNormal = false)
			{
				this->n = n;
				if (! bNormal)
				{
					this->n.Normalize();
				}
				d = -DotProduct(n, p);
			}
			Plane3D(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2)
			{
				Vector3f u = p1 - p0;
				Vector3f v = p2 - p0;
				this->n = CrossProduct(u, v);
				n.Normalize();
				d = -DotProduct(n, p0);
			}

			// ax + by + cz + d
			float DotCoord(const Vector3f& p) const
			{
				return DotProduct(n, p) + d;
			}

			// ax + by + cz
			float DotNormal(const Vector3f& p) const
			{
				return DotProduct(n, p);
			}

			float GetDistance(const Vector3f& p) const
			{
				return DotCoord(p);
			}

			Vector3f GetNearestPoint(const Vector3f& p) const
			{
				// q = p + (-kn), k is the nearest distance from p to plane, and n is normalize, so
				// k = n ， p + d
				Vector3f q = p - DotCoord(p) * n;
				return q;
			}
		};

		template <typename T>
		class Rect
		{
		public:
			T left, top, right, bottom;

			Rect() {}
			Rect(T l, T t, T r, T b) : left(l), top(t), right(r), bottom(b) {}
			Rect(const Rect& rect) : left(rect.left), top(rect.top), right(rect.right), bottom(rect.bottom) {}

			bool operator==(const Rect& rect) const { return Equal(left, rect.left) && Equal(top, rect.top) && Equal(right, rect.right) && Equal(bottom, rect.bottom); }
			bool operator!=(const Rect& rect) const { return ! operator==(rect); }

			Rect& operator=(const Rect& rect)  { left = rect.left; top = rect.top; right = rect.right; bottom = rect.bottom; return *this; }
			Rect  operator&(const Rect& rect) const { Rect r; r.left = max(left, rect.left); r.top = max(top, rect.top); r.right = min(right, rect.right); r.bottom = min(bottom, rect.bottom); return r; }
			Rect& operator&=(const Rect& rect) { left = max(left, rect.left); top = max(top, rect.top); right = min(right, rect.right); bottom = min(bottom, rect.bottom); return *this; }
			Rect  operator|(const Rect& rect) const { Rect r; r.left = min(left, rect.left); r.top = min(top, rect.top); r.right = max(right, rect.right); r.bottom = max(bottom, rect.bottom); return r; }
			Rect& operator|=(const Rect& rect) { left = min(left, rect.left); top = min(top, rect.top); right = max(right, rect.right); bottom = max(bottom, rect.bottom); return *this; }

			T Width()  const { return right - left; }
			T Height() const { return bottom - top; }
			bool IsEmpty() const { return left >= right || top >= bottom; }
			bool PtInRect(const Point& pt) const { return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom; }
			void OffsetRect(T x, T y) { left += x; right += x; top += y; bottom += y; }
			void OffsetRect(const Vector2<T>& pt) { left += pt.x; right += pt.x; top += pt.y; bottom += pt.y; }
			void InflateRect(T l, T t, T r, T b) { left += l; right += r; top += t; bottom += b; }
		};

		typedef Rect<float>	RectF;
		typedef Rect<int>	RectI;
	}
}

#endif