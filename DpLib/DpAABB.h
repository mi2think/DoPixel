/********************************************************************
	created:	2015/11/09
	created:	9:11:2015   22:34
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpAABB.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpAABB
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	AABB
*********************************************************************/
#ifndef __DP_AABB__
#define __DP_AABB__

#include "DpMath.h"
#include "DpVector3.h"
#include "DpVector4.h"
#include "DpMatrix44.h"

namespace dopixel
{
	namespace math
	{
		class AABB
		{
		public:
			Vector3f min_;
			Vector3f max_;

			AABB() { Reset(); }
			AABB(const AABB& aabb) : min_(aabb.min_), max_(aabb.max_) {}
			AABB& operator=(const AABB& aabb) { min_ = aabb.min_; max_ = aabb.max_; return *this; }

			bool operator==(const AABB& aabb) const { return min_ == aabb.min_ && max_ == aabb.max_; }
			bool operator!=(const AABB& aabb) const { return !operator==(aabb); }

			Vector3f Size() const { return max_ - min_; }
			float XSize() const { return max_.x - min_.x; }
			float YSize() const { return max_.y - min_.y; }
			float ZSize() const { return max_.z - min_.z; }

			Vector3f Center() const { return (max_ + min_) / 2.0f; }

			bool Valid() const { return min_.x <= max_.x && min_.y <= max_.y && min_.z <= max_.z; }
			void Reset()
			{
				min_.x = min_.y = min_.z = FLT_MAX; 
				max_.x = max_.y = max_.z = FLT_MIN;
			}

			AABB& Add(const Vector3f& pt)
			{
				if (pt.x < min_.x) min_.x = pt.x;
				if (pt.y < min_.y) min_.y = pt.y;
				if (pt.z < min_.z) min_.z = pt.z;

				if (pt.x > max_.x) max_.x = pt.x;
				if (pt.y > min_.y) max_.y = pt.y;
				if (pt.z > min_.z) max_.z = pt.z;

				return *this;
			}
			AABB& Add(const AABB& aabb)
			{
				if (&aabb != this)
				{
					if (aabb.min_.x < min_.x) min_.x = aabb.min_.x;
					if (aabb.min_.y < min_.y) min_.y = aabb.min_.y;
					if (aabb.min_.z < min_.z) min_.z = aabb.min_.z;

					if (aabb.max_.x > max_.x) max_.x = aabb.max_.x;
					if (aabb.max_.y > max_.y) max_.y = aabb.max_.y;
					if (aabb.max_.z > max_.z) max_.z = aabb.max_.z;
				}

				return *this;
			}

			void GetCorners(vector<Vector3f>& vec) const
			{
				Vector3f size = Size();
				Vector3f corners[8] = {
					min_, Vector3f(min_ + Vector3f(0, size.y, 0)),
					Vector3f(min_ + Vector3f(size.x, size.y, 0)), Vector3f(min_ + Vector3f(size.x, 0, 0)),
					Vector3f(min_ + Vector3f(0, 0, size.z)), Vector3f(min_ + Vector3f(0, size.y, size.z)),
					max_, Vector3f(min_ + Vector3f(size.x, 0, size.z))
				};
				return vec.assign(corners, corners + 8);
			}

			AABB& Transform(const Matrix44f& m)
			{
				vector<Vector3f> vec;
				GetCorners(vec);
				Reset();

				vector<Vector4f> vec_t(vec.size());
				for (int i = 0; i < (int)vec_t.size(); ++i)
				{
					vec_t[i] = Vector4f(vec[i].x, vec[i].y, vec[i].z, 1.0f);
					vec_t[i] *= m;
					ASSERT(Equal(vec_t[i].w, 1.0f));
					vec_t[i] /= vec_t[i].w;

					Add(Vector3f(vec_t[i].x, vec_t[i].y, vec_t[i].z));
				}
				return *this;
			}

			bool Contains(const Vector3f& pt) const
			{
				return pt.x >= min_.x && pt.x <= max_.x
					&& pt.y >= min_.y && pt.y <= max_.y
					&& pt.z >= min_.z && pt.z <= max_.z;
			}
		};
	}
}

#endif
