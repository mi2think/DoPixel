/********************************************************************
	created:	2014/07/01
	created:	1:7:2014   1:07
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpObject.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpObject
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Object
*********************************************************************/
#include "DpCamera.h"
#include "DpObject.h"
#include "DpColor.h"
#include "DpDevice.h"
#include "DpLight.h"
#include "DpCore.h"
#include "DpDevice.h"

#include <cassert>
#include <map>

using namespace dopixel::math;

namespace dopixel
{
	namespace core
	{
		Object::Object()
			: id(0)
			, state(STATE_VISIBLE | STATE_ACTIVE)
			, attr(0)
			, avgRadius(nullptr)
			, maxRadius(nullptr)
			, materialId(0)
			, numVertices(0)
			, numFrames(1)
			, totalVertices(0)
			, currFrame(0)
			, vListLocal(nullptr)
			, vListTrans(nullptr)
			, vListLocalHead(nullptr)
			, vListTransHead(nullptr)
			, coordlist(nullptr)
			, texture(nullptr)
			, numPolys(0)
			, pList(nullptr)
		{

		}

		Object::~Object()
		{
			Destroy();
		}

		void Object::Init(int numVertices, int numPolys, int numFrames)
		{
			// we just might need this function if we manually want to build an object???
			InitVertices(numVertices, numFrames);

			InitPolys(numPolys);

			InitCoordList(numPolys * 3);

			SetFrame(0);
		}

		void Object::InitVertices(int numVertices, int numFrames)
		{
			SAFE_DELETEARRAY(vListLocalHead);
			vListLocal = nullptr;

			SAFE_DELETEARRAY(vListTransHead);
			vListTrans = nullptr;

			SAFE_DELETEARRAY(avgRadius);
			SAFE_DELETEARRAY(maxRadius);

			this->numVertices = numVertices;
			this->numFrames = numFrames;
			this->totalVertices = this->numVertices * this->numFrames;

			vListLocalHead = new Vertex[totalVertices];
			vListTransHead = new Vertex[totalVertices];
			memset(vListLocalHead, 0, sizeof(Vertex) * totalVertices);
			memset(vListTransHead, 0, sizeof(Vertex) * totalVertices);

			avgRadius = new float[numFrames];
			maxRadius = new float[numFrames];
			memset(avgRadius, 0, sizeof(float) * numFrames);
			memset(maxRadius, 0, sizeof(float) * numFrames);

			SetFrame(0);
		}

		void Object::InitPolys(int numPolys)
		{
			SAFE_DELETEARRAY(pList);

			pList = new Poly[numPolys];
			memset(pList, 0, sizeof(Poly) * numPolys);
		}

		void Object::InitCoordList(int numCoords)
		{
			SAFE_DELETEARRAY(coordlist);

			coordlist = new Vector2f[numCoords];
		}

		void Object::SetFrame(int frame)
		{
			if ((attr & ATTR_MULTI_FRAME) == 0 && frame != 0)
				return;

			if (frame < 0 || frame >= numFrames) // start with 0
				return;

			currFrame = frame;
			vListLocal = &vListLocalHead[numVertices * frame];
			vListTrans = &vListTransHead[numVertices * frame];
		}

		void Object::Destroy()
		{
			SAFE_DELETEARRAY(vListLocalHead);
			vListLocal = nullptr;

			SAFE_DELETEARRAY(vListTransHead);
			vListTrans = nullptr;

			SAFE_DELETEARRAY(avgRadius);
			SAFE_DELETEARRAY(maxRadius);

			SAFE_DELETEARRAY(pList);

			SAFE_DELETEARRAY(coordlist);
		}

		void Object::Translate(const Vector4f& pos)
		{
			// simply updates the world_pos
			worldPos += pos;
		}

		void Object::Rotate(float angleX, float angleY, float angleZ, bool allFrames)
		{
			Matrix44f m;
			MatrixRotationXYZ(m, angleX, angleY, angleZ);

			if (allFrames)
			{
				// now simply rotate each point of the mesh in local/model coordinates
				for (int i = 0; i < totalVertices; ++i)
				{
					vListLocalHead[i].p *= m;
					if ((vListLocalHead[i].attr & Vertex::Attr_Normal) != 0)
					{
						vListLocalHead[i].n *= m;
					}
				}
			}
			else
			{
				for (int i = 0; i < numVertices; ++i)
				{
					vListLocal[i].p *= m;
					if ((vListLocal[i].attr & Vertex::Attr_Normal) != 0)
					{
						vListLocal[i].n *= m;
					}
				}
			}
		}

		void Object::Scale(const Vector4f& scale, bool allFrames)
		{
			if (allFrames)
			{
				for (int i = 0; i < totalVertices; ++i)
				{
					vListLocalHead[i].x *= scale.x;
					vListLocalHead[i].y *= scale.y;
					vListLocalHead[i].z *= scale.z;
					// leave w unchanged, always equal to 1
				}
			}
			else
			{
				for (int i = 0; i < numVertices; ++i)
				{
					vListLocal[i].x *= scale.x;
					vListLocal[i].y *= scale.y;
					vListLocal[i].z *= scale.z;
					// leave w unchanged, always equal to 1
				}
			}

			// recompute radius
			UpdateRadius(allFrames);
		}

		void Object::UpdateRadius(bool allFrames)
		{
			auto fn = [](float& avgR, float& maxR, Vertex* p, int count)
			{
				avgR = 0;
				maxR = 0;
				
				float totalRadius = 0;
				for (int i = 0; i < count; ++i)
				{
					float distVertex = p[i].p.Length();
					if (distVertex >= maxR)
						maxR = distVertex;
					totalRadius += distVertex;
				}
				avgR = totalRadius / count;
			};

			if (allFrames)
			{
				Vertex* p = vListLocalHead;
				for (int i = 0; i < numFrames; ++i, p += numVertices)
				{
					fn(avgRadius[i], maxRadius[i], p, numVertices);
				}
			}
			else
			{
				fn(avgRadius[currFrame], maxRadius[currFrame], vListLocal, numVertices);
			}
		}

		void Object::ComputeVertexNormals()
		{
			// vertex normals of each poly are commonly used for lighting compute for gouraud shading.
			// we want to average the normals of all poly that are adjacent to a given vertex

			// k: vertex index
			// v: poly index which share the same vertex 
			std::map<int, std::vector<int>> vertexSharedByPolys;

			// for each poly, find vertex shared by poly
			for (int i = 0; i < numPolys; ++i)
			{
				Poly& poly = pList[i];
				if ((poly.attr & POLY_ATTR_SHADE_GOURAUD) != 0)
				{
					// vertex index - shared poly index
					int i0 = poly.vert[0];
					int i1 = poly.vert[1];
					int i2 = poly.vert[2];

					vertexSharedByPolys[i0].push_back(i);
					vertexSharedByPolys[i1].push_back(i);
					vertexSharedByPolys[i2].push_back(i);
				}
			}

			if (vertexSharedByPolys.empty())
				return;

			// for each vertex, compute vertex average normal
			for (int i = 0; i < totalVertices; ++i)
			{
				auto it = vertexSharedByPolys.find(i);
				if (it == vertexSharedByPolys.end())
					continue;

				auto& polyIndexList = vertexSharedByPolys[i];
				assert(polyIndexList.size() > 0);

				Vector4f n;
				for (unsigned int j = 0; j < polyIndexList.size(); ++j)
				{
					Poly& polyj = pList[polyIndexList[j]];
					Vector4f nj = polyj.GetFacetNormal();
					n += nj;
				}

				if (polyIndexList.size() > 1)
				{
					n /= polyIndexList.size();
				}
				n.Normalize();
				vListLocal[i].n = n;
			}
		}

		void Object::Transform(const Matrix44f& m, int transform, bool transformBase, bool allFrames)
		{
			auto fnTransfrom = [&m](Vertex* v, int count)
			{
				for (int i = 0; i < count; ++i, ++v)
				{
					v->p *= m;
					if ((v->attr & Vertex::Attr_Normal) != 0)
						v->n *= m;
				}
			};

			auto fnTransfromTo = [&m](Vertex* v1, Vertex* v2, int count)
			{
				for (int i = 0; i < count; ++i, ++v1, ++v2)
				{
					// copy
					*v2 = *v1;
					// update position
					v2->p = v1->p * m;
					if ((v1->attr & Vertex::Attr_Normal) != 0)
						v2->n = v1->n * m;
				}
			};

			if (allFrames)
			{
				switch (transform)
				{
				case TRANSFORM_LOCAL_ONLY:
					fnTransfrom(vListLocalHead, totalVertices);
					break;
				case TRANSFORM_TRANS_ONLY:
					fnTransfrom(vListTransHead, totalVertices);
					break;
				case TRANSFORM_LOCAL_TO_TRANS:
					fnTransfromTo(vListLocalHead, vListTransHead, totalVertices);
					break;
				}
			}
			else
			{
				// for active frame
				switch (transform)
				{
				case TRANSFORM_LOCAL_ONLY:
					fnTransfrom(vListLocal, numVertices);
					break;
				case TRANSFORM_TRANS_ONLY:
					fnTransfrom(vListTrans, numVertices);
					break;
				case TRANSFORM_LOCAL_TO_TRANS:
					fnTransfromTo(vListLocal, vListTrans, numVertices);
					break;
				}
			}

			if (transformBase)
			{
				ux *= m;
				uy *= m;
				uz *= m;
			}
		}

		void Object::ModelToWorld(const Vector4f& worldPos, int transform, bool allFrames)
		{
			assert(transform == TRANSFORM_LOCAL_TO_TRANS || transform == TRANSFORM_TRANS_ONLY);

			// no need to transform vertex normals, they are invariant of position

			this->worldPos = worldPos;

			if (allFrames)
			{
				if (transform == TRANSFORM_LOCAL_TO_TRANS)
				{
					for (int i = 0; i < totalVertices; ++i)
					{
						// copy
						vListTransHead[i] = vListLocalHead[i];
						// update position
						vListTransHead[i].p = vListLocalHead[i].p + worldPos;
					}
				}
				else
				{
					for (int i = 0; i < totalVertices; ++i)
					{
						vListTransHead[i].p += worldPos;
					}
				}
			}
			else
			{
				if (transform == TRANSFORM_LOCAL_TO_TRANS)
				{
					for (int i = 0; i < numVertices; ++i)
					{
						// copy
						vListTransHead[i] = vListLocalHead[i];
						// update position
						vListTransHead[i].p = vListLocalHead[i].p + worldPos;
					}
				}
				else
				{
					for (int i = 0; i < numVertices; ++i)
					{
						vListTrans[i].p += worldPos;
					}
				}
			}

		}

		void Object::ModelToWorldMatrix(const Vector4f& worldPos, int transform, bool allFrames)
		{
			this->worldPos = worldPos;
			Matrix44f m(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				worldPos.x, worldPos.y, worldPos.z, 1);
			Transform(m, transform, true, allFrames);
			// TODO: need copy other attrs from local to trans
		}

		void Object::WorldToCamera(const Camera& camera)
		{
			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i].p *= camera.matrixCamera;
			}
		}

		bool Object::Cull(const Camera& camera, const Vector4f& worldPos, int cullFlag)
		{
			// Try to cull object by the world pos of object and the camera clip plane

			Vector4f spherePos = worldPos;
			spherePos *= camera.matrixCamera;

			// only the current frame matters for culling
			float _maxRadius = maxRadius[currFrame];

			// Cull Z
			if (cullFlag & CULL_PLANE_Z)
			{
				if (spherePos.z + _maxRadius < camera.nearClipZ || spherePos.z - _maxRadius > camera.farClipZ)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull X
			if (cullFlag & CULL_PLANE_X)
			{
				float xtest = 0.5f * spherePos.z * camera.viewPlaneWidth / camera.viewDist;
				if (spherePos.x - _maxRadius > xtest || spherePos.x + _maxRadius < -xtest)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull Y
			if (cullFlag & CULL_PLANE_Y)
			{
				float ytest = 0.5f * spherePos.z *  camera.viewPlaneHeight / camera.viewDist;
				if (spherePos.y - _maxRadius > ytest || spherePos.y + _maxRadius < -ytest)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			return false;
		}

		void Object::ResetCull()
		{
			state &= (~STATE_CULLED);

			for (int i = 0; i < numPolys; ++i)
			{
				Poly* p = &pList[i];
				if (! p || ! (p->state & POLY_STATE_ACTIVE))
					continue;

				p->ResetCull();
			}
		}

		void Object::RemoveBackfaces(const Camera& camera)
		{
			if (state & STATE_CULLED)
				return;

			for (int i = 0; i < numPolys; ++i)
			{
				Poly* p = &pList[i];
				if (! p || !(p->state & POLY_STATE_ACTIVE))
					continue;;
				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED) || (p->attr & POLY_ATTR_2SIDE))
					continue;

				// Vertex index
				int vi0 = p->vert[0];
				int vi1 = p->vert[1];
				int vi2 = p->vert[2];

				// Calc Poly face normal vector
				// u:p0->p1, v:p0->p2, n: uxv
				Vector4f u = vListTrans[vi1].p - vListTrans[vi0].p;
				Vector4f v = vListTrans[vi2].p - vListTrans[vi0].p;
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - vListTrans[vi0].p;
				float dp = DotProduct(view, n);
				if (dp <= 0.0f)
					p->state |= POLY_STATE_BACKFACE;
			}
		}

		void Object::CameraToPerspective(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			float k = 1.0f / tan(angle2radian(camera.fov * 0.5f));

			for (int i = 0; i < numVertices; ++i)
			{
				float z = vListTrans[i].z;
				assert(z != 0);
				
				// First we need transform to projection plane, (x, y) -> (x', y')
				// like this:
				//vListTrans[i].x = vListTrans[i].x * camera.viewDist / z;
				//vListTrans[i].y = vListTrans[i].y * camera.viewDist * camera.aspectRatio / z;
				
				// Then we get x:[-1, 1], y:[-1.0/ar, 1.0/ar], we need scale it to x:[-1, 1], y:[-1,1]
				// (x', y') -> (x'', y'')
				// Assume projection height is H, and the result height is 2, So:
				// y' / y'' = H / 2      -> y'' = y' / (H / 2)
				// x' / x'' = H * ar / 2 -> x'' = x' / (H * ar / 2)

				vListTrans[i].x = vListTrans[i].x * k / (camera.aspectRatio * z);
				vListTrans[i].y = vListTrans[i].y * k / z;
			}
		}

		void Object::CameraToPerspectiveMatrix(const Camera& camera)
		{
			Matrix44f m;
			camera.BuildCameraToPerspective(m);
			Transform(m, TRANSFORM_TRANS_ONLY, true, false);
		}

		void Object::ConvertFromHomogeneous()
		{
			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i].p /= vListTrans[i].w;
			}
		}

		void Object::PerspectiveToScreen(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			// Assume the coordinate of object is normalied, range is [-1,1]
			// Then Scale it by viewport, and reverse Y axis
			
			// Results: 
			// x: [0, viewportWidth]
			// y: [0, viewportHeight]

			// So:
			// x -> (x + 1) * viewportWidth / 2
			// y -> viewportHeight - (y + 1) * viewportHeight / 2
			// the results is same with book "Introduction to 3D Game Programming with DirectX 9.0". :)

			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i].x = (vListTrans[i].x + 1) * camera.viewportWidth / 2;
				vListTrans[i].y = (1 - vListTrans[i].y) * camera.viewportHeight / 2;

			}
		}

		void Object::CameraToScreen(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			float k = 1.0f / tan(angle2radian(camera.fov * 0.5f));

			for (int i = 0; i < numVertices; ++i)
			{
				// First, camera to perspective
				float z = vListTrans[i].z;
				vListTrans[i].x = vListTrans[i].x * k / (camera.aspectRatio * z);
				vListTrans[i].y = vListTrans[i].y * k / z;
				
				// See PerspectiveToScreen
				vListTrans[i].x = (vListTrans[i].x + 1) * camera.viewportWidth / 2;
				vListTrans[i].y = (1 - vListTrans[i].y) * camera.viewportHeight / 2;
			}
		}

		void Object::Lighting(const Camera& camera, const std::vector<Light>& lights)
		{
			if (!(state & Object::STATE_ACTIVE) || (state & Object::STATE_CULLED) || !(state & Object::STATE_VISIBLE))
				return;

			// Lighting each poly
			for (int i = 0; i < numPolys; ++i)
			{
				Poly* poly = &pList[i];
				poly->vlist = vListTrans;
	
				TLBase::Lighting(camera, lights, *poly);

				poly->vlist = vListLocal;
			}
		}

		void Object::RenderWire(Device& device) const
		{
			if (state & STATE_CULLED)
				return;

			device.SetRenderState(RS_FillMode, Fill_Wireframe);

			for (int i = 0; i < numPolys; ++i)
			{
				const Poly* p = &pList[i];
				if (! p || !(p->state & POLY_STATE_ACTIVE))
					continue;;
				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED))
					continue;
				
				int i0 = p->vert[0];
				int i1 = p->vert[1];
				int i2 = p->vert[2];

				device.DrawTriangle(vListTrans[i0], vListTrans[i1], vListTrans[i2]);
			}
		}

		void Object::RenderSolid(Device& device) const
		{
			if (state & STATE_CULLED)
				return;

			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetRenderState(RS_ShadeMode, Shade_Flat);

			for (int i = 0; i < numPolys; ++i)
			{
				const Poly* p = &pList[i];

				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED))
					continue;

				int i0 = p->vert[0];
				int i1 = p->vert[1];
				int i2 = p->vert[2];

				device.DrawTriangle(vListTrans[i0], vListTrans[i1], vListTrans[i2]);
			}
		}

		void Object::RenderGouraud(Device& device) const
		{
			if (state & STATE_CULLED)
				return;

			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetRenderState(RS_ShadeMode, Shade_Gouraud);

			for (int i = 0; i < numPolys; ++i)
			{
				const Poly* p = &pList[i];

				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED))
					continue;

				int i0 = p->vert[0];
				int i1 = p->vert[1];
				int i2 = p->vert[2];

				device.DrawTriangle(vListTrans[i0], vListTrans[i1], vListTrans[i2]);
			}
		}

		void Object::RenderTexture(Device& device, Texture* texture) const
		{
			if (state & STATE_CULLED)
				return;

			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetTexture(texture);

			for (int i = 0; i < numPolys; ++i)
			{
				const Poly* p = &pList[i];

				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED))
					continue;

				int i0 = p->vert[0];
				int i1 = p->vert[1];
				int i2 = p->vert[2];

				device.DrawTriangle(vListTrans[i0], vListTrans[i1], vListTrans[i2]);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		bool RenderList::InsertPolyFace(const PolyFace& polyFace)
		{
			assert(numPolyFaces < MAX_PLOYS );
			if (numPolyFaces >= MAX_PLOYS)
				return false;

			PolyFace& pf = this->polyFace[numPolyFaces];
			pf = polyFace;

			pPolyFace[numPolyFaces] = &pf;

			if (numPolyFaces == 0)
			{
				pf.prev = nullptr;
				pf.next = nullptr;
			}
			else
			{
				pf.prev = pPolyFace[numPolyFaces - 1];
				pf.next = nullptr;
				pf.prev->next = &pf;
			}

			++numPolyFaces;
			return true;
		}

		bool RenderList::InsertPoly(const Poly& poly)
		{
			assert(numPolyFaces < MAX_PLOYS );
			if (numPolyFaces >= MAX_PLOYS)
				return false;

			// Covert poly to poly face, then insert into render list
			PolyFace& pf = polyFace[numPolyFaces];
			pPolyFace[numPolyFaces] = &pf;
			
			// using operator= to assignment Poly to PolyFace
			pf = poly;

			if (numPolyFaces == 0)
			{
				pf.prev = nullptr;
				pf.next = nullptr;
			}
			else
			{
				pf.prev = pPolyFace[numPolyFaces - 1];
				pf.next = nullptr;
				pf.prev->next = &pf;
			}

			++numPolyFaces;
			return true;
		}

		bool RenderList::InsertObject(const Object& object, int insertLocal)
		{
			// Insert poly faces of object into render list, which poly face is visible and active

			if (! (object.state & Object::STATE_ACTIVE) || ! (object.state & Object::STATE_VISIBLE) || (object.state & Object::STATE_CULLED))
				return false;

			for (int i = 0; i < object.numPolys; ++i)
			{
				Poly* poly = const_cast<Poly*>(&object.pList[i]);
				if (! (poly->state & POLY_STATE_ACTIVE) || (poly->state & POLY_STATE_BACKFACE) || (poly->state & POLY_STATE_CLIPPED) )
					continue;

				// We copy the vertex list which poly pointed to render list
				// Don't change the object, restore the data src for once inserted
				Vertex* oldList = poly->vlist;

				if (insertLocal)
					poly->vlist = const_cast<Vertex*>(object.vListLocal);
				else
					poly->vlist = const_cast<Vertex*>(object.vListTrans);

				if (! InsertPoly(*poly))
				{
					poly->vlist = oldList;
					return false;
				}
				poly->vlist = oldList;
			}

			return true;
		}

		// Use m to transform render list
		void RenderList::Transform(const Matrix44f& m, int transform)
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					switch (transform)
					{
					case TRANSFORM_LOCAL_ONLY:
						pf->vlist[j].p *= m;
						if ((pf->vlist[j].attr & Vertex::Attr_Normal) != 0)
							pf->vlist[j].n *= m;
						break;
					case TRANSFORM_TRANS_ONLY:
						pf->tlist[j].p *= m;
						if ((pf->tlist[j].attr & Vertex::Attr_Normal) != 0)
							pf->tlist[j].n *= m;
						break;
					case TRANSFORM_LOCAL_TO_TRANS:
						pf->tlist[j].p = pf->vlist[j].p * m;
						if ((pf->vlist[j].attr & Vertex::Attr_Normal) != 0)
							pf->tlist[j].n = pf->vlist[j].n * m;
						break;
					default:
						break;
					}
				}
			}
		}

		void RenderList::ModelToWorld(const Vector4f& worldPos, int transform)
		{
			assert(transform == TRANSFORM_LOCAL_TO_TRANS || transform == TRANSFORM_TRANS_ONLY);

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					if (transform == TRANSFORM_LOCAL_TO_TRANS)
						pf->tlist[j].p = pf->vlist[j].p + worldPos;
					else
						pf->tlist[j].p += worldPos;
				}
			}
		}

		void RenderList::ModelToWorldMatrix(const Vector4f& worldPos, int transform)
		{
			Matrix44f m(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						worldPos.x, worldPos.y, worldPos.z, 1);
			Transform(m, transform);
		}

		void RenderList::WorldToCamera(const Camera& camera)
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					pf->tlist[j].p *= camera.matrixCamera;
				}
			}
		}

		void RenderList::SortByZ(int sortType /* = SORT_AvgZ */)
		{
			auto fnSort = [&sortType](const PolyFace* p1, const PolyFace* p2) -> bool
			{
				float z1 = 0.0f;
				float z2 = 0.0f;
				switch (sortType)
				{
				case RenderList::SORT_AvgZ:
					z1 = (0.333333f) * (p1->tlist[0].z + p1->tlist[1].z + p1->tlist[2].z);
					z2 = (0.333333f) * (p2->tlist[0].z + p2->tlist[1].z + p2->tlist[2].z);
					break;
				case RenderList::SORT_NearZ:
					z1 = MIN(MIN(p1->tlist[0].z, p1->tlist[1].z), p1->tlist[2].z);
					z2 = MIN(MIN(p2->tlist[0].z, p2->tlist[1].z), p2->tlist[2].z);
					break;
				case RenderList::SORT_FarZ:
					z1 = MAX(MAX(p1->tlist[0].z, p1->tlist[1].z), p1->tlist[2].z);
					z2 = MAX(MAX(p2->tlist[0].z, p2->tlist[1].z), p2->tlist[2].z);
					break;
				}

				if (z1 != z2)
					return z1 > z2;

				return false;
			};

			std::sort(pPolyFace, pPolyFace + numPolyFaces, fnSort);
		}

		void RenderList::RemoveBackfaces(const Camera& camera)
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE) || (pf->attr & POLY_ATTR_2SIDE))
					continue;
				
				// Calc Poly face normal vector
				Vector4f u = pf->tlist[1].p - pf->tlist[0].p;
				Vector4f v = pf->tlist[2].p - pf->tlist[0].p;
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - pf->tlist[0].p;

				float dp = DotProduct(view, n);
				if (dp <= 0.0f)
					pf->state |= POLY_STATE_BACKFACE;
			}
		}

		void RenderList::ClipPolys(const Camera& camera, int clipFlag)
		{
			// using near/far z clip plane for clip poly, also using left/right, top/bottom clip plane for
			// simple accept or refuse test
			// in camera space

			enum ClipCode 
			{
				CLIP_CODE_GZ	= 0x1,	// z > z_max
				CLIP_CODE_LZ	= 0x2,	// z < z_min
				CLIP_CODE_IZ	= 0x4,	// z_min =< z =< z_max

				CLIP_CODE_GX	= 0x1,	// x > x_max
				CLIP_CODE_LX	= 0x2,	// x < x_min
				CLIP_CODE_IX	= 0x4,	// x_min =< x =< x_max

				CLIP_CODE_GY	= 0x1,	// y > y_max
				CLIP_CODE_LY	= 0x2,	// y < y_min
				CLIP_CODE_IY	= 0x4,	// y_min =< y =< y_max

				CLIP_CODE_NULL	= 0x0,
			};

			int vertexCode[3] = { 0 };

			// for each poly in list, clip or cull it
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				Vertex& v0 = pf->tlist[0];
				Vertex& v1 = pf->tlist[1];
				Vertex& v2 = pf->tlist[2];

				// cull poly by left/right clip plane
				if ((clipFlag & CULL_PLANE_X) != 0)
				{
					// tan(theta / 2) = (w / 2) / h = |x| / z
					// => |x| = (w / 2) * z / h

					float z_factor = 0.5f * camera.viewPlaneWidth / camera.viewPlaneHeight;

					// vertex 0
					float z_test = z_factor * v0.z;
					if (v0.x > z_test)
						vertexCode[0] = CLIP_CODE_GX;
					else if (v0.x < -z_test)
						vertexCode[0] = CLIP_CODE_LX;
					else
						vertexCode[0] = CLIP_CODE_IX;

					// vertex 1
					z_test = z_factor * v1.z;
					if (v1.x > z_test)
						vertexCode[1] = CLIP_CODE_GX;
					else if (v1.x < -z_test)
						vertexCode[1] = CLIP_CODE_LX;
					else
						vertexCode[1] = CLIP_CODE_IX;

					// vertex 2
					z_test = z_factor * v2.z;
					if (v2.x > z_test)
						vertexCode[2] = CLIP_CODE_GX;
					else if (v2.x < -z_test)
						vertexCode[2] = CLIP_CODE_LX;
					else
						vertexCode[2] = CLIP_CODE_IX;
				
					// simple refuse test
					if (vertexCode[0] == CLIP_CODE_GX && vertexCode[1] == CLIP_CODE_GX && vertexCode[2] == CLIP_CODE_GX
						|| vertexCode[0] == CLIP_CODE_LX && vertexCode[1] == CLIP_CODE_LX && vertexCode[2] == CLIP_CODE_LX)
					{
						pf->state |= POLY_STATE_CLIPPED;
						continue;
					}
				}

				// cull poly by top/bottom clip plane
				if ((clipFlag & CULL_PLANE_Y) != 0)
				{
					// tan(theta / 2) = (w / 2) / h = |y| / z
					// => |y| = (w / 2) * z / h

					float z_factor = 0.5f * camera.viewPlaneWidth / camera.viewPlaneHeight;

					// vertex 0
					float z_test = z_factor * v0.z;
					if (v0.y > z_test)
						vertexCode[0] = CLIP_CODE_GY;
					else if (v0.y < -z_test)
						vertexCode[0] = CLIP_CODE_LY;
					else
						vertexCode[0] = CLIP_CODE_IY;

					// vertex 1
					z_test = z_factor * v1.z;
					if (v1.y > z_test)
						vertexCode[1] = CLIP_CODE_GY;
					else if (v1.y < -z_test)
						vertexCode[1] = CLIP_CODE_LY;
					else
						vertexCode[1] = CLIP_CODE_IY;

					// vertex 2
					z_test = z_factor * v2.z;
					if (v2.y > z_test)
						vertexCode[2] = CLIP_CODE_GY;
					else if (v2.y < -z_test)
						vertexCode[2] = CLIP_CODE_LY;
					else
						vertexCode[2] = CLIP_CODE_IY;

					// simple refuse test
					if (vertexCode[0] == CLIP_CODE_GY && vertexCode[1] == CLIP_CODE_GY && vertexCode[2] == CLIP_CODE_GY
						|| vertexCode[0] == CLIP_CODE_LY && vertexCode[1] == CLIP_CODE_LY && vertexCode[2] == CLIP_CODE_LY)
					{
						pf->state |= POLY_STATE_CLIPPED;
						continue;
					}
				}

				// clip or cull poly by near/far clip plane
				if ((clipFlag & CULL_PLANE_Z) != 0)
				{
					// vertex in frustum
					int numVertexIn = 0;
					
					// vertex 0
					if (v0.z > camera.farClipZ)
						vertexCode[0] = CLIP_CODE_GZ;
					else if (v0.z < camera.nearClipZ)
						vertexCode[0] = CLIP_CODE_LZ;
					else
					{
						vertexCode[0] = CLIP_CODE_IZ;
						++numVertexIn;
					}

					// vertex 1
					if (v1.z > camera.farClipZ)
						vertexCode[1] = CLIP_CODE_GZ;
					else if (v1.z < camera.nearClipZ)
						vertexCode[1] = CLIP_CODE_LZ;
					else
					{
						vertexCode[1] = CLIP_CODE_IZ;
						++numVertexIn;
					}

					// vertex 2
					if (v2.z > camera.farClipZ)
						vertexCode[2] = CLIP_CODE_GZ;
					else if (v2.z < camera.nearClipZ)
						vertexCode[2] = CLIP_CODE_LZ;
					else
					{
						vertexCode[2] = CLIP_CODE_IZ;
						++numVertexIn;
					}

					// simple refuse test
					if (vertexCode[0] == CLIP_CODE_GZ && vertexCode[1] == CLIP_CODE_GZ && vertexCode[2] == CLIP_CODE_GZ
						|| vertexCode[0] == CLIP_CODE_LZ && vertexCode[1] == CLIP_CODE_LZ && vertexCode[2] == CLIP_CODE_LZ)
					{
						pf->state |= POLY_STATE_CLIPPED;
						continue;
					}

					// test if any vertex outside the near z clip plane
					if (((vertexCode[0] | vertexCode[1] | vertexCode[2]) & CLIP_CODE_IZ) != 0)
					{
						int vindex0 = 0;
						int vindex1 = 1;
						int vindex2 = 2;

						if (numVertexIn == 1)
						{
							// first, find the inner vertex, and assign it to _v0
							if (vertexCode[0] == CLIP_CODE_IZ)
								;
							else if (vertexCode[1] == CLIP_CODE_IZ)
							{
								vindex0 = 1;
								vindex1 = 2;
								vindex2 = 0;
							}
							else
							{
								vindex0 = 2;
								vindex1 = 0;
								vindex2 = 1;
							}
							Vertex* _v0 = &(pf->tlist[vindex0]);
							Vertex* _v1 = &(pf->tlist[vindex1]);
							Vertex* _v2 = &(pf->tlist[vindex2]);

							// second, clip edge of poly
							// p = v0.p + v01.p * t
							
							// clip v0 -> v1
							Vector4f v01 = pf->tlist->p - _v0->p;
							float t1 = (camera.nearClipZ - _v0->z) / v01.z;
							Vertex vt1 = _v0->Interpolate(*_v1, t1);

							// clip v0 -> v2
							Vector4f v02 = _v2->p - _v0->p;
							float t2 = (camera.nearClipZ - _v0->z) / v02.z;
							Vertex vt2 = _v0->Interpolate(*_v1, t2);

							// cover old vertex by using clipped vertex
							*_v1 = vt1;
							*_v2 = vt2;
						}
						else if (numVertexIn == 2)
						{
							// first, find outside vertex
							if (vertexCode[0] == CLIP_CODE_LZ)
								;
							else if (vertexCode[1] == CLIP_CODE_LZ)
							{
								vindex0 = 1;
								vindex1 = 2;
								vindex2 = 0;
							}
							else
							{
								vindex0 = 2;
								vindex1 = 0;
								vindex2 = 1;
							}
							Vertex* _v0 = &(pf->tlist[vindex0]);
							Vertex* _v1 = &(pf->tlist[vindex1]);
							Vertex* _v2 = &(pf->tlist[vindex2]);

							// second, clip edge of poly
							// p = v0.p + v01.p * t

							// clip v0 -> v1
							Vector4f v01 = _v1->p - _v0->p;
							float t1 = (camera.nearClipZ - _v0->z) / v01.z;
							Vertex vt1 = _v0->Interpolate(*_v1, t1);

							// clip v0 -> v2
							Vector4f v02 = _v2->p - _v0->p;
							float t2 = (camera.nearClipZ - _v0->z) / v02.z;
							Vertex vt2 = _v0->Interpolate(*_v1, t2);

							// cover v0 by vt1
							*_v0 = vt1;

							// generate a new poly, then insert end of render list
							PolyFace newPolyFace = *pf;
							// cover v1 by vt1
							// cover v0 by vt2
							newPolyFace.tlist[vindex1] = vt1;
							newPolyFace.tlist[vindex0] = vt2;
							newPolyFace.tlist[vindex2] = *_v2;
							
							InsertPolyFace(newPolyFace);
						}
					}
				}
			}
		}

		void RenderList::CameraToPerspective(const Camera& camera)
		{
			// Assume the poly in render list has been transformed to camera,
			// And stored in tlist

			float k = 1.0f / tan(angle2radian(camera.fov * 0.5f));

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					float z = pf->tlist[j].z;
					assert(z != 0);

					// First we need transform to projection plane, (x, y) -> (x', y')
					// like this:
					//pf->tlist[j].x = pf->tlist[j].x * camera.viewDist / z;
					//pf->tlist[j].y = pf->tlist[j].y * camera.viewDist * camera.aspectRatio / z;

					// Then we get x:[-1, 1], y:[-1.0/ar, 1.0/ar], we need scale it to x:[-1, 1], y:[-1,1]
					// (x', y') -> (x'', y'')
					// Assume projection height is H, and the result height is 2, So:
					// y' / y'' = H / 2      -> y'' = y' / (H / 2)
					// x' / x'' = H * ar / 2 -> x'' = x' / (H * ar / 2)

					pf->tlist[j].x = pf->tlist[j].x * k / (camera.aspectRatio * z);
					pf->tlist[j].y = pf->tlist[j].y * k / z;
				}
			}
		}

		void RenderList::CameraToPerspectiveMatrix(const Camera& camera)
		{
			Matrix44f m;
			camera.BuildCameraToPerspective(m);
			Transform(m, TRANSFORM_TRANS_ONLY);
		}

		void RenderList::ConvertFromHomogeneous()
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					pf->tlist[j].p /= pf->tlist[j].w;
				}
			}
		}

		void RenderList::PerspectiveToScreen(const Camera& camera)
		{
			// Assume the coordinate of object is normalized, range is [-1,1]
			// Then Scale it by viewport, and reverse Y axis

			// Results: 
			// x: [0, viewportWidth]
			// y: [0, viewportHeight]

			// So:
			// x -> (x + 1) * viewportWidth / 2
			// y -> viewportHeight - (y + 1) * viewportHeight / 2
			// the results is same with book "Introduction to 3D Game Programming with DirectX 9.0". :)

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					pf->tlist[j].x = (pf->tlist[j].x + 1) * camera.viewportWidth / 2;
					pf->tlist[j].y = (1 - pf->tlist[j].y) * camera.viewportHeight / 2;
				}
			}
		}

		void RenderList::CameraToScreen(const Camera& camera)
		{
			float k = 1.0f / tan(angle2radian(camera.fov * 0.5f));

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					// First, camera to perspective
					float z = pf->tlist[j].z;
					pf->tlist[j].x = pf->tlist[j].x * k / (camera.aspectRatio * z);
					pf->tlist[j].y = pf->tlist[j].y * k / z;

					// see PerspectiveToScreen
					pf->tlist[j].x = (pf->tlist[j].x + 1) * camera.viewportWidth / 2;
					pf->tlist[j].y = (1 - pf->tlist[j].y) * camera.viewportHeight / 2;
				}
			}
		}

		void RenderList::Lighting(const Camera& camera, const std::vector<Light>& lights)
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				TLBase::Lighting(camera, lights, *polyFace);
			}
		}

		void RenderList::RenderWire(Device& device) const
		{
			device.SetRenderState(RS_FillMode, Fill_Wireframe);

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(pf->tlist[0], pf->tlist[1], pf->tlist[2]);
			}
		}

		void RenderList::RenderSolid(Device& device) const
		{
			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetRenderState(RS_ShadeMode, Shade_Flat);

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(pf->tlist[0], pf->tlist[1], pf->tlist[2]);
			}
		}

		void RenderList::RenderGouraud(Device& device) const
		{
			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetRenderState(RS_ShadeMode, Shade_Gouraud);

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(pf->tlist[0], pf->tlist[1], pf->tlist[2]);
			}
		}

		void RenderList::RenderTexture(Device& device, Texture* texture) const
		{
			device.SetRenderState(RS_FillMode, Fill_Solid);
			device.SetTexture(texture);

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(pf->tlist[0], pf->tlist[1], pf->tlist[2]);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		bool TLBase::s_isLighting = false;

		void TLBase::Lighting(const Camera& camera, const std::vector<Light>& lights, Poly& poly)
		{
			poly.state |= POLY_STATE_LIT;

			int i0 = poly.vert[0];
			int i1 = poly.vert[1];
			int i2 = poly.vert[2];

			InternalLighting(camera, lights, poly.attr, poly.vlist[i0], poly.vlist[i1], poly.vlist[i2]);
		}

		void TLBase::Lighting(const Camera& camera, const std::vector<Light>& lights, PolyFace& polyFace)
		{
			polyFace.state |= POLY_STATE_LIT;

			InternalLighting(camera, lights, polyFace.attr, polyFace.tlist[0], polyFace.tlist[1], polyFace.tlist[2]);
		}

		// Now just calc diffuse light model.
		// Rsdiffuse: diffuse color of material, assume every surface has a material
		// Idiffuse: intensity of diffuse color of light.

		void TLBase::InternalLighting(const Camera& camera, const std::vector<Light>& lights, int shadeType, Vertex& vertex0, Vertex& vertex1, Vertex& vertex2)
		{
			const Vector4f& p0 = vertex0.p;
			const Vector4f& p1 = vertex1.p;
			const Vector4f& p2 = vertex2.p;

			if ((shadeType & POLY_ATTR_SHADE_FLAT) != 0)
			{
				// Init color
				float rSum = 0;
				float gSum = 0;
				float bSum = 0;

				// has lighted
				if ((vertex0.attr & Vertex::Attr_Lit) != 0)
					return;

				// Normal vector
				Vector4f u = p1 - p0;
				Vector4f v = p2 - p0;
				Vector4f n = CrossProduct(u, v);
				n.Normalize();

				// lit
				for (const auto& light : lights)
				{
					if (light.state != Light::STATE_ON)
						continue;

					switch (light.attr)
					{
					case Light::ATTR_AMBIENT:
						{
							rSum += (float)light.color.r * vertex0.color.r / 256;
							gSum += (float)light.color.g * vertex0.color.g / 256;
							bSum += (float)light.color.b * vertex0.color.b / 256;
						}
						break;
					case Light::ATTR_INFINITE:
						InfiniteLightLit(rSum, gSum, bSum, vertex0.color, light, n);
						break;
					case Light::ATTR_POINT:
						PointLightLit(rSum, gSum, bSum, vertex0.color, light, p0, n);
						break;
					case Light::ATTR_SPOTLIGHT1:
						SpotLight1Lit(rSum, gSum, bSum, vertex0.color, light, p0, n);
						break;
					case Light::ATTR_SPOTLIGHT2:
						SpotLight2Lit(rSum, gSum, bSum, vertex0.color, light, p0, n);
						break;
					}
				}

				rSum = math::Clamp(rSum, 0.0f, 255.0f);
				gSum = math::Clamp(gSum, 0.0f, 255.0f);
				bSum = math::Clamp(bSum, 0.0f, 255.0f);

				vertex0.litColor = Color((unsigned char)rSum, (unsigned char)gSum, (unsigned char)bSum);
				vertex1.litColor = vertex0.litColor;
				vertex2.litColor = vertex0.litColor;
				
				vertex0.attr |= Vertex::Attr_Lit;
				// since vertex1, vertex2 may also belong to other poly, it may need lit
			}
			else if ((shadeType & POLY_ATTR_SHADE_GOURAUD) != 0)
			{
				const Vector4f& n0 = vertex0.n;
				const Vector4f& n1 = vertex1.n;
				const Vector4f& n2 = vertex2.n;

				// Init color
				float rSum0 = 0;
				float gSum0 = 0;
				float bSum0 = 0;

				float rSum1 = 0;
				float gSum1 = 0;
				float bSum1 = 0;

				float rSum2 = 0;
				float gSum2 = 0;
				float bSum2 = 0;

				// has lighted
				bool hasV0Lit = ((vertex0.attr & Vertex::Attr_Lit) != 0);
				bool hasV1Lit = ((vertex1.attr & Vertex::Attr_Lit) != 0);
				bool hasV2Lit = ((vertex2.attr & Vertex::Attr_Lit) != 0);
				if (hasV0Lit && hasV1Lit && hasV2Lit)
					return;

				// lit
				for (const auto& light : lights)
				{
					if (light.state != Light::STATE_ON)
						continue;

					switch (light.attr)
					{
					case Light::ATTR_AMBIENT:
						{
							if (!hasV0Lit)
							{
								rSum0 += (float)light.color.r * vertex0.color.r / 256;
								gSum0 += (float)light.color.r * vertex0.color.g / 256;
								bSum0 += (float)light.color.r * vertex0.color.b / 256;
							}

							if (!hasV1Lit)
							{
								rSum1 += (float)light.color.r * vertex1.color.r / 256;
								gSum1 += (float)light.color.r * vertex1.color.g / 256;
								bSum1 += (float)light.color.r * vertex1.color.b / 256;
							}

							if (!hasV2Lit)
							{
								rSum2 += (float)light.color.r * vertex2.color.r / 256;
								gSum2 += (float)light.color.r * vertex2.color.g / 256;
								bSum2 += (float)light.color.r * vertex2.color.b / 256;
							}
						}
						break;
					case Light::ATTR_INFINITE:
						{
							// Vertex 0
							if (! hasV0Lit)
								InfiniteLightLit(rSum0, gSum0, bSum0, vertex0.color, light, n0);
							// Vertex 1
							if (! hasV1Lit)
								InfiniteLightLit(rSum1, gSum1, bSum1, vertex1.color, light, n1);
							// Vertex 2
							if (! hasV2Lit)
								InfiniteLightLit(rSum2, gSum2, bSum2, vertex2.color, light, n2);
						}
						break;
					case Light::ATTR_POINT:
						{
							// Vertex 0
							if (! hasV0Lit)
								PointLightLit(rSum0, gSum0, bSum0, vertex0.color, light, p0, n0);
							// Vertex 1
							if (! hasV1Lit)
								PointLightLit(rSum1, gSum1, bSum1, vertex1.color, light, p1, n1);
							// Vertex 2
							if (hasV2Lit)
								PointLightLit(rSum2, gSum2, bSum2, vertex2.color, light, p2, n2);
						}
						break;
					case Light::ATTR_SPOTLIGHT1:
						{
							// Vertex 0
							if (! hasV0Lit)
								SpotLight1Lit(rSum0, gSum0, bSum0, vertex0.color, light, p0, n0);
							// Vertex 1
							if (! hasV1Lit)
								SpotLight1Lit(rSum1, gSum1, bSum1, vertex1.color, light, p1, n1);
							// Vertex 2
							if (! hasV2Lit)
								SpotLight1Lit(rSum2, gSum2, bSum2, vertex2.color, light, p2, n2);
						}
						break;
					case Light::ATTR_SPOTLIGHT2:
						{
							// Vertex 0
							if (! hasV0Lit)
								SpotLight2Lit(rSum0, gSum0, bSum0, vertex0.color, light, p0, n0);
							// Vertex 1
							if (! hasV1Lit)
								SpotLight2Lit(rSum1, gSum1, bSum1, vertex1.color, light, p1, n1);
							// Vertex 2
							if (! hasV2Lit)
								SpotLight2Lit(rSum2, gSum2, bSum2, vertex2.color, light, p2, n2);
						}
						break;
					}
				}

				rSum0 = math::Clamp(rSum0, 0.0f, 255.0f);
				gSum0 = math::Clamp(gSum0, 0.0f, 255.0f);
				bSum0 = math::Clamp(bSum0, 0.0f, 255.0f);

				rSum1 = math::Clamp(rSum1, 0.0f, 255.0f);
				gSum1 = math::Clamp(gSum1, 0.0f, 255.0f);
				bSum1 = math::Clamp(bSum1, 0.0f, 255.0f);

				rSum2 = math::Clamp(rSum2, 0.0f, 255.0f);
				gSum2 = math::Clamp(gSum2, 0.0f, 255.0f);
				bSum2 = math::Clamp(bSum2, 0.0f, 255.0f);

				vertex0.litColor = Color((unsigned char)rSum0, (unsigned char)gSum0, (unsigned char)bSum0);
				vertex1.litColor = Color((unsigned char)rSum1, (unsigned char)gSum1, (unsigned char)bSum1);
				vertex2.litColor = Color((unsigned char)rSum2, (unsigned char)gSum2, (unsigned char)bSum2);

				vertex0.attr |= Vertex::Attr_Lit;
				vertex1.attr |= Vertex::Attr_Lit;
				vertex2.attr |= Vertex::Attr_Lit;
			}
			else
			{
				// emmisive shading only, do nothing
				vertex0.litColor = vertex0.color;
				vertex1.litColor = vertex1.color;
				vertex2.litColor = vertex2.color;

				vertex0.attr |= Vertex::Attr_Lit;
				// since vertex1, vertex2 may also belong to other poly, it may need lit
			}
		}

		void TLBase::InfiniteLightLit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& n)
		{
			assert(light.attr == Light::ATTR_INFINITE);

			// infinite lighting, we need the surface normal, and the direction of the light
			// but no longer to compute normal or length, we already have the vertex normal and it's length is 1.0

			// Infinite light:
			// I(d)dir = I0dir * Cldir

			// diffuse model
			// Itotald = Rsdiffuse * Idiffuse * (n . l)

			float dp = DotProduct(n, light.dir);
			if (dp > 0)
			{
				rLit += light.color.r * color.r * dp / 256;
				gLit += light.color.g * color.g * dp / 256;
				bLit += light.color.b * color.b * dp / 256;
			}
		}

		void TLBase::PointLightLit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n)
		{
			assert(light.attr == Light::ATTR_POINT);

			// Point light, So need know the the normal vector of poly (n),
			// the dir of light (l) and the distance between light & poly (d)

			// Point light: 
			// I(d)point  = I0point  * Clpoint  / (kc + kl * d + kq * d * d)
			// Where d = |p - s|

			// diffuse model
			// Itotald = Rsdiffuse * Idiffuse * (n . l)

			Vector4f l = light.pos - p;
			float dp = DotProduct(n, l);
			if (dp > 0)
			{
				float d = l.Length();
				float dd = l.LengthSQ();
				float k = light.kc + light.kl * d + light.kq * dd;

				// We need to divide d since l isn't unit vector
				float a = dp / (k * d);

				rLit += light.color.r * color.r * a / 256;
				gLit += light.color.g * color.g * a / 256;
				bLit += light.color.b * color.b * a / 256;
			}
		}

		void TLBase::SpotLight1Lit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n)
		{
			assert(light.attr == Light::ATTR_SPOTLIGHT1);

			// Simple spot light, using point light with dir for simulate spot light
			// Simple model: I(d)point = I0point  * Clpoint  / (kc + kl * d + kq * d * d)

			// diffuse model
			// Itotald = Rsdiffuse * Idiffuse * (n . l)

			// Note: using direction of light, not l

			float dp = DotProduct(n, light.dir);
			if (dp > 0)
			{
				Vector4f l = light.pos - p;
				float d = l.Length();
				float dd = l.LengthSQ();
				float k = light.kc + light.kl * d + light.kq * dd;

				float a = dp / k;

				rLit += light.color.r * color.r * a / 256;
				gLit += light.color.g * color.g * a / 256;
				bLit += light.color.b * color.b * a / 256;
			}
		}

		void TLBase::SpotLight2Lit(float& rLit, float& gLit, float& bLit, Color color, const Light& light, const Vector4f& p, const Vector4f& n)
		{
			assert(light.attr == Light::ATTR_SPOTLIGHT2);

			// Simple spot light, do not distinguish umbra & penumbra
			// Simple model: I(d)spotlight = IOspotlight * Clsportlight * MAX(cos theta, 0)^pf /  (Kc + kl * d + kq * d * d)
			// theta: angle between dir of light and the vector s to light

			// diffuse model
			// Itotald = Rsdiffuse * Idiffuse * (n . l)

			// Note: using direction of light, not l

			float dp = DotProduct(n, light.dir);
			if (dp > 0)
			{
				// compute vector from light to surface (different from l which IS the light dir)
				Vector4f s = p - light.pos;
				float d = s.Length();

				// compute spot light term (s . l), for calc cos(theta), which theta is angle between s(light to surface) and l(light.dir)
				float dpsl = DotProduct(s, light.dir) / d;
				if (dpsl > 0)
				{
					float dd = s.LengthSQ();
					float k = light.kc + light.kl * d + light.kq * dd;

					// (s . l) ^ pf
					float dpsl_exp = dpsl;
					int i_exp = 1;
					while ((i_exp++) < int(light.pf))
						dpsl_exp *= dpsl;

					float a = dp * dpsl_exp / k;

					rLit += light.color.r * color.r * a / 256;
					gLit += light.color.g * color.g * a / 256;
					bLit += light.color.b * color.b * a / 256;
				}
			}
		}
	}
}