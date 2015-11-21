/********************************************************************
	created:	2015/11/21
	created:	21:11:2015   17:07
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRectangle.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRectangle
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Rectangle
*********************************************************************/
#ifndef __DP_RECTANGLE_H__
#define __DP_RECTANGLE_H__

#include "DpMath.h"

namespace dopixel
{
	namespace math
	{
		template <typename T>
		class Rectangle
		{
		public:
			T left, top, right, bottom;

			Rectangle() {}
			Rectangle(T l, T t, T r, T b)
				: left(l)
				, top(t)
				, right(r)
				, bottom(b) 
			{}
			Rectangle(const Rectangle& rect)
				: left(rect.left)
				, top(rect.top)
				, right(rect.right)
				, bottom(rect.bottom)
			{}

			bool operator==(const Rectangle& rect) const
			{ 
				return Equal(left, rect.left) && Equal(top, rect.top) && Equal(right, rect.right) && Equal(bottom, rect.bottom);
			}

			bool operator!=(const Rectangle& rect) const 
			{
				return !operator==(rect);
			}

			Rectangle& operator=(const Rectangle& rect)
			{
				left = rect.left;
				top = rect.top;
				right = rect.right;
				bottom = rect.bottom;
				return *this;
			}
			
			Rectangle& operator&=(const Rectangle& rect)
			{
				left = MAX(left, rect.left);
				top = MAX(top, rect.top);
				right = MIN(right, rect.right);
				bottom = MIN(bottom, rect.bottom);
				return *this;
			}

			Rectangle operator&(const Rectangle& rect) const
			{
				Rectangle r = *this;
				r &= rect;
				return r;
			}

			Rectangle& operator|=(const Rectangle& rect)
			{
				left = MIN(left, rect.left);
				top = MIN(top, rect.top);
				right = MAX(right, rect.right);
				bottom = MAX(bottom, rect.bottom);
				return *this;
			}

			Rectangle operator|(const Rectangle& rect) const
			{
				Rectangle r = *this;
				r |= rect;
				return r;
			}

			T Width()  const { return right - left; }		
			T Height() const { return bottom - top; }
			bool Valid() const { return right > left && bottom > top; }
			bool Contains(const Vector2<T>& pt) const
			{
				return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
			}

			void OffsetRect(T x, T y)
			{
				left += x;
				right += x;
				top += y;
				bottom += y;
			}
			void OffsetRect(const Vector2<T>& pt)
			{
				OffsetRect(pt.x, pt.y);
			}

			void InflateRect(T l, T t, T r, T b)
			{
				left += l;
				right += r;
				top += t;
				bottom += b;
			}
		};

		typedef Rectangle<float> Rectf;
		typedef Rectangle<int> Recti;
	}
}

#endif