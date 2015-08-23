/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   23:08
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMatrix43.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMatrix43
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	4 x 3 Matrix, Row Major! Assume the 4th Col:[0 0 0 1]
*********************************************************************/
#ifndef __DP_MATRIX43__
#define __DP_MATRIX43__

#include "DpMath.h"
#include "DpVector3.h"
#include "DpQuaternion.h"

namespace dopixel
{
	namespace math
	{

		template <typename T> class Matrix43;
		typedef Matrix43<float> Matrix43f;

		template <typename T>
		class Matrix43
		{
		public:

			enum { R = 4, C = 3, N = R * C };
			union
			{
				struct 
				{
					T m11, m12, m13;
					T m21, m22, m23;
					T m31, m32, m33;
					T tx,  ty,  tz;
				};
				T m[R][C];
			};

			Matrix43() {}
			Matrix43(const T* _m) { memcpy(m, _m, sizeof(T) * N); }
			Matrix43(const Matrix43& _m) { memcpy(m, _m.m, sizeof(T) * N); }
			Matrix43(T _m11, T _m12, T _m13,
				     T _m21, T _m22, T _m23,
					 T _m31, T _m32, T _m33,
					 T x,	 T y,    T z)
			: m11(_m11), m12(_m12), m13(_m13)
			, m21(_m21), m22(_m22), m23(_m23)
			, m31(_m31), m32(_m32), m33(_m33)
			, tx(x), ty(y), tz(z) {}

			bool operator==(const Matrix43& _m) const
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
			bool operator!=(const Matrix43& _m) const { return ! operator==(_m); }

			Matrix43& operator+=(const Matrix43& _m)
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

			Matrix43 operator+(const Matrix43& _m) const
			{
				Matrix43 n = *this;
				n += _m;
				return n;
			}

			Matrix43& operator-=(const Matrix43& _m)
			{
				for (int i = 0; i < R; ++i)
				{
					for(int j = 0; j < C; ++j)
					{
						m[i][j] -= _m.m[i][j];
					}
				}
				return *this;
			}

			Matrix43 operator-(const Matrix43& _m) const
			{
				Matrix43 n = *this;
				n -= _m;
				return n;
			}

			template <typename U>
			Matrix43& operator*=(U k)
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
			Matrix43 operator*(U k) const
			{
				Matrix43 n = *this;
				n *= k;
				return n;
			}

			template <typename U>
			Matrix43& operator/=(U k)
			{
				float f = 1.0f / k;
				return operator*=(f);
			}
			template <typename U>
			Matrix43 operator/(U k) const
			{
				Matrix43 n = *this;
				n /= k;
				return n;
			}

			Matrix43 operator-() const
			{
				Matrix43 n;
				for (int i = 0; i < R; ++i)
				{
					for (int j = 0; j < C; ++j)
					{
						n.m[i][j] = -m[i][j];
					}
				}
				return n;
			}

			Matrix43 operator*(const Matrix43& _m) const
			{
				Matrix43 n;

				n.m11 = m11 * _m.m11 + m12 * _m.m21 + m13 * _m.m31;
				n.m12 = m11 * _m.m12 + m12 * _m.m22 + m13 * _m.m32;
				n.m13 = m11 * _m.m13 + m12 * _m.m23 + m13 * _m.m33;

				n.m21 = m21 * _m.m11 + m22 * _m.m21 + m23 * _m.m31;
				n.m22 = m21 * _m.m12 + m22 * _m.m22 + m23 * _m.m32;
				n.m23 = m21 * _m.m13 + m22 * _m.m23 + m23 * _m.m33;

				n.m31 = m31 * _m.m11 + m32 * _m.m21 + m33 * _m.m31;
				n.m32 = m31 * _m.m12 + m32 * _m.m22 + m33 * _m.m32;
				n.m33 = m31 * _m.m13 + m32 * _m.m23 + m33 * _m.m33;

				n.tx  = tx  * _m.m11 + ty  * _m.m21 + tz  * _m.m31 + _m.tx;
				n.ty  = tx  * _m.m12 + ty  * _m.m22 + tz  * _m.m32 + _m.ty;
				n.tz  = tx  * _m.m13 + ty  * _m.m23 + tz  * _m.m33 + _m.tz;

				return n;
			}

			Matrix43& operator*=(const Matrix43& _m)
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
				tx = ty = tz = 0;
			}

			void SetTranslation(const Vector3<T>& v)
			{
				tx = v.x;
				ty = v.y;
				tz = v.z;
			}

			Vector3<T> GetTranslation() const
			{
				return Vector3<T>(tx, ty, tz);
			}
		};

		template <typename T, typename U>
		inline Matrix43<T> operator*(U k, const Matrix43<T>& _m)
		{
			Matrix43<T> n = _m;
			n *= k;
			return n;
		}

		template <typename T>
		inline Matrix43<T>& MatrixMultiply(Matrix43<T>& n, const Matrix43<T>& m1, const Matrix43<T>& m2)
		{
			n.m11 = m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31;
			n.m12 = m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32;
			n.m13 = m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33;

			n.m21 = m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31;
			n.m22 = m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32;
			n.m23 = m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33;

			n.m31 = m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31;
			n.m32 = m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32;
			n.m33 = m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33;

			n.tx  = m1.tx  * m2.m11 + m1.ty  * m2.m21 + m1.tz  * m2.m31 + m2.tx;
			n.ty  = m1.tx  * m2.m12 + m1.ty  * m2.m22 + m1.tz  * m2.m32 + m2.ty;
			n.tz  = m1.tx  * m2.m13 + m1.ty  * m2.m23 + m1.tz  * m2.m33 + m2.tz;

			return n;
		}

		// Determinant of 3x3 of 4x3 
		template <typename T>
		inline float MatrixDeterminant(const Matrix43<T>& _m)
		{
			return _m.m11 * (_m.m22 * _m.m33 - _m.m23 * _m.m32) + _m.m12 * (_m.m23 * _m.m31 - _m.m21 * _m.m33)
				+ _m.m13 * (_m.m21 * _m.m32 - _m.m22 * _m.m31);
		}

		template <typename T>
		inline Matrix43<T>& MatrixInverse(Matrix43<T>& n, const Matrix43<T>& _m)
		{
			float det = MatrixDeterminant(_m);

			assert(abs(det) > eps);

			float f = 1.0f / det;
			
			n.m11 = + (_m.m22 * _m.m33 - _m.m32 * _m.m23) * f;
			n.m12 = - (_m.m12 * _m.m33 - _m.m32 * _m.m13) * f;
			n.m13 = + (_m.m12 * _m.m23 - _m.m22 * _m.m13) * f;

			n.m21 = - (_m.m21 * _m.m33 - _m.m31 * _m.m23) * f;
			n.m22 = + (_m.m12 * _m.m33 - _m.m32 * _m.m13) * f;
			n.m23 = - (_m.m11 * _m.m23 - _m.m21 * _m.m13) * f;

			n.m31 = + (_m.m21 * _m.m32 - _m.m31 * _m.m22) * f;
			n.m32 = - (_m.m11 * _m.m32 - _m.m31 * _m.m12) * f;
			n.m33 = + (_m.m11 * _m.m22 - _m.m21 * _m.m12) * f;

			// Inverse of translation
			n.tx = - (_m.tx * _m.m11 + _m.ty * _m.m21 + _m.tz * _m.m31);
			n.ty = - (_m.tx * _m.m12 + _m.ty * _m.m22 + _m.tz * _m.m32);
			n.tz = - (_m.tx * _m.m13 + _m.ty * _m.m23 + _m.tz * _m.m33);

			return n;
		}

		template <typename T>
		inline Matrix43<T>& MaxtrixTranslation(Matrix43<T>& n, const Vector3<T>& v)
		{
			for (int i = 0; i < Matrix43<T>::R; ++i)
			{
				for (int j = 0; j < Matrix43<T>::C; ++j)
				{
					i == j ? n.m[i][j] = 1 : n.m[i][j] = 0;
				}
			}

			n.tx = v.x;
			n.ty = v.y;
			n.tz = v.z;

			return n;
		}

		// Scale

		// scale by coordinate axis
		template <typename T>
		inline Matrix43<T>& MatrixScaling(Matrix43<T>& n, const Vector3<T>& v)
		{
			n.ZeroTranslation();

			n.m11 = v.x;  n.m12 = 0.0f; n.m13 = 0.0f;
			n.m21 = 0.0f; n.m22 = v.y;  n.m23 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f; n.m33 = v.z;

			return n;
		}

		// scale by Vector v, v must be normal Vector
		template <typename T>
		inline Matrix43<T>& MatrixScalingAxis(Matrix43<T>& n, const Vector3<T>& v, float k)
		{
			assert(v.IsNormalized());

			float a = k - 1.0f;
			float ax = a * v.x;
			float ay = a * v.y;
			float az = a * v.z;

			n.ZeroTranslation();

			n.m11 = ax * v.x + 1.0f;
			n.m12 = ay * v.y + 1.0f;
			n.m13 = az * v.z + 1.0f;

			n.m12 = n.m21 = ax * v.y;
			n.m13 = n.m31 = ax * v.z;
			n.m23 = n.m32 = ay * v.z;

			return n;
		}

		// Projection

		// Projection on the plane, where v กอ plane & plane through the origin, v must be normal Vector
		template <typename T>
		inline Matrix43<T>& MatrixProjection(Matrix43<T>& n,const Vector3<T>& v)
		{
			assert(v.IsNormalized());

			n.ZeroTranslation();

			n.m11 = 1.0f - v.x * v.x;
			n.m22 = 1.0f - v.y * v.y;
			n.m33 = 1.0f - v.z * v.z;

			n.m12 = n.m21 = -v.x * v.y;
			n.m13 = n.m31 = -v.x * v.z;
			n.m23 = n.m32 = -v.y * v.z;

			return n;
		}

		// Reflect

		// Reflect by plane x = k
		template <typename T>
		inline Matrix43<T>& MatrixReflectX(Matrix43<T>& n, float k)
		{
			n.m11 =-1.0f; n.m12 = 0.0f; n.m13 = 0.0f;
			n.m21 = 0.0f; n.m22 = 1.0f; n.m23 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f; n.m33 = 1.0f;

			n.tx = 2.0f * k;
			n.ty = 0.0f;
			n.tz = 0.0f;

			return n;
		}

		// Reflect by plane y = k
		template <typename T>
		inline Matrix43<T>& MatrixReflectY(Matrix43<T>& n, float k)
		{
			n.m11 = 1.0f;  n.m12 = 0.0f; n.m13 = 0.0f;
			n.m21 = 0.0f;  n.m22 =-1.0f; n.m23 = 0.0f;
			n.m31 = 0.0f;  n.m32 = 0.0f; n.m33 = 1.0f;

			n.tx = 0.0f;
			n.ty = 2.0f * k;
			n.tz = 0.0f;

			return n;
		}

		// Reflect by plane z = k
		template <typename T>
		inline Matrix43<T>& MatrixReflectZ(Matrix43<T>& n, float k)
		{
			n.m11 = 1.0f;  n.m12 = 0.0f; n.m13 = 0.0f;
			n.m21 = 0.0f;  n.m22 = 1.0f; n.m23 = 0.0f;
			n.m31 = 0.0f;  n.m32 = 0.0f; n.m33 =-1.0f;

			n.tx = 0.0f;
			n.ty = 0.0f;
			n.tz = 2.0f * k;

			return n;
		}

		// Reflect, v must be normal Vector
		template <typename T>
		inline Matrix43<T>& MatrixReflectAxis(Matrix43<T>& n, const Vector3<T>& v)
		{
			assert(v.IsNormalized());

			n.ZeroTranslation();

			float ax = -2.0f * v.x;
			float ay = -2.0f * v.y;
			float az = -2.0f * v.z;

			n.m11 = 1.0f + ax * v.x;
			n.m12 = 1.0f + ay * v.y;
			n.m13 = 1.0f + az * v.z;

			n.m12 = n.m21 = ax * v.y;
			n.m13 = n.m31 = ax * v.z;
			n.m23 = n.m32 = ay * v.z;

			return n;
		}

		// Rotate

		template <typename T>
		inline Matrix43<T>& MatrixRotationX(Matrix43<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.ZeroTranslation();

			n.m11 = 1.0f; n.m12 = 0.0f; n.m13 = 0.0f;
			n.m21 = 0.0f; n.m22 = c;	n.m23 = s;
			n.m31 = 0.0f; n.m32 = -s;	n.m33 = c;

			return n;
		}

		template <typename T>
		inline Matrix43<T>& MatrixRotationY(Matrix43<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.ZeroTranslation();

			n.m11 = c;    n.m12 = 0.0f; n.m13 = -s;
			n.m21 = 0.0f; n.m22 = 1.0f;	n.m23 = 0;
			n.m31 = s;	  n.m32 = 0.0f;	n.m33 = c;

			return n;
		}

		template <typename T>
		inline Matrix43<T>& MatrixRotationZ(Matrix43<T>& n, float angle)
		{
			float s = 0.0f;
			float c = 0.0f;
			SinCos(s, c, angle);

			n.ZeroTranslation();

			n.m11 = c;    n.m12 = s;    n.m13 = 0.0f;
			n.m21 = -s;   n.m22 = c;	n.m23 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f;	n.m33 = 1.0f;

			return n;
		}

		// Assume v through the origin, v must be normal Vector
		template <typename T>
		inline Matrix43<T>& MatrixRotationAxis(Matrix43<T>& n, const Vector3<T>& v, float angle)
		{
			assert(v.IsNormalized());

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

			n.m21 = ay * v.x - az * s;
			n.m22 = ay * v.y + c;
			n.m23 = ay * v.z + v.x * s; 

			n.m31 = az * v.x + ay * s;
			n.m32 = az * v.y - ax * s;
			n.m33 = az * v.z + c;

			return n;
		}

		// Shear XY by z
		template<typename T>
		inline Matrix43<T>& MatrixShearXY(Matrix43<T>& n, float s, float t)
		{
			n.ZeroTranslation();

			n.m11 = 1.0f; n.m12 = 0.0f;  n.m13 = 0.0f;
			n.m21 = 0.0f; n.m22 = 1.0f;  n.m23 = 0.0f;
			n.m31 = s;    n.m32 = t;     n.m33 = 0.0f;

			return n;
		}

		// Shear XZ by Y
		template<typename T>
		inline Matrix43<T>& MatrixShearXZ(Matrix43<T>& n, float s, float t)
		{
			n.ZeroTranslation();

			n.m11 = 1.0f; n.m12 = 0.0f;  n.m13 = 0.0f;
			n.m21 = s;    n.m22 = 1.0f;  n.m23 = t;
			n.m31 = 0.0f; n.m32 = 0.0f;  n.m33 = 1.0f;

			return n;
		}

		// Shear YZ by X
		template<typename T>
		inline Matrix43<T>& MatrixShearYZ(Matrix43<T>& n, float s, float t)
		{
			n.ZeroTranslation();

			n.m11 = 1.0f; n.m12 = s;     n.m13 = t;
			n.m21 = 0.0f; n.m22 = 1.0f;  n.m23 = 0.0f;
			n.m31 = 0.0f; n.m32 = 0.0f;  n.m33 = 1.0f;

			return n;
		}

		template <typename OS, typename T>
		inline OS& operator<<(OS& os, const Matrix43<T>& _m)
		{
			os << "\n";
			os << "|" << _m.m11 << "\t" << _m.m12 << "\t" << _m.m13 << "|\n";
			os << "|" << _m.m21 << "\t" << _m.m22 << "\t" << _m.m23 << "|\n";
			os << "|" << _m.m31 << "\t" << _m.m32 << "\t" << _m.m33 << "|\n";
			os << "|" << _m.tx  << "\t" << _m.ty  << "\t" << _m.tz  << "|\n";
			return os;
		}

		//////////////////////////////////////////////////////////////////////////

		template <typename T, typename U>
		inline Vector3<T> operator*(const Vector3<T>& v, const Matrix43<U>& _m)
		{
			return Vector3<T>(v.x * _m.m11 + v.y * _m.m21 + v.z * _m.m31,
							  v.x * _m.m12 + v.y * _m.m22 + v.z * _m.m32,
							  v.x * _m.m13 + v.y * _m.m23 + v.z * _m.m33);
		}

		template <typename T, typename U>
		inline Vector3<T>& operator*=(Vector3<T>& v, const Matrix43<U>& _m)
		{
			v = v * _m;
			return v;
		}

		// From Quaternion

		template <typename T>
		inline Matrix43<T>& MatrixRotationQuaternion(Matrix43<T>& n, const Quaternion& q)
		{
			// Common expr

			float _2x = 2.0f * q.x;
			float _2y = 2.0f * q.y;
			float _2z = 2.0f * q.z;

			float _2xy = _2x * q.y;
			float _2yz = _2y * q.z;
			float _2xz = _2z * q.x;

			float _2wx = _2x * q.w;
			float _2wy = _2y * q.w;
			float _2wz = _2z * q.w;

			n.ZeroTranslation();	

			n.m11 = 1.0f - _2y * q.y - _2z * q.z;
			n.m12 = _2xy + _2wz;
			n.m13 = _2xz - _2wy;

			n.m21 = _2xy - _2wz;
			n.m22 = 1.0f - _2x * q.x - _2z * q.z;
			n.m23 = _2yz + _2wx;

			n.m31 = _2xz + _2wy;
			n.m32 = _2yz - _2wx;
			n.m33 = 1.0f - _2x * q.x - _2y * q.y;

			return n;
		}

		// From Yaw-Pitch-Roll
		// Yaw around the Y axis, a pitch around the X axis, and a roll around the Z axis.
	/*	template <typename T>
		inline Matrix43<T> MatrixRotationYawPitchRoll(float yaw, float pitch, float roll)
		{

		}
		*/
	}
}

#endif