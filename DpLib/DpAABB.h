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

#include <limits>

namespace dopixel
{
	namespace math
	{
		class AABB
		{
		public:
			Vector3f min;
			Vector3f max;

			AABB() { Reset(); }
			AABB(const AABB& aabb) : min(aabb.min), max(aabb.max) {}
			AABB& operator=(const AABB& aabb) { min = aabb.min; max = aabb.max; return *this; }

			bool operator==(const AABB& aabb) const { return min == aabb.min && max == aabb.max; }
			bool operator!=(const AABB& aabb) const { return !operator==(aabb); }

			Vector3f Size() const { return max - min; }
			float XSize() const { return max.x - min.x; }
			float YSize() const { return max.y - min.y; }
			float ZSize() const { return max.z - min.z; }

			Vector3f Center() const { return (max + min) / 2.0f; }

			bool Valid() const { return min.x > max.x || min.y > max.y || min.z > max.z; }
			void Reset()
			{
				min.x = min.y = min.z = FLT_MAX; 
				max.x = max.y = max.z = FLT_MIN;
			}

			AABB& Add(const Vector3f& pt)
			{
				if (pt.x < min.x) min.x = pt.x;
				if (pt.y < min.y) min.y = pt.y;
				if (pt.z < min.z) min.z = pt.z;

				if (pt.x > max.x) max.x = pt.x;
				if (pt.y > min.y) max.y = pt.y;
				if (pt.z > min.z) max.z = pt.z;

				return *this;
			}
			AABB& Add(const AABB& aabb)
			{
				if (aabb.min.x < min.x) min.x = aabb.min.x;
				if (aabb.min.y < min.y) min.y = aabb.min.y;
				if (aabb.min.z < min.z) min.z = aabb.min.z;

				if (aabb.max.x > max.x) max.x = aabb.max.x;
				if (aabb.max.y > max.y) max.y = aabb.max.y;
				if (aabb.max.z > max.z) max.z = aabb.max.z;

				return *this;
			}

			bool Contains(const Vector3f& pt) const
			{
				return pt.x >= min.x && pt.x <= max.x
					&& pt.y >= min.y && pt.y <= max.y
					&& pt.z >= min.z && pt.z <= max.z;
			}
		};
	}
}

#endif
