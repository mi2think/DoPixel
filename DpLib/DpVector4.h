/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:33
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpVector4.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpVector4
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Vector4
*********************************************************************/

#ifndef __DP_VECTOR4__
#define __DP_VECTOR4__

#include "DpMath.h"

namespace DoPixel
{
	namespace Math
	{
		template <typename T> class Vector4;
		typedef Vector4<float> Vector4f;

		template <typename T>
		class Vector4
		{
		public:
			T x, y, z, w;

			Vector4() : x(0), y(0), z(0), w(1){}
			Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
			Vector4(const Vector4& vec4) : x(vec4.x), y(vec4.y), z(vec4.z), w(vec4.w) {}
			Vector4& operator=(const Vector4& vec4) { x = vec4.x; y = vec4.y; z = vec4.z; w = vec4.w; return *this; }

			bool operator==(const Vector4& vec4) const { return Equal(vec4.x, x) && Equal(vec4.y, y) && Equal(vec4.z, z) && Equal(vec4.w, w); }
			bool operator!=(const Vector4& vec4) const { return ! operator==(vec4); }

			Vector4 operator-() const { return Vector4(-x, -y, -z, 1); }

			Vector4 operator+(const Vector4& vec4) const { return Vector4(x + vec4.x, y + vec4.y, z + vec4.z, 1); }
			Vector4 operator-(const Vector4& vec4) const { return Vector4(x - vec4.x, y - vec4.y, z - vec4.z, 1); }

			Vector4& operator+=(const Vector4& vec4) { x += vec4.x; y += vec4.y; z += vec4.z; return *this; }
			Vector4& operator-=(const Vector4& vec4) { x -= vec4.x; y -= vec4.y; z -= vec4.z; return *this; }	

			template <typename U>
			Vector4& operator*=(U k) { x *= k; y *= k; z *= k; return *this; }

			template <typename U>
			Vector4& operator/=(U k) { float f = 1.0f / k; x *= f; y *= f; z *= f; return *this; }

			void Zero() { x = y = z = T(0); w = 1; }
			void Normalize() 
			{
				float sq = x * x + y * y + z * z;
				if (sq > 0.0f)
				{
					float f = 1.0f / sqrt(sq);
					x *= f;
					y *= f;
					z *= f;
					w = 1;
				}
			}
			bool IsNormalized() const { return Equal(LengthSQ(), 1.0f); }
			float Length() const
			{
				return sqrt(x * x + y * y + z * z);
			}
			float LengthSQ() const
			{
				return x * x + y * y + z * z;
			}
			Vector4 Interpolate(const Vector4& v1, float t) const
			{
				return *this + (v1 - *this) * t;
			}
		};

		template <typename T, typename U>
		inline Vector4<T> operator*(const Vector4<T>& vec4, U k)
		{
			return Vector4<T>(vec4.x * k, vec4.y * k, vec4.z * k, 1);
		}

		template <typename T, typename U>
		inline Vector4<T> operator*(U k, const Vector4<T>& vec4)
		{
			return Vector4<T>(vec4.x * k, vec4.y * k, vec4.z * k, 1);
		}

		template <typename T, typename U>
		inline Vector4<T> operator/(const Vector4<T>& vec4, U k)
		{
			float f = 1.0f / k;
			return Vector4<T>(vec4.x * f, vec4.y * f, vec4.z *f, 1);
		}

		template <typename T>
		inline T DotProduct(const Vector4<T>& a, const Vector4<T>& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		template <typename T>
		inline Vector4<T> CrossProduct(const Vector4<T>& a, const Vector4<T>& b)
		{
			return Vector4<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, 1);
		}

		template <typename T>
		inline float DistanceSQ(const Vector4<T>& a, const Vector4<T>& b)
		{
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			float dz = a.z - b.z;
			return dx * dx + dy * dy + dz * dz;
		}

		template <typename T>
		inline float Distance(const Vector4<T>& a, const Vector4<T>& b)
		{
			return sqrt(DistanceSQ(a, b));
		}

		template <typename T>
		inline Vector4<T> Projection(const Vector4<T>& v, const Vector4<T>& n)
		{
			T dotp = DotProduct(v, n);
			T disSq = n.LengthSQ();
			return n * ((float)dotp / disSq);
		}

		template <typename T>
		inline float VectorAngle(const Vector4<T>& a, const Vector4<T>& b)
		{
			T dotp = DotProduct(a, b);
			float lenA = a.Length();
			float lenB = b.Length();
			return acos(dotp / (lenA * lenB));
		}

		template <typename OS, typename T>
		inline OS& operator << (OS& os, const Vector4<T>& vec4)
		{
			os << "(" << vec4.x << "," << vec4.y << "," << vec4.z << "," << vec4.w << ")";
			return os;
		}
	}
}



#endif
