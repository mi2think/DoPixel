/********************************************************************
	created:	2014/06/22
	created:	22:6:2014   20:29
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpVector2.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpVector2
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Vector2
*********************************************************************/
#ifndef __DP_VECTOR2__
#define __DP_VECTOR2__

#include "DpMath.h"

namespace DoPixel
{
	namespace Math
	{
		template <typename T> class Vector2;
		typedef Vector2<float> Vector2f;
		typedef Vector2<float> Point;
		typedef Vector2<int>   Vector2i;

		template <typename T>
		class Vector2
		{
		public:
			T x, y;

			Vector2() : x(0), y(0){}
			Vector2(T _x, T _y) : x(_x), y(_y) {}
			Vector2(const Vector2& vec2) : x(vec2.x), y(vec2.y){}
			Vector2& operator=(const Vector2& vec2) { x = vec2.x; y = vec2.y; return *this; }

			bool operator==(const Vector2& vec2) const { return Equal(vec2.x, x) && Equal(vec2.y, y); }
			bool operator!=(const Vector2& vec2) const { return ! operator==(vec2); }

			Vector2 operator-() const { return Vector2(-x, -y); }

			Vector2 operator+(const Vector2& vec2) const { return Vector2(x + vec2.x, y + vec2.y); }
			Vector2 operator-(const Vector2& vec2) const { return Vector2(x - vec2.x, y - vec2.y); }

			Vector2& operator+=(const Vector2& vec2) { x += vec2.x; y += vec2.y; return *this; }
			Vector2& operator-=(const Vector2& vec2) { x -= vec2.x; y -= vec2.y; return *this; }

			template <typename U>
			Vector2& operator*=(U k) { x *= k; y *= k; return *this; }

			template <typename U>
			Vector2& operator/=(U k) { float f = 1.0f / k; x *= f; y *= f; return *this; }

			void Zero() { x = y = T(0); }
			void Normalize() 
			{
				float sq = x * x + y * y;
				if (sq > 0.0f)
				{
					float f = 1.0f / sqrt(sq);
					x *= f;
					y *= f;
				}
			}
			bool IsNormalized() const { return Equal(LengthSQ(), 1.0f); }
			float Length() const
			{
				return sqrt(x * x + y * y);
			}
			float LengthSQ() const
			{
				return x * x + y * y;
			}
			Vector2 Interpolate(const Vector2& v1, float t) const
			{
				return *this + (v1 - *this) * t;
			}
		};

		template <typename T, typename U>
		inline Vector2<T> operator*(const Vector2<T>& vec2, U k)
		{
			return Vector2<T>(vec2.x * k, vec2.y * k);
		}

		template <typename T, typename U>
		inline Vector2<T> operator*(U k, const Vector2<T>& vec2)
		{
			return Vector2<T>(vec2.x * k, vec2.y * k);
		}

		template <typename T, typename U>
		inline Vector2<T> operator/(const Vector2<T>& vec2, U k)
		{
			return Vector2<T>(vec2.x / k, vec2.y / k);
		}

		template <typename T>
		inline T DotProduct(const Vector2<T>& a, const Vector2<T>& b)
		{
			return a.x * b.x + a.y * b.y;
		}

		template <typename T>
		inline float DistanceSQ(const Vector2<T>& a, const Vector2<T>& b)
		{
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			return dx * dx + dy * dy;
		}

		template <typename T>
		inline float Distance(const Vector2<T>& a, const Vector2<T>& b)
		{
			return sqrt(DistanceSQ(a, b));
		}

		template <typename T>
		inline Vector2<T> Projection(const Vector2<T>& v, const Vector2<T>& n)
		{
			T dotp = DotProduct(v, n);
			T disSq = n.LengthSQ();
			return n * ((float)dotp / disSq);
		}

		template <typename T>
		inline float VectorAngle(const Vector2<T>& a, const Vector2<T>& b)
		{
			T dotp = DotProduct(a, b);
			float lenA = a.Length();
			float lenB = b.Length();
			return acos(dotp / (lenA * lenB));
		}

		template <typename OS, typename T>
		inline OS& operator << (OS& os, const Vector2<T>& vec2)
		{
			os << "(" << vec2.x << "," << vec2.y << ")";
			return os;
		}
	}
}

#endif
