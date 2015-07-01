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

using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		class Camera;
		class Device;
		class Light;

		struct Vertex
		{
			enum Attr
			{
				Attr_None	= 0x0,
				Attr_Point	= 0x1,
				Attr_Normal	= 0x2,
				Attr_Texture= 0x4,
			};

			union
			{
				float m[12];

				struct  
				{
					float x, y, z, w;		// point
					float nx, ny, nz, nw;	// normal
					float u0, v0;			// texture coord

					float i;	// light color
					int attr;	// Attr
				};

				struct
				{
					Vector4f v;		// the vertex
					Vector4f n;		// the normal
					Vector2f uv0;	// texture coord
				};
			};
		};

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

		// using outer vertex list
		struct Poly
		{
			int state;			// state status
			int attr;
	
			Color color;		// the color of poly
			Color litColor[3];	// store lit color. for constant color, the first store it
								// for gouraud color, store 3 vertex colors
			Texture* texture;
			int materialId;

			Vertex* vlist;		// vertex list
			int vert[3];		// index of vertex

			// why do not store coord in Vertex???

			Vector2f* clist;	// texture coord list
			int coord[3];		// index of texture coord list

			float nlength;		// length of normal vector
		};

		// using inner vertex list
		struct PolyFace
		{
			int state;			// state status
			int attr;

			Color color;		// the color of poly
			Color litColor[3];	// store lit color. for constant color, the first store it
								// for gouraud color, store 3 vertex colors
			Texture* texture;
			int materialId;

			Vector4f normal;	// normal
			float nlength;		// length of normal vector

			float avg_z;		// avg of z, for simple sort

			Vertex vlist[3];	// vertex of triangle
			Vertex tlist[3];	// vertex of transformed

			PolyFace* prev;
			PolyFace* next;

			PolyFace& operator=(const Poly& poly)
			{
				attr = poly.attr;
				state = poly.state;
				color = poly.color;

				litColor[0] = poly.litColor[0];
				litColor[1] = poly.litColor[1];
				litColor[2] = poly.litColor[2];
				
				texture = poly.texture;
				materialId = poly.materialId;
				nlength = poly.nlength;

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
			enum { MAX_VERTICES = 2048, MAX_PLOYS = 2048 };
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

			void SetFrame(int frame);

			void Destroy();

			void Translate(const Vector4f& pos);

			void Rotate(float angleX, float angleY, float angleZ, bool allFrames);

			void Scale(const Vector4f& scale, bool allFrames);

			void UpdateRadius(bool allFrames = true);

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

			static void Lighting(const Camera& camera, const std::vector<Light>& lights, PolyFace& polyFace);
		private:
			static void InternalLighting(Color* litColor, const Camera& camera, const std::vector<Light>& lights, int shadeType, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, const Color& color);
		};
	}
}


#endif