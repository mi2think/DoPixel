/********************************************************************
	created:	2014/06/27
	created:	27:6:2014   13:51
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMatrix44.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMatrix44
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	4 x 4 Matrix, Row Major!
*********************************************************************/

#ifndef __DP_MATRIX44__
#define __DP_MATRIX44__

#include "DoPixel.h"
#include "DpMath.h"
#include "DpVector3.h"
#include "DpVector4.h"
#include "DpMatrix33.h"

namespace dopixel
{
	namespace math
	{
		template <typename T> class Matrix44;
		typedef Matrix44<float> Matrix44f;

		template <typename T>
		class Matrix44
		{
		public:
			enum { R = 4, C = 4, N = R * C };

			union
			{
				struct
				{
					T m11, m12, m13, m14;
					T m21, m22, m23, m24;
					T m31, m32, m33, m34;
					T m41, m42, m43, m44;
				};
				T m[R][C];
			};

			Matrix44() {}
			Matrix44(const T* _m) { memcpy(m, _m, sizeof(T) * N); }
			Matrix44(const Matrix44& _m) { memcpy(m, _m.m, sizeof(T) * N); }
			Matrix44(T _m11, T _m12, T _m13, T _m14,
				     T _m21, T _m22, T _m23, T _m24,
					 T _m31, T _m32, T _m33, T _m34,
					 T _m41, T _m42, T _m43, T _m44)
			: m11(_m11), m12(_m12), m13(_m13), m14(_m14)
			, m21(_m21), m22(_m22), m23(_m23), m24(_m24)
			, m31(_m31), m32(_m32), m33(_m33), m34(_m34)
			, m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}

			Matrix44& operator=(const Matrix44& _m)
			{
				memcpy(m, _m.m, sizeof(T) * N);
				return *this;
			}

			bool operator==(const Matrix44& _m) const
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						if (! Equal(m[i][j], _m.m[i][j]))
							return false;
					}
				}
				return true;
			}
			bool operator!=(const Matrix44& _m) const { return ! operator==(_m); }

			Matrix44& operator+=(const Matrix44& _m)
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						m[i][j] += _m.m[i][j];
					}
				}
				return *this;
			}

			Matrix44 operator+(const Matrix44& _m) const
			{
				Matrix44 n = *this;
				n += _m;
				return n;
			}

			Matrix44& operator-=(const Matrix44& _m)
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						m[i][j] -= _m.m[i][j];
					}
				}
				return *this;
			}

			Matrix44 operator-(const Matrix44& _m) const
			{
				Matrix44 n = *this;
				n -= _m;
				return n;
			}

			template <typename U>
			Matrix44& operator*=(U k)
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						m[i][j] *= k;
					}
				}
				return *this;
			}
			template <typename U>
			Matrix44 operator*(U k) const
			{
				Matrix44 n = *this;
				n *= k;
				return n;
			}

			template <typename U>
			Matrix44& operator/=(U k)
			{
				float f = 1.0f / k;
				return operator*=(f);
			}
			template <typename U>
			Matrix44 operator/(U k) const
			{
				Matrix44 n = *this;
				n /= k;
				return n;
			}

			Matrix44 operator-() const
			{
				Matrix44 n;
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						n.m[i][j] = -m[i][j];
					}
				}
				return n;
			}

			Matrix44 operator*(const Matrix44& _m) const
			{
				Matrix44 n;

				n.m11 = m11 * _m.m11 + m12 * _m.m21 + m13 * _m.m31 + m14 * _m.m41;
				n.m12 = m11 * _m.m12 + m12 * _m.m22 + m13 * _m.m32 + m14 * _m.m42;
				n.m13 = m11 * _m.m13 + m12 * _m.m23 + m13 * _m.m33 + m14 * _m.m43;
				n.m14 = m11 * _m.m14 + m12 * _m.m24 + m13 * _m.m34 + m14 * _m.m44;

				n.m21 = m21 * _m.m11 + m22 * _m.m21 + m23 * _m.m31 + m24 * _m.m41;
				n.m22 = m21 * _m.m12 + m22 * _m.m22 + m23 * _m.m32 + m24 * _m.m42;
				n.m23 = m21 * _m.m13 + m22 * _m.m23 + m23 * _m.m33 + m24 * _m.m43;
				n.m24 = m21 * _m.m14 + m22 * _m.m24 + m23 * _m.m34 + m24 * _m.m44;

				n.m31 = m31 * _m.m11 + m32 * _m.m21 + m33 * _m.m31 + m34 * _m.m41;
				n.m32 = m31 * _m.m12 + m32 * _m.m22 + m33 * _m.m32 + m34 * _m.m42;
				n.m33 = m31 * _m.m13 + m32 * _m.m23 + m33 * _m.m33 + m34 * _m.m43;
				n.m34 = m31 * _m.m14 + m32 * _m.m24 + m33 * _m.m34 + m34 * _m.m44;

				n.m41 = m41 * _m.m11 + m42 * _m.m21 + m43 * _m.m31 + m44 * _m.m41;
				n.m42 = m41 * _m.m12 + m42 * _m.m22 + m43 * _m.m32 + m44 * _m.m42;
				n.m43 = m41 * _m.m13 + m42 * _m.m23 + m43 * _m.m33 + m44 * _m.m43;
				n.m44 = m41 * _m.m14 + m42 * _m.m24 + m43 * _m.m34 + m44 * _m.m44;

				return n;
			}

			Matrix44 operator*=(const Matrix44& _m)
			{
				*this = *this * _m;
				return *this;
			}

			void Identity()
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						i == j ? m[i][j] = 1 : m[i][j] = 0;
					}
				}
			}

			// Translation methods
			void ZeroTranslation()
			{
				m41 = m42 = m43 = 0;
			}

			// Transpose
			void Transpose()
			{
				for (int i = 0; i < R; ++i)
				{
					for (int j = i; j < C; ++j)
					{
						if (i != j)
						{
							swap_t(m[i][j], m[j][i]);
						}
					}
				}
			}
		};

		template <typename T, typename U>
		inline Matrix44<T> operator*(U k, const Matrix44<T>& _m)
		{
			Matrix44<T> n = _m;
			n *= k;
			return n;
		}

		// Matrix n and _m must not be same one
		template <typename T>
		inline Matrix44<T>& MatrixTranspose(Matrix44<T>& n, const Matrix44<T>& _m)
		{
			ASSERT(&n != &_m);

			for (int i = 0; i < Matrix44<T>::R; ++i)
			{
				for (int j = 0; j < Matrix44<T>::C; ++j)
				{
					n.m[i][j] = _m.m[j][i];
				}
			}

			return n;
		}

		// algebraic cofactor
		template <typename T>
		inline Matrix44<T>& MatrixAlgebraicCofactor(Matrix44<T>& n, const Matrix44<T>& _m)
		{
			T t[16];
			int index = 0;
			bool negative = false;
			for (int i = 0; i < Matrix44<T>::R; ++i)
			{
				for (int j = 0; j < Matrix44<T>::C; ++j)
				{
					T t2[9];
					int index2 = 0;
					for (int i2 = 0; i2 < Matrix44<T>::R; ++i2)
					{
						for (int j2 = 0; j2 < Matrix44<T>::C; ++j2)
						{
							if (i2 == i || j2 == j)
								continue;
							t2[index2++] = _m.m[i2][j2];
						}
					}
					
					t[index] = MatrixDeterminant(Matrix33<T>(t2));
					if (negative)
					{
						t[index] = -t[index];
					}
					negative = !negative;
					++index;
				}
				negative = !negative;
			}
			n = Matrix44<T>(t);
			return n;
		}

		// standard adjugate
		template <typename T>
		inline Matrix44<T>& MatrixStandardAdjugate(Matrix44<T>& n, const Matrix44<T>& _m)
		{
			Matrix44<T> t;
			MatrixAlgebraicCofactor(t, _m);
			MatrixTranspose(n, t);
			return n;
		}

		template <typename T>
		inline float MatrixDeterminant(const Matrix44<T>& _m)
		{
			Matrix44<T> t;
			MatrixAlgebraicCofactor(t, _m);
			return _m.m11 * t.m11 + _m.m12 * t.m12 + _m.m13 * t.m13 + _m.m14 * t.m14;
		}

		template <typename T>
		inline Matrix44<T>& MatrixInverse(Matrix44<T>& n, const Matrix44<T>& _m)
		{
			MatrixStandardAdjugate(n, _m);
			float det = _m.m11 * n.m11 + _m.m12 * n.m21 + _m.m13 * n.m31 + _m.m14 * n.m41;
			n /= det;
			return n;
		}

		template <typename T>
		inline Matrix44<T>& MatrixMultiply(Matrix44<T>& n, const Matrix44<T>& m1, const Matrix44<T>& m2)
		{ 
			n.m11 = m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31 + m1.m14 * m2.m41;
			n.m12 = m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32 + m1.m14 * m2.m42;
			n.m13 = m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33 + m1.m14 * m2.m43;
			n.m14 = m1.m11 * m2.m14 + m1.m12 * m2.m24 + m1.m13 * m2.m34 + m1.m14 * m2.m44;

			n.m21 = m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31 + m1.m24 * m2.m41;
			n.m22 = m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32 + m1.m24 * m2.m42;
			n.m23 = m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33 + m1.m24 * m2.m43;
			n.m24 = m1.m21 * m2.m14 + m1.m22 * m2.m24 + m1.m23 * m2.m34 + m1.m24 * m2.m44;

			n.m31 = m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31 + m1.m34 * m2.m41;
			n.m32 = m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32 + m1.m34 * m2.m42;
			n.m33 = m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33 + m1.m34 * m2.m43;
			n.m34 = m1.m31 * m2.m14 + m1.m32 * m2.m24 + m1.m33 * m2.m34 + m1.m34 * m2.m44;

			n.m41 = m1.m41 * m2.m11 + m1.m42 * m2.m21 + m1.m43 * m2.m31 + m1.m44 * m2.m41;
			n.m42 = m1.m41 * m2.m12 + m1.m42 * m2.m22 + m1.m43 * m2.m32 + m1.m44 * m2.m42;
			n.m43 = m1.m41 * m2.m13 + m1.m42 * m2.m23 + m1.m43 * m2.m33 + m1.m44 * m2.m43;
			n.m44 = m1.m41 * m2.m14 + m1.m42 * m2.m24 + m1.m43 * m2.m34 + m1.m44 * m2.m44;

			return n;
		}

		// Translation

		template <typename T>
		inline Matrix44<T>& MaxtrixTranslation(Matrix44<T>& n, const Vector3<T>& v)
		{
			for (int i = 0; i < Matrix44<T>::R; ++i)
			{
				for (int j = 0; j < Matrix44<T>::C; ++j)
				{
					i == j ? n.m[i][j] = 1 : n.m[i][j] = 0;
				}
			}

			n.m41 = v.x;
			n.m42 = v.y;
			n.m43 = v.z;

			return n;
		}

		// Scale

		// scale by coordinate axis
		template <typename T>
		inline Matrix44<T>& MatrixScaling(Matrix44<T>& n, const Vector3<T>& v)
		{
			n.m11 = v.x;  n.m12 = 0.0f; n.m13 = 0.0f; n.m14 = 0.0f;
			n.m21 = 0.0f; n.m22 = v.y;  n.m23 = 0.0f; n.m24 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f; n.m33 = v.z;  n.m34 = 0.0f;
			n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;
			return n;
		}

		// Rotate

		template <typename T>
		inline Matrix44<T>& MatrixRotationX(Matrix44<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.m11 = 1.0f; n.m12 = 0.0f; n.m13 = 0.0f; n.m14 = 0.0f;
			n.m21 = 0.0f; n.m22 = c;	n.m23 = s;	  n.m24 = 0.0f;
			n.m31 = 0.0f; n.m32 = -s;	n.m33 = c;	  n.m34 = 0.0f;
			n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

			return n;
		}

		template <typename T>
		inline Matrix44<T>& MatrixRotationY(Matrix44<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.m11 = c;    n.m12 = 0.0f; n.m13 = -s;   n.m14 = 0.0f;
			n.m21 = 0.0f; n.m22 = 1.0f;	n.m23 = 0;	  n.m24 = 0.0f;
			n.m31 = s;	  n.m32 = 0.0f;	n.m33 = c;	  n.m34 = 0.0f;
			n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

			return n;
		}

		template <typename T>
		inline Matrix44<T>& MatrixRotationZ(Matrix44<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.m11 = c;    n.m12 = s;    n.m13 = 0.0f; n.m14 = 0.0f;
			n.m21 = -s;   n.m22 = c;	n.m23 = 0.0f; n.m24 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f;	n.m33 = 1.0f; n.m34 = 0.0f;
			n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

			return n;
		}

		template <typename T>
		inline Matrix44<T>& MatrixRotationXYZ(Matrix44<T>& m, float angleX, float angleY, float angleZ)
		{
			Matrix44<T> mx;
			MatrixRotationX(mx, angleX);
			
			Matrix44<T> my;
			MatrixRotationY(my, angleY);

			Matrix44<T> mz;
			MatrixRotationZ(mz, angleZ);

			Matrix44<T> n;
			MatrixMultiply(n, mx, my);

			return MatrixMultiply(m, n, mz);
		}

		template <typename T>
		inline Matrix44<T>& MatrixRotationZYX(Matrix44<T>& m, float angleX, float angleY, float angleZ)
		{
			Matrix44<T> mx;
			MatrixRotationX(mx, angleX);

			Matrix44<T> my;
			MatrixRotationY(my, angleY);

			Matrix44<T> mz;
			MatrixRotationZ(mz, angleZ);

			Matrix44<T> n;
			MatrixMultiply(n, mz, my);

			return MatrixMultiply(m, n, mx);
		}

		// Assume v through the origin, v must be normal Vector
		template <typename T>
		inline Matrix44<T>& MatrixRotationAxis(Matrix44<T>& n, const Vector4<T>& v, float angle)
		{
			ASSERT(v.IsNormalized());

			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			// Common expr 
			float a = 1.0f - c;
			float ax = a * v.x;
			float ay = a * v.y;
			float az = a * v.z;

			n.ZeroTranslation();

			n.m11 = ax * v.x + c;
			n.m12 = ax * v.y + v.z * s;
			n.m13 = ax * v.z - v.y * s;
			n.m14 = 0.0f;

			n.m21 = ay * v.x - az * s;
			n.m22 = ay * v.y + c;
			n.m23 = ay * v.z + v.x * s; 
			n.m24 = 0.0f;

			n.m31 = az * v.x + ay * s;
			n.m32 = az * v.y - ax * s;
			n.m33 = az * v.z + c;
			n.m34 = 0.0f;

			n.m41 = 0.0f;
			n.m42 = 0.0f;
			n.m43 = 0.0f;
			n.m44 = 1.0f;

			return n;
		}

		template <typename OS, typename T>
		inline OS& operator<<(OS& os, const Matrix44<T>& _m)
		{
			os << "\n";
			os << "|" << _m.m11 << "\t" << _m.m12 << "\t" << _m.m13 << "\t" << _m.m14 << "|\n";
			os << "|" << _m.m21 << "\t" << _m.m22 << "\t" << _m.m23 << "\t" << _m.m24 << "|\n";
			os << "|" << _m.m31 << "\t" << _m.m32 << "\t" << _m.m33 << "\t" << _m.m34 << "|\n";
			os << "|" << _m.m41 << "\t" << _m.m42 << "\t" << _m.m43 << "\t" << _m.m44 << "|\n";
			return os;
		}

		//////////////////////////////////////////////////////////////////////////

		template <typename T, typename U>
		inline Vector4<T> operator*(const Vector4<T>& v, const Matrix44<U>& _m)
		{
			return Vector4<T>(v.x * _m.m11 + v.y * _m.m21 + v.z * _m.m31 + v.w * _m.m41,
							  v.x * _m.m12 + v.y * _m.m22 + v.z * _m.m32 + v.w * _m.m42,
							  v.x * _m.m13 + v.y * _m.m23 + v.z * _m.m33 + v.w * _m.m43,
							  v.x * _m.m14 + v.y * _m.m24 + v.z * _m.m34 + v.w * _m.m44);
		}

		template <typename T, typename U>
		inline Vector4<T>& operator*=(Vector4<T>& v, const Matrix44<U>& _m)
		{
			v = v * _m;
			return v;
		}
	}
}

#endif