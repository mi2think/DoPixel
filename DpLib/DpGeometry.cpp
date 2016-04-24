/********************************************************************
	created:	2015/11/21
	created:	21:11:2015   11:05
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpGeometry.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpGeometry
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Geometry
*********************************************************************/
#include "DpGeometry.h"
#include "DpVector4.h"

namespace dopixel
{
	namespace math
	{
		Plane::Plane()
		{
		}

		Plane::Plane(const Vector3f& n, float d)
			: n_(n)
			, d_(d)
		{
			ASSERT(n.IsNormalized());
		}

		Plane::Plane(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2)
		{
			Vector3f u = p1 - p0;
			Vector3f v = p2 - p0;
			n_ = CrossProduct(u, v);
			n_.Normalize();
			d_ = -DotProduct(n_, p0);
		}

		Plane::Plane(const Vector3f& n, const Vector3f& p)
			: n_(n)
		{
			ASSERT(n_.IsNormalized());
			d_ = -DotProduct(n_, p);
		}

		Plane::Plane(float a, float b, float c, float d)
			: n_(a, b, c)
			, d_(d)
		{
			Normalize();
		}

		Plane::Plane(const Plane& plane)
			: n_(plane.n_)
			, d_(plane.d_)
		{
		}

		Plane& Plane::operator =(const Plane& plane)
		{
			if (this != &plane)
			{
				n_ = plane.n_;
				d_ = plane.d_;
			}
			return *this;
		}

		float Plane::Distance(const Vector3f& p) const
		{
			return DotProduct(n_, p) + d_;
		}

		Vector3f Plane::NearestPoint(const Vector3f& p) const
		{
			// we assume q is the nearest point in plane for p
			// and k is the nearest distance from p to plane. since n is unit-vector
			// such: q = p + (-kn), and k = Distance(p)
			return p - Distance(p) * n_;
		}

		void Plane::Normalize()
		{
			float len = n_.Length();
			float f = 1.0f / len;
			n_ *= f;
			d_ *= f; // we also need divide d by len
		}

		void Plane::Transform(const Matrix44f& m)
		{
			// just do it! see formula in page 102
			math::Vector4f v(n_.x, n_.y, n_.z, d_);
			v *= m;
			n_.x = v.x;
			n_.y = v.y;
			n_.z = v.z;
			d_ = v.w;
		}

		//////////////////////////////////////////////////////////////////////////

		Frustum::Frustum()
		{
		}

		Frustum::Frustum(const Matrix44f& view, const Matrix44f& proj)
		{
			Matrix44f viewProj;
			MatrixMultiply(viewProj, view, proj);
			ExtractFrustum(viewProj);
		}

		Frustum::Frustum(const Matrix44f& viewProj)
		{
			ExtractFrustum(viewProj);
		}

		const Plane& Frustum::GetPlane(PlaneID index) const
		{
			ASSERT(index < PlaneMax);
			return planes_[index];
		}

		void Frustum::ExtractFrustum(const Matrix44f& viewProj)
		{
			//Fast Extraction of Viewing Frustum Planes - Gribb & Hartmann
			//http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf

			const auto& m = viewProj.m;

			// Near clipping plane
			planes_[Frustum::PlaneNear] = Plane(m[0][2],
				m[1][2],
				m[2][2],
				m[3][2]);

			// Far clipping plane
			planes_[Frustum::PlaneFar] = Plane(m[0][3] - m[0][2],
				m[1][3] - m[1][2],
				m[2][3] - m[2][2],
				m[3][3] - m[3][2]);

			//left clipping plane
			planes_[Frustum::PlaneLeft] = Plane(m[0][3] + m[0][0],
				m[1][3] + m[1][0],
				m[2][3] + m[2][0],
				m[3][3] + m[3][0]);

			// Right clipping plane
			planes_[Frustum::PlaneRight] = Plane(m[0][3] - m[0][0],
				m[1][3] - m[1][0],
				m[2][3] - m[2][0],
				m[3][3] - m[3][0]);

			// Top clipping plane
			planes_[Frustum::PlaneTop] = Plane(m[0][3] - m[0][1],
				m[1][3] - m[1][1],
				m[2][3] - m[2][1],
				m[3][3] - m[3][1]);

			// Bottom clipping plane
			planes_[Frustum::PlaneBottom] = Plane(m[0][3] + m[0][1],
				m[1][3] + m[1][1],
				m[2][3] + m[2][1],
				m[3][3] + m[3][1]);
		}

		bool Frustum::ContainsPoint(const Vector3f& pt) const
		{
			for (int i = 0; i < Frustum::PlaneMax; i++)
			{
				if (planes_[i].Distance(pt) < 0)
					return false;
			}
			return true;
		}

		//////////////////////////////////////////////////////////////////////////

		bool Intersect(const Plane& plane, const Ray& ray, float& t)
		{
			//see formula in page 99
			float dp = DotProduct(plane.n_, ray.dir_);
			if (Equal(dp, 0.0f))
			{
				// ray is parallel to plane
				// may the ray lies in the plane if DotProduct(plane.n_, ray.pt_) + plane.d_ == 0
				// we treat this as not intersect yet.
				return false;
			}

			float k = DotProduct(plane.n_, ray.pt_);
			float t0 = -(k + plane.d_) / dp;
			if (t0 < 0)
				return false;
			t = t0;
			return true;
		}

		bool Intersect(const Plane& plane, const LineSeg& line, float& t)
		{
			Vector3f dir = line.v2_ - line.v1_;
			dir.Normalize();

			Ray ray(line.v1_, dir);
			float t0;
			if (Intersect(plane, ray, t0) && t0 <= 1.0f)
			{
				t = t0;
				return true;
			}
			return false;
		}
	}
}