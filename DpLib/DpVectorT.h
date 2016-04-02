/********************************************************************
	created:	2016/04/02
	created:	2:4:2016   9:05
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpVectorT.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpVectorT
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	VectorT
*********************************************************************/
#ifndef __VECTORT_H__
#define __VECTORT_H__

namespace dopixel
{
	namespace math
	{
		template<typename T0, typename T1>
		class Vector2T
		{
		public:
			T0 t0;
			T1 t1;

			Vector2T() : t0(T0()), t1(T1()) {}
			Vector2T(const T0& _t0, const T1& _t1) : t0(_t0), t1(_t1) {}
			Vector2T(const Vector2T& vec) : t0(vec.t0), t1(vec.t1) {}
			Vector2T& operator=(const Vector2T& vec) { t0 = vec.t0, t1 = vec.t1; return *this; }
			
			Vector2T operator-() const { return Vector2T(-t0, -t1); }

			Vector2T operator+(const Vector2T& vec) const { return Vector2T(t0 + vec.t0, t1 + vec.t1); }
			Vector2T operator-(const Vector2T& vec) const { return Vector2T(t0 - vec.t0, t1 - vec.t1); }

			Vector2T& operator+=(const Vector2T& vec) { t0 += vec.t0; t1 += vec.t1; return *this; }
			Vector2T& operator-=(const Vector2T& vec) { t0 -= vec.t0; t1 -= vec.t1; return *this; }

			template<typename U>
			Vector2T operator*(const U& k) const { return Vector2T(t0 * k, t1 * k); }
			template<typename U>
			Vector2T operator/(const U& k) const { float f = 1.0f / k; return *this * f; }

			template<typename U>
			Vector2T& operator*=(const U& k) { t0 *= k; t1 *= k; return *this; }
			template<typename U>
			Vector2T& operator/=(const U& k) { float f = 1.0f / k; *this *= f; return *this;}
		};
		
		template<typename T0, typename T1, typename U>
		inline Vector2T<T0, T1> operator*(const U& k, const Vector2T<T0, T1>& vec)
		{
			return vec * k;
		}

		template<typename T0, typename T1, typename U>
		inline Vector2T<T0, T1> operator/(const U& k, const Vector2T<T0, T1>& vec)
		{
			return vec * (1.0f / k);
		}

		template<typename T0, typename T1, typename T2>
		class Vector3T
		{
		public:
			T0 t0;
			T1 t1;
			T2 t2;

			Vector3T() : t0(T0()), t1(T1()), t2(T2()) {}
			Vector3T(const T0& _t0, const T1& _t1, const T2& _t2) : t0(_t0), t1(_t1), t2(_t2) {}
			Vector3T(const Vector3T& vec) : t0(vec.t0), t1(vec.t1), t2(vec.t2) {}
			Vector3T& operator=(const Vector3T& vec) { t0 = vec.t0, t1 = vec.t1; t2 = vec.t2; return *this; }

			Vector3T operator-() const { return Vector3T(-t0, -t1, -t2); }

			Vector3T operator+(const Vector3T& vec) const { return Vector3T(t0 + vec.t0, t1 + vec.t1, t2 + vec.t2); }
			Vector3T operator-(const Vector3T& vec) const { return Vector3T(t0 - vec.t0, t1 - vec.t1, t2 - vec.t2); }

			Vector3T& operator+=(const Vector3T& vec) { t0 += vec.t0; t1 += vec.t1; t2 += vec.t2; return *this; }
			Vector3T& operator-=(const Vector3T& vec) { t0 -= vec.t0; t1 -= vec.t1; t2 -= vec.t2; return *this; }

			template<typename U>
			Vector3T operator*(const U& k) const { return Vector3T(t0 * k, t1 * k, t2 * k); }
			template<typename U>
			Vector3T operator/(const U& k) const { float f = 1.0f / k; return *this * f; }

			template<typename U>
			Vector3T& operator*=(const U& k) { t0 *= k; t1 *= k; t2 *= k; return *this; }
			template<typename U>
			Vector3T& operator/=(const U& k) { float f = 1.0f / k; *this *= f; return *this; }
		};

		template<typename T0, typename T1, typename T2, typename U>
		inline Vector3T<T0, T1, T2> operator*(const U& k, const Vector3T<T0, T1, T2>& vec)
		{
			return vec * k;
		}

		template<typename T0, typename T1, typename T2, typename U>
		inline Vector3T<T0, T1, T2> operator/(const U& k, const Vector3T<T0, T1, T2>& vec)
		{
			return vec * (1.0f / k);
		}

		template<typename T0, typename T1, typename T2, typename T3>
		class Vector4T
		{
		public:
			T0 t0;
			T1 t1;
			T2 t2;
			T3 t3;

			Vector4T() : t0(T0()), t1(T1()), t2(T2()), t3(T3()) {}
			Vector4T(const T0& _t0, const T1& _t1, const T2& _t2, const T3& _t3) : t0(_t0), t1(_t1), t2(_t2), t3(_t3) {}
			Vector4T(const Vector4T& vec) : t0(vec.t0), t1(vec.t1), t2(vec.t2), t3(vec.t3) {}
			Vector4T& operator=(const Vector4T& vec) { t0 = vec.t0, t1 = vec.t1; t2 = vec.t2; t3 = vec.t3; return *this; }

			Vector4T operator-() const { return Vector4T(-t0, -t1, -t2, -t3); }

			Vector4T operator+(const Vector4T& vec) const { return Vector4T(t0 + vec.t0, t1 + vec.t1, t2 + vec.t2, t3 + vec.t3); }
			Vector4T operator-(const Vector4T& vec) const { return Vector4T(t0 - vec.t0, t1 - vec.t1, t2 - vec.t2, t3 - vec.t3); }

			Vector4T& operator+=(const Vector4T& vec) { t0 += vec.t0; t1 += vec.t1; t2 += vec.t2; t3 += vec.t3; return *this; }
			Vector4T& operator-=(const Vector4T& vec) { t0 -= vec.t0; t1 -= vec.t1; t2 -= vec.t2; t3 -= vec.t3; return *this; }

			template<typename U>
			Vector4T operator*(const U& k) const { return Vector4T(t0 * k, t1 * k, t2 * k, t3 * k); }
			template<typename U>
			Vector4T operator/(const U& k) const { float f = 1.0f / k; return *this * f; }

			template<typename U>
			Vector4T& operator*=(const U& k) { t0 *= k; t1 *= k; t2 *= k; t3 *= k; return *this; }
			template<typename U>
			Vector4T& operator/=(const U& k) { float f = 1.0f / k; *this *= f; return *this; }
		};

		template<typename T0, typename T1, typename T2, typename T3, typename U>
		inline Vector4T<T0, T1, T2, T3> operator*(const U& k, const Vector4T<T0, T1, T2, T3>& vec)
		{
			return vec * k;
		}

		template<typename T0, typename T1, typename T2, typename T3, typename U>
		inline Vector4T<T0, T1, T2, T3> operator/(const U& k, const Vector4T<T0, T1, T2, T3>& vec)
		{
			return vec * (1.0f / k);
		}
	}
}

#endif