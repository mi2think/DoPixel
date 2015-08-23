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
#include "DpVector2.h"
#include "DpMatrix44.h"
#include "DpColor.h"
#include "DpMaterial.h"
#include "DpDevice.h"

using namespace dopixel::math;

namespace dopixel
{
	namespace core
	{
		class Camera;
		class Light;

		//////////////////////////////////////////////////////////////////////////

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
			POLY_ATTR_RGB32			= 0x20,

			POLY_ATTR_SHADE_PURE	= 0x40,  // Constant, Emissive
			POLY_ATTR_SHADE_FLAT    = 0x80,
			POLY_ATTR_SHADE_GOURAUD = 0x100,
			POLY_ATTR_SHADE_PHONG	= 0x200,
			POLY_ATTR_SHADE_TEXTURE = 0x400,

			POLY_STATE_ACTIVE		= 0x1,
			POLY_STATE_CLIPPED		= 0x2,
			POLY_STATE_BACKFACE		= 0x4,
			POLY_STATE_LIT			= 0x8,
		};

		// Transform Def
		enum
		{
			TRANSFORM_LOCAL_ONLY,		// Transform local vertex
			TRANSFORM_TRANS_ONLY,		// Transform trans vertex
			TRANSFORM_LOCAL_TO_TRANS,	// Transform local vertex to trans vertex
		};

		// Cull flag
		enum { CULL_PLANE_X = 0x1, CULL_PLANE_Y = 0x2, CULL_PLANE_Z = 0x4, CULL_PLANE_XYZ = CULL_PLANE_X | CULL_PLANE_Y | CULL_PLANE_Z };

		// using outer vertex list
		struct Poly
		{
			int state;			// state status
			int attr;

			Texture* texture;
			int materialId;

			Vertex* vlist;		// vertex list
			int vert[3];		// index of vertex

			// why do not store coord in Vertex???

			Vector2f* clist;	// texture coord list
			int coord[3];		// index of texture coord list

			Vector4f GetFacetNormal() const
			{
				const auto& v0 = vlist[vert[0]];
				const auto& v1 = vlist[vert[1]];
				const auto& v2 = vlist[vert[2]];

				Vector4f u = v1.p - v0.p;
				Vector4f v = v2.p - v0.p;
				Vector4f n = CrossProduct(u, v);
				return n;
			}

			void ResetCull()
			{
				state &= (~POLY_STATE_BACKFACE);
				state &= (~POLY_STATE_CLIPPED);
				state &= (~POLY_STATE_LIT);
				// Reset vertex lit
				vlist[vert[0]].attr &= (~Vertex::Attr_Lit);
				vlist[vert[1]].attr &= (~Vertex::Attr_Lit);
				vlist[vert[2]].attr &= (~Vertex::Attr_Lit);
			}
		};

		// using inner vertex list
		struct PolyFace
		{
			int state;			// state status
			int attr;

			Texture* texture;
			int materialId;

			Vector4f normal;	// normal

			float avg_z;		// avg of z, for simple sort

			Vertex vlist[3];	// vertex of triangle
			Vertex tlist[3];	// vertex of transformed

			PolyFace* prev;
			PolyFace* next;

			PolyFace& operator=(const Poly& poly)
			{
				attr = poly.attr;
				state = poly.state;
				
				texture = poly.texture;
				materialId = poly.materialId;

				vlist[0] = poly.vlist[poly.vert[0]];
				vlist[1] = poly.vlist[poly.vert[1]];
				vlist[2] = poly.vlist[poly.vert[2]];

				vlist[0].uv0 = poly.clist[poly.coord[0]];
				vlist[1].uv0 = poly.clist[poly.coord[1]];
				vlist[2].uv0 = poly.clist[poly.coord[2]];

				// tlist is not transformed at this point, so it use vlist(poly local vertex)
				tlist[0] = poly.vlist[poly.vert[0]];
				tlist[1] = poly.vlist[poly.vert[1]];
				tlist[2] = poly.vlist[poly.vert[2]];

				tlist[0].uv0 = poly.clist[poly.coord[0]];
				tlist[1].uv0 = poly.clist[poly.coord[1]];
				tlist[2].uv0 = poly.clist[poly.coord[2]];

				// texture coord
				// why not store in Vertex ???, see Poly.

				return *this;
			}
		};

		// including muti-frame which use same geometry but different vertex
		class Object
		{
		public:
			enum { MAX_VERTICES = 10240, MAX_PLOYS = 10240 };
			enum { STATE_VISIBLE = 0x1, STATE_ACTIVE = 0x2, STATE_CULLED = 0x4, };
			enum { ATTR_SINGLE_FRAME = 0x1, ATTR_MULTI_FRAME = 0x2, ATTR_TEXTURES = 0x4, };

			int id;
			char name[64];
			int state;
			int attr;

			int materialId;

			float* avgRadius;		// Now object contain multi frame, the avg or max radius of each frame
			float* maxRadius;		// may different, so it's array.

			Vector4f worldPos;

			Vector4f dir;			// rotate angles in local coordinates

			Vector4f ux, uy, uz;	// local axis,for store dir of object, updated in rotate

			int numFrames;			// num of object's frame
			int numVertices;		// vertex num of object's single frame
			int totalVertices;		// vertex num of object's total frame
			int currFrame;			// current active frame of object, 0 if numFrames = 1

			Vertex* vListLocal;		// store local vertex of active frame
			Vertex* vListTrans;		// store transformed vertex of active frame

			Vertex* vListLocalHead;	// for total vertex of object
			Vertex* vListTransHead;

			Vector2f* coordlist;		// texture coord list
			Texture*  texture;

			// Poly list come from Model files
			int numPolys;			// poly num of object
			Poly* pList;

			Object();

			~Object();

			void Init(int numVertices, int numPolys, int numFrames);

			void InitVertices(int numVertices, int numFrames);

			void InitPolys(int numPolys);

			void InitCoordList(int numCoords);
			//////////////////////////////////////////////////////////////////////////

			void SetFrame(int frame);

			void Destroy();

			void Translate(const Vector4f& pos);

			void Rotate(float angleX, float angleY, float angleZ, bool allFrames);

			void Scale(const Vector4f& scale, bool allFrames);

			void UpdateRadius(bool allFrames = true);

			void ComputeVertexNormals();

			// Use m to transform obj
			// transformBase: true for transform ux,uy,uz of object
			void Transform(const Matrix44f& m, int transform, bool transformBase, bool allFrames);

			void ModelToWorld(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS, bool allFrames = false);

			void ModelToWorldMatrix(const Vector4f& worldPos, int transform = TRANSFORM_LOCAL_TO_TRANS, bool allFrames = false);

			void WorldToCamera(const Camera& camera);

			bool Cull(const Camera& camera, const Vector4f& worldPos, int cullFlag);

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
			void RenderWire(Device& device) const;

			void RenderSolid(Device& device) const;

			void RenderGouraud(Device& device) const;

			void RenderTexture(Device& device, Texture* texture) const;
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

			void ClipPolys(const Camera& camera, int clipFlag);

			void CameraToPerspective(const Camera& camera);

			void CameraToPerspectiveMatrix(const Camera& camera);

			void ConvertFromHomogeneous();

			void PerspectiveToScreen(const Camera& camera);

			void CameraToScreen(const Camera& camera);

			// Lighting
			void Lighting(const Camera& camera, const std::vector<Light>& lights);

			// Render
			void RenderWire(Device& device) const;

			void RenderSolid(Device& device) const;

			void RenderGouraud(Device& device) const;

			void RenderTexture(Device& device, Texture* texture) const;
		};

		// Transformation & Lighting
		class TLBase
		{
		public:
			static void Lighting(const Camera& camera, const std::vector<Light>& lights, Poly& poly);

			static void Lighting(const Camera& camera, const std::vector<Light>& lights, PolyFace& polyFace);

			static void SetLighting(bool lighting) { s_isLighting = lighting; }
		private:
			static void InternalLighting(const Camera& camera, const std::vector<Light>& lights, int shadeType, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2);

			static void InfiniteLightLit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& n);

			static void PointLightLit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n);

			static void SpotLight1Lit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n);

			static void SpotLight2Lit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n);

			static bool s_isLighting;
		};
	}
}


#endif