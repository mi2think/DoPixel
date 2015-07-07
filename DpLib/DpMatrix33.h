/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   12:10
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMatrix33.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMatrix33
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	3 x 3 Matrix, Row Major!
*********************************************************************/
#ifndef __DP_MATRIX33__
#define __DP_MATRIX33__

#include "DpMath.h"
#include "DpVector3.h"
#include <cassert>

namespace DoPixel
{
	namespace Math
	{
		template <typename T> class Matrix33;
		typedef Matrix33<float> Matrix33f;

		template <typename T>
		class Matrix33
		{
		public:

			enum { R = 3, C = 3, N = R * C };
			union
			{
				struct
				{
					T m11, m12, m13;
					T m21, m22, m23;
					T m31, m32, m33;
				};
				T m[R][C];
			};

			Matrix33() {}
			Matrix33(const T* _m) { memcpy(m, _m, sizeof(T) * N); }
			Matrix33(const Matrix33& _m) { memcpy(m, &_m, sizeof(T) * N); }
			Matrix33(T _m11, T _m12, T _m13,
					 T _m21, T _m22, T _m23,
					 T _m31, T _m32, T _m33)
			: m11(_m11), m12(_m12), m13(_m13)
			, m21(_m21), m22(_m22), m23(_m23)
			, m31(_m31), m32(_m32), m33(_m33) {}

			bool operator==(const Matrix33& _m) const
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
			bool operator!=(const Matrix33& _m) const { return ! operator==(_m); }

			Matrix33& operator+=(const Matrix33& _m)
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
			
			Matrix33 operator+(const Matrix33& _m) const
			{
				Matrix33 n = *this;
				n += _m;
				return n;
			}
			
			Matrix33& operator-=(const Matrix33& _m)
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
			
			Matrix33 operator-(const Matrix33& _m) const
			{
				Matrix33 n = *this;
				n -= _m;
				return n;
			}
			
			template <typename U>
			Matrix33& operator*=(U k)
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
			Matrix33 operator*(U k) const
			{
				Matrix33 n = *this;
				n *= k;
				return n;
			}

			template <typename U>
			Matrix33& operator/=(U k)
			{
				float f = 1.0f / k;
				return operator*=(f);
			}
			template <typename U>
			Matrix33 operator/(U k) const
			{
				Matrix33 n = *this;
				n /= k;
				return n;
			}

			Matrix33 operator-() const
			{
				Matrix33 n;
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						n.m[i][j] = -m[i][j];
					}
				}
				return n;
			}

			Matrix33 operator*(const Matrix33& _m) const
			{
				Matrix33 n;
				
				n.m11 = m11 * _m.m11 + m12 * _m.m21 + m13 * _m.m31;
				n.m12 = m11 * _m.m12 + m12 * _m.m22 + m13 * _m.m32;
				n.m13 = m11 * _m.m13 + m12 * _m.m23 + m13 * _m.m33;

				n.m21 = m21 * _m.m11 + m22 * _m.m21 + m23 * _m.m31;
				n.m22 = m21 * _m.m12 + m22 * _m.m22 + m23 * _m.m32;
				n.m23 = m21 * _m.m13 + m22 * _m.m23 + m23 * _m.m33;

				n.m31 = m31 * _m.m11 + m32 * _m.m21 + m33 * _m.m31;
				n.m32 = m31 * _m.m12 + m32 * _m.m22 + m33 * _m.m32;
				n.m33 = m31 * _m.m13 + m32 * _m.m23 + m33 * _m.m33;

				return n;
			}

			Matrix33& operator *=(const Matrix33& _m)
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
						i == j ? m[i][j] = 1 :m[i][j] = 0;
					}
				}
			}

			static const Matrix33f IdentityMatrix;
			static const Matrix33f ZeroMatrix;
		};

		template <typename T, typename U>
		Matrix33<T> operator*(U k, const Matrix33<T>& _m)
		{
			Matrix33<T> n = _m;
			n *= k;
			return n;
		}

		// Matrix n and _m must not be same one
		template <typename T>
		Matrix33<T>& MatrixTranspose(Matrix33<T>& n, const Matrix33<T>& _m)
		{
			assert(&n != &_m);

			n.m11 = _m.m11; n.m12 = _m.m21; n.m13 = _m.m31;
			n.m21 = _m.m12; n.m22 = _m.m22; n.m23 = _m.m32;
			n.m31 = _m.m13; n.m32 = _m.m23; n.m33 = _m.m33;
			return n;
		}

		template <typename T>
		float MatrixDeterminant(const Matrix33<T>& _m)
		{
			return _m.m11 * (_m.m22 * _m.m33 - _m.m23 * _m.m32) + _m.m12 * (_m.m23 * _m.m31 - _m.m21 * _m.m33)
				 + _m.m13 * (_m.m21 * _m.m32 - _m.m22 * _m.m31);
		}

		template <typename T>
		Matrix33<T>& MatrixInverse(Matrix33<T>& n, const Matrix33<T>& _m)
		{
			float det = MatrixDeterminant(_m);

			assert(abs(det) > EPSILON_E5);
			
			n.m11 = + (_m.m22 * _m.m33 - _m.m32 * _m.m23);
			n.m12 = - (_m.m21 * _m.m33 - _m.m31 * _m.m23);
			n.m13 = + (_m.m21 * _m.m32 - _m.m31 * _m.m22);

			n.m21 = - (_m.m12 * _m.m33 - _m.m32 * _m.m13);
			n.m22 = + (_m.m11 * _m.m33 - _m.m31 * _m.m13);
			n.m23 = - (_m.m11 * _m.m32 - _m.m31 * _m.m12);

			n.m31 = + (_m.m12 * _m.m23 - _m.m22 * _m.m13);
			n.m32 = - (_m.m11 * _m.m23 - _m.m21 * _m.m13);
			n.m33 = + (_m.m11 * _m.m22 - _m.m21 * _m.m12);

			Matrix33<T> _n = n;;
			MatrixTranspose(n, _n);

			n /= det;
			return n;
		}

		template <typename OS, typename T>
		OS& operator<<(OS& os, const Matrix33<T>& _m)
		{
			os << "\n";
			os << "|" << _m.m11 << "\t" << _m.m12 << "\t" << _m.m13 << "|\n";
			os << "|" << _m.m21 << "\t" << _m.m22 << "\t" << _m.m23 << "|\n";
			os << "|" << _m.m31 << "\t" << _m.m32 << "\t" << _m.m33 << "|\n";
			return os;
		}

		//////////////////////////////////////////////////////////////////////////

		template <typename T, typename U>
		Vector3<T> operator*(const Vector3<T>& _v, const Matrix33<U>& _m)
		{
			Vector3<T> v;
			v.x = T(_v.x * _m.m11 + _v.y * _m.m21 + _v.z * _m.m31);
			v.y = T(_v.x * _m.m12 + _v.y * _m.m22 + _v.z * _m.m32);
			v.z = T(_v.x * _m.m13 + _v.y * _m.m23 + _v.z * _m.m33);
			return v;
		}
	}
}

#endif
