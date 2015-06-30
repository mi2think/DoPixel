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
#include <cassert>

using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
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
			Destroy();

			this->numVertices = numVertices;
			this->numPolys = numPolys;
			this->numFrames = numFrames;
			this->totalVertices = this->numVertices * this->numFrames;

			vListLocalHead = new Vertex[numVertices * numFrames];
			vListTransHead = new Vertex[numVertices * numFrames];

			pList = new Poly[numPolys];

			coordlist = new Vector2f[numPolys * 3];

			avgRadius = new float[numFrames];
			maxRadius = new float[numFrames];

			SetFrame(0);
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
			if (vListLocalHead)
			{
				delete[] vListLocalHead;
				vListLocal = nullptr;
				vListLocalHead = nullptr;
			}

			if (vListTransHead)
			{
				delete[] vListTransHead;
				vListTrans = nullptr;
				vListTransHead = nullptr;
			}

			if (pList)
			{
				delete[] pList;
				pList = nullptr;
			}

			if (coordlist)
			{
				delete[] coordlist;
				coordlist = nullptr;
			}

			if (avgRadius)
			{
				delete[] avgRadius;
				avgRadius = nullptr;
			}

			if (maxRadius)
			{
				delete[] maxRadius;
				maxRadius = nullptr;
			}
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
					vListLocalHead[i].v *= m;
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
					vListLocal[i].v *= m;
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

			// scale poly normal length
			for (int i = 0; i < numPolys; ++i)
			{
				// why 'scale.x * scale.y * scale.z' ???
				pList[i].nlength *= (scale.x * scale.y * scale.z);
			}
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
					float distVertex = p[i].v.Length();
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

		void Object::Transform(const Matrix44f& m, int transform, bool transformBase, bool allFrames)
		{
			auto fnTransfrom = [&m](Vertex* p, int count)
			{
				for (int i = 0; i < count; ++i, ++p)
				{
					p->v *= m;
					if ((p->attr & Vertex::Attr_Normal) != 0)
						p->n *= m;
				}
			};

			auto fnTransfromTo = [&m](Vertex* p, Vertex* q, int count)
			{
				for (int i = 0; i < count; ++i, ++p, ++q)
				{
					q->v = p->v * m;
					if ((p->attr & Vertex::Attr_Normal) != 0)
						q->n = p->n * m;
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
						vListTransHead[i].v = vListLocalHead[i].v + worldPos;
						// copy normal
						vListTransHead[i].n = vListLocalHead[i].n;
					}
				}
				else
				{
					for (int i = 0; i < totalVertices; ++i)
					{
						vListTransHead[i].v += worldPos;
					}
				}
			}
			else
			{
				if (transform == TRANSFORM_LOCAL_TO_TRANS)
				{
					for (int i = 0; i < numVertices; ++i)
					{
						vListTrans[i].v = vListLocal[i].v + worldPos;
						// copy normal
						vListTrans[i].n = vListLocal[i].n;
					}
				}
				else
				{
					for (int i = 0; i < numVertices; ++i)
					{
						vListTrans[i].v += worldPos;
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
		}

		void Object::WorldToCamera(const Camera& camera)
		{
			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i].v *= camera.matrixCamera;
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
			if (cullFlag & Camera::CULL_PLANE_Z)
			{
				if (spherePos.z + _maxRadius < camera.nearClipZ || spherePos.z - _maxRadius > camera.farClipZ)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull X
			if (cullFlag & Camera::CULL_PLANE_X)
			{
				float xtest = 0.5f * spherePos.z * camera.viewPlaneWidth / camera.viewDist;
				if (spherePos.x - _maxRadius > xtest || spherePos.x + _maxRadius < -xtest)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull Y
			if (cullFlag & Camera::CULL_PLANE_Y)
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

				p->state &= (~POLY_STATE_BACKFACE);
				p->state &= (~POLY_STATE_CLIPPED);
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
				Vector4f u = vListTrans[vi1].v - vListTrans[vi0].v;
				Vector4f v = vListTrans[vi2].v - vListTrans[vi0].v;
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - vListTrans[vi0].v;
				float dp = DotProduct(view, n);
				if (dp <= 0.0f)
					p->state |= POLY_STATE_BACKFACE;
			}
		}

		void Object::CameraToPerspective(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			for (int i = 0; i < numVertices; ++i)
			{
				float z = vListTrans[i].z;
				vListTrans[i].x = vListTrans[i].x * camera.viewDist / z;
				vListTrans[i].y = vListTrans[i].y * camera.viewDist * camera.aspectRatio / z;
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
				vListTrans[i].v /= vListTrans[i].w;
			}
		}

		void Object::PerspectiveToScreen(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			// Assume the coordinate of object is normalied, range is [-1,1]
			// Then Scale it by viewport, and reverse Y axis

			float alpha = 0.5f * camera.viewportWidth - 0.5f;
			float beta = 0.5f * camera.viewportHeight - 0.5f;

			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i].x = alpha + alpha * vListTrans[i].x;
				vListTrans[i].y = beta - beta * vListTrans[i].y;
			}
		}

		void Object::CameraToScreen(const Camera& camera)
		{
			// You may not keep object in this stage of pipeline
			// Just a demo

			float alpha = 0.5f * camera.viewportWidth - 0.5f;
			float beta = 0.5f * camera.viewportHeight - 0.5f;

			for (int i = 0; i < numVertices; ++i)
			{
				// First, camera to perspective
				// Here, d also called camera.viewDist, is (camera.viewportWidth) * tan(theta) / 2
				// So: Xper = (d * Xc) / Zc
				//	   Yper = (d * Yc) / Zc
				float z = vListTrans[i].z;
				vListTrans[i].x = vListTrans[i].x * camera.viewDist / z;
				vListTrans[i].y = vListTrans[i].y * camera.viewDist * camera.aspectRatio / z;
				
				// Then We need reverse Y axis
				// Xper = Xper
				// Yper = (camera.viewportHeight - 1) - Yper

				// Now, the pos range:
				// Xper:(-(camera.viewportWidth - 1) / 2, (camera.viewportWidth - 1) / 2)
				// Yper:(-(camera.viewportHeight - 1) / 2,(camera.viewportHeight - 1) / 2)
				// Then, we need to translate it to screen
				
				// Xscreen = Xper + (camera.viewportWidth - 1) / 2
				// Yscreen = -Yper + (camera.viewportHeight - 1) / 2

				vListTrans[i].x = alpha + vListTrans[i].x;
				vListTrans[i].y = beta - vListTrans[i].y;
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
	
				TLBase::Lighting(camera, lights, *poly);
			}
		}

		void Object::RenderWire(const Device& device) const
		{
			if (state & STATE_CULLED)
				return;

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

				device.DrawLine(Point(vListTrans[i0].x, vListTrans[i0].y), Point(vListTrans[i1].x, vListTrans[i1].y), p->color);
				device.DrawLine(Point(vListTrans[i1].x, vListTrans[i1].y), Point(vListTrans[i2].x, vListTrans[i2].y), p->color);
				device.DrawLine(Point(vListTrans[i2].x, vListTrans[i2].y), Point(vListTrans[i0].x, vListTrans[i0].y), p->color);
			}
		}

		void Object::RenderSolid(const Device& device) const
		{
			if (state & STATE_CULLED)
				return;

			for (int i = 0; i < numPolys; ++i)
			{
				const Poly* p = &pList[i];

				if ((p->state & POLY_STATE_BACKFACE) || (p->state & POLY_STATE_CLIPPED))
					continue;

				int i0 = p->vert[0];
				int i1 = p->vert[1];
				int i2 = p->vert[2];

				device.DrawTriangle(Point(vListTrans[i0].x, vListTrans[i0].y), Point(vListTrans[i1].x, vListTrans[i1].y), Point(vListTrans[i2].x, vListTrans[i2].y), p->litColor[0]);
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
						pf->vlist[j].v *= m;
						if ((pf->vlist[j].attr & Vertex::Attr_Normal) != 0)
							pf->vlist[j].n *= m;
						break;
					case TRANSFORM_TRANS_ONLY:
						pf->tlist[j].v *= m;
						if ((pf->tlist[j].attr & Vertex::Attr_Normal) != 0)
							pf->tlist[j].n *= m;
						break;
					case TRANSFORM_LOCAL_TO_TRANS:
						pf->tlist[j].v = pf->vlist[j].v * m;
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
						pf->tlist[j].v = pf->vlist[j].v + worldPos;
					else
						pf->tlist[j].v += worldPos;
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
					pf->tlist[j].v *= camera.matrixCamera;
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
				Vector4f u = pf->tlist[1].v - pf->tlist[0].v;
				Vector4f v = pf->tlist[2].v - pf->tlist[0].v;
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - pf->tlist[0].v;

				float dp = DotProduct(view, n);
				if (dp <= 0.0f)
					pf->state |= POLY_STATE_BACKFACE;
			}
		}

		void RenderList::CameraToPerspective(const Camera& camera)
		{
			// Assume the poly in render list has been transformed to camera,
			// And stored in tlist

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					float z = pf->tlist[j].z;
					pf->tlist[j].x = pf->tlist[j].x * camera.viewDist / z;
					pf->tlist[j].y = pf->tlist[j].y * camera.viewDist * camera.aspectRatio / z;
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
					pf->tlist[j].v /= pf->tlist[j].w;
				}
			}
		}

		void RenderList::PerspectiveToScreen(const Camera& camera)
		{
			// Assume the coordinate of object is normalized, range is [-1,1]
			// Then Scale it by viewport, and reverse Y axis

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				float alpha = 0.5f * camera.viewportWidth - 0.5f;
				float beta = 0.5f * camera.viewportHeight - 0.5f;

				for (int j = 0; j < 3; ++j)
				{
					pf->tlist[j].x = alpha + pf->tlist[j].x * alpha;
					pf->tlist[j].y = beta - pf->tlist[j].y * beta;
				}
			}
		}

		void RenderList::CameraToScreen(const Camera& camera)
		{
			float alpha = 0.5f * camera.viewportWidth - 0.5f;
			float beta  = 0.5f * camera.viewportHeight - 0.5f;

			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_BACKFACE) || (pf->state & POLY_STATE_CLIPPED))
					continue;

				for (int j = 0; j < 3; ++j)
				{
					// First, camera to perspective
					// Here, d also called camera.viewDist, is (camera.viewportWidth) * tan(theta) / 2
					// So: Xper = (d * Xc) / Zc
					//	   Yper = (d * Yc) / Zc
					float z = pf->tlist[j].z;
					pf->tlist[j].x = pf->tlist[j].x * camera.viewDist / z;
					pf->tlist[j].y = pf->tlist[j].y * camera.viewDist * camera.aspectRatio / z;
					
					// Then We need reverse Y axis
					// Xper = Xper
					// Yper = (camera.viewportHeight - 1) - Yper

					// Now, the pos range:
					// Xper:(-(camera.viewportWidth - 1) / 2, (camera.viewportWidth - 1) / 2)
					// Yper:(-(camera.viewportHeight - 1) / 2,(camera.viewportHeight - 1) / 2)
					// Then, we need to translate it to screen

					// Xscreen = Xper + (camera.viewportWidth - 1) / 2
					// Yscreen = -Yper + (camera.viewportHeight - 1) / 2

					pf->tlist[j].x = alpha + pf->tlist[j].x;
					pf->tlist[j].y = beta - pf->tlist[j].y;
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

				//Poly poly;
				//poly.attr = pf->attr;
				//poly.color = pf->color;
				//poly.state = pf->state;
				//poly.vert[0] = 0;
				//poly.vert[1] = 1;
				//poly.vert[2] = 2;
				//poly.vlist = pf->tlist;

				//TLBase::Lighting(camera, lights, poly);

				//pf->shadeColor = poly.shadeColor;
			}
		}

		void RenderList::RenderWire(const Device& device) const
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawLine(Point(pf->tlist[0].x, pf->tlist[0].y), Point(pf->tlist[1].x, pf->tlist[1].y), pf->color);
				device.DrawLine(Point(pf->tlist[1].x, pf->tlist[1].y), Point(pf->tlist[2].x, pf->tlist[2].y), pf->color);
				device.DrawLine(Point(pf->tlist[2].x, pf->tlist[2].y), Point(pf->tlist[0].x, pf->tlist[0].y), pf->color);
			}
		}

		void RenderList::RenderSolid(const Device& device) const
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(Point(pf->tlist[0].x, pf->tlist[0].y), Point(pf->tlist[1].x, pf->tlist[1].y), Point(pf->tlist[2].x, pf->tlist[2].y), pf->color);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Now just calc diffuse light model.
		// Rsdiffuse: diffuse color of material, assume every surface has a material
		// Idiffuse: intensity of diffuse color of light.

		void TLBase::Lighting(const Camera& camera, const std::vector<Light>& lights, Poly& poly)
		{
			if (!(poly.state & POLY_STATE_ACTIVE) || (poly.state & POLY_STATE_CLIPPED) || (poly.state & POLY_STATE_BACKFACE))
				return;

			poly.state |= POLY_STATE_LIT;

			int rBase = poly.color.r;
			int gBase = poly.color.g;
			int bBase = poly.color.b;

			int i0 = poly.vert[0];
			int i1 = poly.vert[1];
			int i2 = poly.vert[2];

			const Vector4f& v0 = poly.vlist[i0].v;
			const Vector4f& v1 = poly.vlist[i1].v;
			const Vector4f& v2 = poly.vlist[i2].v;

			// Normal vector
			Vector4f u = v1 - v0;
			Vector4f v = v2 - v1;
			Vector4f n = CrossProduct(u, v);
			n.Normalize();

			// Compute lit
			auto fnInfiniteLightLit = [&rBase, &gBase, &bBase](int& rSum, int& gSum, int& bSum, const Light& light, const Vector4f& n)
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
					rSum += int(light.color.r * rBase * dp / 256);
					gSum += int(light.color.g * gBase * dp / 256);
					bSum += int(light.color.b * bBase * dp / 256);
				}
			};

			auto fnPointLightLit = [&rBase, &gBase, &bBase](int& rSum, int& gSum, int& bSum, const Light& light, const Vector4f& v, const Vector4f& n)
			{
				assert(light.attr == Light::ATTR_POINT);

				// Point light, So need know the the normal vector of poly (n),
				// the dir of light (l) and the distance between light & poly (d)

				// Point light: 
				// I(d)point  = I0point  * Clpoint  / (kc + kl * d + kq * d * d)
				// Where d = |p - s|

				// diffuse model
				// Itotald = Rsdiffuse * Idiffuse * (n . l)
				
				Vector4f l = light.pos - v;
				float dp = DotProduct(n, l);
				if (dp > 0)
				{
					float d = l.Length();
					float dd = l.LengthSQ();
					float k = light.kc + light.kl * d + light.kq * dd;

					// We need to divide d since l isn't unit vector
					float a = dp / (k * d);

					rSum += int(light.color.r * rBase * a / 256);
					gSum += int(light.color.g * gBase * a / 256);
					bSum += int(light.color.b * bBase * a / 256);
				}
			};

			auto fnSpotLight1Lit = [&rBase, &gBase, &bBase](int& rSum, int& gSum, int& bSum, const Light& light, const Vector4f& v, const Vector4f& n)
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
					Vector4f l = light.pos - v;
					float d = l.Length();
					float dd = l.LengthSQ();
					float k = light.kc + light.kl * d + light.kq * dd;

					float a = dp / k;

					rSum += int(light.color.r * rBase * a / 256);
					gSum += int(light.color.g * gBase * a / 256);
					bSum += int(light.color.b * bBase * a / 256);
				}
			};

			auto fnSpotLight2Lit = [&rBase, &gBase, &bBase](int& rSum, int& gSum, int& bSum, const Light& light, const Vector4f& v, const Vector4f& n)
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
					Vector4f s = v - light.pos;
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

						rSum += int(light.color.r * rBase * a / 256);
						gSum += int(light.color.g * gBase * a / 256);
						bSum += int(light.color.b * bBase * a / 256);
					}
				}
			};

			if ((poly.attr & POLY_ATTR_SHADE_FLAT) != 0)
			{
				// Init color
				int rSum = 0;
				int gSum = 0;
				int bSum = 0;

				for (const auto& light : lights)
				{
					if (light.state != Light::STATE_ON)
						continue;

					switch (light.attr)
					{
					case Light::ATTR_AMBIENT:
						{
							rSum += light.color.r * rBase / 256;
							gSum += light.color.g * gBase / 256;
							bSum += light.color.b * bBase / 256;
						}
						break;
					case Light::ATTR_INFINITE:
						fnInfiniteLightLit(rSum, gSum, bSum, light, n);
						break;
					case Light::ATTR_POINT:
						fnPointLightLit(rSum, gSum, bSum, light, v0, n);
						break;
					case Light::ATTR_SPOTLIGHT1:
						fnSpotLight1Lit(rSum, gSum, bSum, light, v0, n);
						break;
					case Light::ATTR_SPOTLIGHT2:
						fnSpotLight2Lit(rSum, gSum, bSum, light, v0, n);
						break;
					}
				}

				rSum = Math::Clamp(rSum, 0, 255);
				gSum = Math::Clamp(gSum, 0, 255);
				bSum = Math::Clamp(bSum, 0, 255);

				poly.litColor[0] = Color((unsigned char)rSum, (unsigned char)gSum, (unsigned char)bSum);
			}
			else if ((poly.attr & POLY_ATTR_SHADE_GOURAUD) != 0)
			{
				const Vector4f& n0 = poly.vlist[i0].n;
				const Vector4f& n1 = poly.vlist[i1].n;
				const Vector4f& n2 = poly.vlist[i2].n;

				// Init color
				int rSum0 = 0;
				int gSum0 = 0;
				int bSum0 = 0;

				int rSum1 = 0;
				int gSum1 = 0;
				int bSum1 = 0;

				int rSum2 = 0;
				int gSum2 = 0;
				int bSum2 = 0;

				int ri = 0;
				int gi = 0;
				int bi = 0;

				for (const auto& light : lights)
				{
					if (light.state != Light::STATE_ON)
						continue;

					switch (light.attr)
					{
					case Light::ATTR_AMBIENT:
						{
							ri = light.color.r * rBase / 256;
							gi = light.color.g * gBase / 256;
							bi = light.color.b * bBase / 256;

							// ambient light has the same affect on each vertex
							rSum0 += ri;
							gSum0 += gi;
							bSum0 += bi;

							rSum1 += ri;
							gSum1 += gi;
							bSum1 += bi;

							rSum2 += ri;
							gSum2 += gi;
							bSum2 += bi;
						}
						break;
					case Light::ATTR_INFINITE:
						{
							// Vertex 0
							fnInfiniteLightLit(rSum0, gSum0, bSum0, light, n0);
							// Vertex 1
							fnInfiniteLightLit(rSum1, gSum1, bSum1, light, n1);
							// Vertex 2
							fnInfiniteLightLit(rSum2, gSum2, bSum2, light, n2);
						}
						break;
					case Light::ATTR_POINT:
						{
							// Vertex 0
							fnPointLightLit(rSum0, gSum0, bSum0, light, v0, n0);
							// Vertex 1
							fnPointLightLit(rSum1, gSum1, bSum1, light, v1, n1);
							// Vertex 2
							fnPointLightLit(rSum2, gSum2, bSum2, light, v2, n2);
						}
						break;
					case Light::ATTR_SPOTLIGHT1:
						{
							// Vertex 0
							fnSpotLight1Lit(rSum0, gSum0, bSum0, light, v0, n0);
							// Vertex 1
							fnSpotLight1Lit(rSum1, gSum1, bSum1, light, v1, n1);
							// Vertex 2
							fnSpotLight1Lit(rSum2, gSum2, bSum2, light, v2, n2);
						}
						break;
					case Light::ATTR_SPOTLIGHT2:
						{
							// Vertex 0
							fnSpotLight2Lit(rSum0, gSum0, bSum0, light, v0, n0);
							// Vertex 1
							fnSpotLight2Lit(rSum1, gSum1, bSum1, light, v1, n1);
							// Vertex 2
							fnSpotLight2Lit(rSum2, gSum2, bSum2, light, v2, n2);
						}
						break;
					}
				}

				rSum0 = Math::Clamp(rSum0, 0, 255);
				gSum0 = Math::Clamp(gSum0, 0, 255);
				bSum0 = Math::Clamp(bSum0, 0, 255);

				rSum1 = Math::Clamp(rSum1, 0, 255);
				gSum1 = Math::Clamp(gSum1, 0, 255);
				bSum1 = Math::Clamp(bSum1, 0, 255);

				rSum2 = Math::Clamp(rSum2, 0, 255);
				gSum2 = Math::Clamp(gSum2, 0, 255);
				bSum2 = Math::Clamp(bSum2, 0, 255);

				poly.litColor[0] = Color((unsigned char)rSum0, (unsigned char)gSum0, (unsigned char)bSum0);
				poly.litColor[1] = Color((unsigned char)rSum1, (unsigned char)gSum1, (unsigned char)bSum1);
				poly.litColor[2] = Color((unsigned char)rSum2, (unsigned char)gSum2, (unsigned char)bSum2);
			}
			else
			{
				// emmisive shading only, do nothing
				poly.litColor[0] = poly.color;
			}
		}
	}
}