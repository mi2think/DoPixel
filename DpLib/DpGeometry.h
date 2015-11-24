/********************************************************************
	created:	2014/06/22
	created:	22:6:2014   20:23
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpGeometry.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpGeometry
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Geometry
*********************************************************************/

#ifndef __DP_GEOMETRY__
#define __DP_GEOMETRY__

#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMatrix44.h"

namespace dopixel
{
	namespace math
	{
		// for a plane, a collection of p, which n.(p - p0) = 0
		// n.p - n.p0 = 0
		// we know: x * n.x + y * n.y + z * n.z + d = 0
		// since: d = -n.p0
		// then: n.p + d = 0
		struct Plane
		{
			Vector3f n_; // normal
			float d_;	 // d

			Plane();
			Plane(const Vector3f& n, float d);
			Plane(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2);
			Plane(const Vector3f& n, const Vector3f& p);
			Plane(float a, float b, float c, float d);
			Plane(const Plane& plane);
			Plane& operator=(const Plane& plane);

			// signed distance:
			// =0: p lies in plane
			// <0: p lies on negative side of plane
			// >0: p lies on positive side of plane
			float Distance(const Vector3f& p) const;

			// nearest point in plane
			Vector3f NearestPoint(const Vector3f& p) const;

			void Normalize();

			// m: inverse transpose of the matrix we want to transpose
			void Transform(const Matrix44f& m);
		};

		struct Frustum
		{
			enum PlaneID
			{
				PlaneNear,
				PlaneFar,
				PlaneLeft,
				PlaneRight,
				PlaneTop,
				PlaneBottom,
				PlaneMax
			};
			Plane planes_[PlaneMax];

			Frustum();
			Frustum(const Matrix44f& view, const Matrix44f& proj);
			Frustum(const Matrix44f& viewProj);

			const Plane& GetPlane(PlaneID index) const;
			bool ContainsPoint(const Vector3f& pt) const;
			void ExtractFrustum(const Matrix44f& viewProj);
		};
	}
}

#endif