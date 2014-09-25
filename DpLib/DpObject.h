/********************************************************************
	created:	2014/06/28
	created:	28:6:2014   20:11
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpObject.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpObject
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Object
*********************************************************************/

#ifndef __DP_OBJECT__
#define __DP_OBJECT__

#include "DpVector4.h"
#include "DpMatrix44.h"
#include "DpColor.h"
using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		class Camera;
		class Device;
		class Light;

		enum
		{
			STATE_ACTIVE	= 0x1,
			STATE_CLIPPED	= 0x2,
			STATE_BACKFACE	= 0x4,

			// Attr of ploy & ployFace
			POLY_ATTR_2SIDE			= 0x1,
			POLY_ATTR_TRANSPARENT	= 0x2,
			POLY_ATTR_8BITCOLOR		= 0x4,
			POLY_ATTR_RGB16			= 0x8,
			POLY_ATTR_RGB24			= 0x10,

			POLY_ATTR_SHADE_PURE	= 0x20,  
			POLY_ATTR_SHADE_FLAT    = 0x40,
			POLY_ATTR_SHADE_GOURAUD = 0x80,
			POLY_ATTR_SHADE_PHONG	= 0x100,
			POLY_ATTR_SHADE_TEXTURE = 0x200,

			POLY_STATE_ACTIVE		= 0x1,
			POLY_STATE_CLIPPED		= 0x2,
			POLY_STATE_BACKFACE		= 0x4,
		};

		// Transform Def
		enum
		{
			TRANSFORM_LOCAL_ONLY,		// Transform local vertex
			TRANSFORM_TRANS_ONLY,		// Transform trans vertex
			TRANSFORM_LOCAL_TO_TRANS,	// Transform local vertex to trans vertex
		};

		struct Poly
		{
			int state;
			int attr;
			Color color;
			Color shadeColor;

			Vector4f* vlist;	// vertex elements
			int vert[3];		// index of vertex
		};

		struct PolyFace
		{
			int state;
			int attr;
			Color color;
			Color shadeColor;

			Vector4f vlist[3];	// vertex of triangle
			Vector4f tlist[3];	// vertex of transformed

			PolyFace* prev;
			PolyFace* next;
		};

		class Object
		{
		public:
			enum { MAX_VERTICES = 64, MAX_PLOYS = 128 };
			enum { STATE_VISIBLE = 0x1, STATE_ACTIVE = 0x2, STATE_CULLED = 0x4 };

			int id;
			char name[64];
			int state;
			int attr;

			int avgRadius;
			int maxRadius;

			Vector4f worldPos;

			// rotate angles in local coordinates
			Vector4f dir;

			// local axis,for store dir of object, updated in rotate
			Vector4f ux, uy, uz;

			int numVertices;	// vertex num of object
			Vector4f vListLocal[MAX_VERTICES];	// store local vertex
			Vector4f vListTrans[MAX_VERTICES];	// store transformed vertex

			int numPolys;		// poly num of object
			Poly pList[MAX_PLOYS];

			// Use m to transform obj
			// transformBase: true for transform ux,uy,uz of object
			void Transform(const Matrix44f& m, int transform, bool transformBase);

			void ModelToWorld(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS);

			void ModelToWorldMatrix(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS);

			void WorldToCamera(const Camera& camera);

			bool Cull(const Camera& camera, int cullFlag);

			void ResetCull();

			void RemoveBackfaces(const Camera& camera);

			void CameraToPerspective(const Camera& camera);

			void CameraToPerspectiveMatrix(const Camera& camera);

			void ConvertFromHomogeneous();

			void PerspectiveToScreen(const Camera& camera);

			void CameraToScreen(const Camera& camera);

			// Lighting
			void Lighting(const Camera& camera, const std::vector<Light>& lights);

			// Render
			void RenderWire(const Device& device) const;

			void RenderSolid(const Device& device) const;
		};

		class RenderList
		{
		public:
			enum { SORT_AvgZ, SORT_NearZ, SORT_FarZ, };
			enum { MAX_PLOYS = 32768 };
			int state;
			int attr;

			int numPolyFaces;
			PolyFace* pPolyFace[MAX_PLOYS];
			PolyFace  polyFace[MAX_PLOYS];

			void Reset() { numPolyFaces = 0; }

			bool InsertPolyFace(const PolyFace& polyFace);

			bool InsertPoly(const Poly& poly);

			bool InsertObject(const Object& object, int insertLocal = 0);

			// Transform

			void Transform(const Matrix44f& m, int transform);

			void ModelToWorld(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS);

			void ModelToWorldMatrix(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS);

			void WorldToCamera(const Camera& camera);

			void SortByZ(int sortType = SORT_AvgZ);

			void RemoveBackfaces(const Camera& camera);

			void CameraToPerspective(const Camera& camera);

			void CameraToPerspectiveMatrix(const Camera& camera);

			void ConvertFromHomogeneous();

			void PerspectiveToScreen(const Camera& camera);

			void CameraToScreen(const Camera& camera);

			// Lighting
			void Lighting(const Camera& camera, const std::vector<Light>& lights);

			// Render
			void RenderWire(const Device& device) const;

			void RenderSolid(const Device& device) const;
		};

		// Transformation & Lighting
		class TLBase
		{
		public:
			static void Lighting(const Camera& camera, const std::vector<Light>& lights, Poly& poly);
		};
	}
}


#endif