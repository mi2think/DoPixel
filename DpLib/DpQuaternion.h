/********************************************************************
	created:	2014/06/21
	created:	21:6:2014   23:32
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpQuaternion.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpQuaternion
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Quaternion
*********************************************************************/

#ifndef __DP_QUATERNION__
#define __DP_QUATERNION__

#include "DpMath.h"
#include "DpVector3.h"
#include "DpMatrix43.h"
#include "DpMatrix44.h"

namespace dopixel
{
	namespace math
	{
		class Quaternion
		{
		public:
			float x, y, z, w;

			Quaternion() : x(0), y(0), z(0), w(0) {}
			Quaternion(float _x, float _y, float _z, float _w)
				: x(_x), y(_y), z(_z), w(_w) {}
			Quaternion(const Quaternion& q)
				: x(q.x), y(q.y), z(q.z), w(q.w) {}
			Quaternion(const float* q) { memcpy(this, q, sizeof(float) * 4); }

			bool operator==(const Quaternion& q) const { return Equal(x, q.x) &&  Equal(y, q.y) && Equal(z, q.z) && Equal(w, q.w); }
			bool operator!=(const Quaternion& q) const { return ! operator==(q); }

			Quaternion& operator+=(const Quaternion& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
			Quaternion operator+(const Quaternion& q) const { Quaternion _q = *this; _q += q; return _q; }
			
			Quaternion& operator-=(const Quaternion& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
			Quaternion operator-(const Quaternion& q) const { Quaternion _q = *this; _q -= q; return _q; }

			Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }

			Quaternion& operator*=(float k) { x *= k; y *= k; z *= k; w *= k; return *this; }
			Quaternion operator*(float k) const { Quaternion q = *this; q *= k; return q; }

			Quaternion& operator/=(float k) { float f = 1.0f / k; x *= f; y *= f; z *= f; w *= f; return *this; }
			Quaternion operator/(float k) const { Quaternion q = *this; q /= k; return q; }

			//  [w1 v1] [w2 v2]
			// =[w1w2 - v1.v2   w1v2 + w2v1 + v1xv2]
			Quaternion operator*(const Quaternion& q) const
			{
				Quaternion _q;

				_q.w = w * q.w - x * q.x - y * q.y - z * q.z;
				_q.x = w * q.x + x * q.w + z * q.y - y * q.z;
				_q.y = w * q.y + y * q.w + x * q.z - z * q.x;
				_q.z = w * q.z + z * q.w + y * q.x - x * q.y;
				return _q;
			}

			Quaternion& operator*=(const Quaternion& q)
			{
				*this = *this * q;
				return *this;
			}

			float Length() const
			{
				return Sqrt(x * x + y * y + z * z + w * w);
			}
			float LengthSQ() const
			{
				return x * x + y * y + z * z + w * w;
			}

			void Identity()
			{
				w = 1.0f;
				x = y = z = 0.0f;
			}

			void Normalize()
			{
				float len = Length();
				if (len > 0.0f)
				{
					float f = 1.0f / len;
					x *= f;
					y *= f;
					z *= f;
					w *= f;
				}
				else
				{
					Identity();
				}
			}
			bool IsNormalized() const { return Equal(LengthSQ(), 1.0f); }

			void ToAxisAngle(Vector3f& v, float& angle) const
			{
				angle = float(SafeAcos(w) * 2.0f * 180 / M_PI);

				// [w x y z] = [cos(angle / 2) sin(angle / 2)v]
				// w = cos(angle / 2)  sin(angle /2)^2 + cos(angle /2)^2 = 1;
				
				float s = 1.0f - w * w;
				if (s <= 0.0f)
					v = Vector3f(1.0f, 0.0f, 0.0f);
				else
				{
					float t = 1.0f / Sqrt(s);
					v = Vector3f(x * t, y * t, z * t);
				}
			}
		};

		inline float QuaternionDot(const Quaternion& q, const Quaternion& _q)
		{
			return q.w * _q.w + q.x * _q.x + q.y * _q.y + q.z * _q.z;
		}

		inline Quaternion QuaternionConjugate(const Quaternion& q)
		{
			return Quaternion(-q.x, -q.y, -q.z, q.w);
		}

		inline Quaternion QuaternionInverse(const Quaternion& q)
		{
			Quaternion _q = QuaternionConjugate(q);
			_q /= _q.Length();
			return _q;
		}

		// if q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)
		inline Quaternion QuaternionLn(const Quaternion& q)
		{
			Vector3f v;
			float theta = 0.0f;
			q.ToAxisAngle(v, theta);
			float alpha = theta / 2.0f;
			return Quaternion(alpha * v.x, alpha * v.y, alpha * v.z, 0);
		}

		inline Quaternion QuaternionPow(const Quaternion& q, float exp)
		{
			if (Abs(q.w) > 0.9999f)
				return q;

			float alpha = Acos(q.w);
			float theta = alpha * exp;
			float f = Sin(theta) / Sin(alpha);

			Quaternion _q;
			_q.w = Cos(theta);
			_q.x = q.x * f;
			_q.y = q.y * f;
			_q.z = q.z * f;

			return _q;
		}

		// Spherical linear interpolation between Q0 (t == 0) and Q1 (t == 1).
		// Expects unit quaternions.
		// Slerp(q0, q1, t) = (sin(1 - t)w / sinw)q0 + (sintw / sinw)q1  cosw = q0 ¡¤ q1 / |q0||q1|
		inline Quaternion QuaternionSlerp(const Quaternion& q0, const Quaternion& q1, float t)
		{
			if (t <= 0.0f)
				return q0;
			if (t >= 1.0f)
				return q1;

			float cosw = QuaternionDot(q0, q1);
			float w1 = q1.w;
			float x1 = q1.x;
			float y1 = q1.y;
			float z1 = q1.z;
			
			if (cosw < 0.0f)
			{
				w1 = -w1;
				x1 = -x1;
				y1 = -y1;
				z1 = -z1;
				cosw = -cosw;
			}
			
			assert(cosw <= 1.1f);

			float k0 = 0.0f;
			float k1 = 0.0f;
			if (cosw > 0.9999f)
			{
				k0 = 1.0f - t;
				k1 = t;
			}
			else
			{
				float sinw = Sqrt(1.0f - cosw * cosw);
				float w = Atan2(sinw, cosw);
				float f = 1.0f / sinw;

				k0 = Sin((1.0f - t) * w) * f;
				k1 = Sin(t * w) * f;
			}

			Quaternion q;
			q.x = k0 * q0.x + k1 * x1;
			q.y = k0 * q0.y + k1 * y1;
			q.z = k0 * q0.z + k1 * z1;
			q.w = k0 * q0.w + k1 * w1;
			return q;
		}

		// v must be normal Vector
		inline Quaternion QuaternionRotationAxis(const Vector3f& v, float radian)
		{
			ASSERT(Equal(v.LengthSQ(), 1.0f));

			float thetaOver2 = radian * 0.5f;
			float sinThetaOver2 = Sin(thetaOver2);

			Quaternion q;
			q.w = Cos(thetaOver2);
			q.x = v.x * sinThetaOver2;
			q.y = v.y * sinThetaOver2;
			q.z = v.z * sinThetaOver2;
			return q;
		}

		inline Quaternion QuaternionRotationX(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = Cos(thetaOver2);
			q.x = Sin(thetaOver2);
			q.y = 0.0f;
			q.z = 0.0f;
			return q;
		}

		inline Quaternion QuaternionRotationY(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = Cos(thetaOver2);
			q.x = 0.0f;
			q.y = Sin(thetaOver2);
			q.z = 0.0f;
			return q;
		}

		inline Quaternion QuaternionRotationZ(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = Cos(thetaOver2);
			q.x = 0.0f;
			q.y = 0.0f;
			q.z = Sin(thetaOver2);
			return q;
		}

		inline Quaternion operator*(const Quaternion& q, const Vector3f& v)
		{
			Quaternion p(v.x, v.y, v.z, 0);
			return q * p;
		}

		inline Vector3f QuaternionRotateVector(const Vector3f& v, const Vector3f& axis, float radian)
		{
			Vector3f _v;

			Quaternion q = QuaternionRotationAxis(axis, radian);
			Quaternion qInverse = QuaternionInverse(q);
			Quaternion p = q * v * qInverse;
			_v.x = p.x;
			_v.y = p.y;
			_v.z = p.z;
			return _v;
		}

		// From matrix
		Quaternion QuaternionRotationMatrix(const Matrix43f& _m);

		// To Matrix
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

		template <typename T>
		inline Matrix44<T>& MatrixRotationQuaternion(Matrix44<T>& n, const Quaternion& q)
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
			n.m14 = 0.0f;

			n.m21 = _2xy - _2wz;
			n.m22 = 1.0f - _2x * q.x - _2z * q.z;
			n.m23 = _2yz + _2wx;
			n.m24 = 0.0f;

			n.m31 = _2xz + _2wy;
			n.m32 = _2yz - _2wx;
			n.m33 = 1.0f - _2x * q.x - _2y * q.y;
			n.m34 = 0.0f;

			n.m44 = 1.0f;

			return n;
		}

		template <typename OS>
		inline OS& operator << (OS& os, const Quaternion& q)
		{
			os << "(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
			return os;
		}
	}
}

#endif