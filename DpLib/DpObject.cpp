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
		void Object::Transform(const Matrix44f& m, int transform, bool transformBase)
		{
			for (int i = 0; i < numVertices; ++i)
			{
				switch (transform)
				{
				case TRANSFORM_LOCAL_ONLY:
					vListLocal[i] *= m;
					break;
				case TRANSFORM_TRANS_ONLY:
					vListTrans[i] *= m;
					break;
				case TRANSFORM_LOCAL_TO_TRANS:
					vListTrans[i] = vListLocal[i] * m;
					break;
				default:
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

		void Object::ModelToWorld(const Vector4f& worldPos, int transform)
		{
			assert(transform == TRANSFORM_LOCAL_TO_TRANS || transform == TRANSFORM_TRANS_ONLY);

			this->worldPos = worldPos;

			if (transform == TRANSFORM_LOCAL_TO_TRANS)
			{
				for (int i = 0; i < numVertices; ++i)
				{
					vListTrans[i] = vListLocal[i];
					vListTrans[i] += worldPos;
				}
			}
			else
			{
				for (int i = 0; i < numVertices; ++i)
				{
					vListTrans[i] += worldPos;
				}
			}
		}

		void Object::ModelToWorldMatrix(const Vector4f& worldPos, int transform)
		{
			this->worldPos = worldPos;
			Matrix44f m(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				worldPos.x, worldPos.y, worldPos.z, 1);
			Transform(m, transform, true);
		}

		void Object::WorldToCamera(const Camera& camera)
		{
			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i] *= camera.matrixCamera;
			}
		}

		bool Object::Cull(const Camera& camera, int cullFlag)
		{
			// Try to cull object by the world pos of object and the camera clip plane
			Vector4f transPos = worldPos;
			transPos *= camera.matrixCamera;

			// Cull Z
			if (cullFlag & Camera::CULL_PLANE_Z)
			{
				if (transPos.z + maxRadius < camera.nearClipZ || transPos.z - maxRadius > camera.farClipZ)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull X
			if (cullFlag & Camera::CULL_PLANE_X)
			{
				float xtest = 0.5f * transPos.z * camera.viewPlaneWidth / camera.viewDist;
				if (transPos.x - maxRadius > xtest || transPos.x + maxRadius < -xtest)
				{
					state |= STATE_CULLED;
					return true;
				}
			}

			// Cull Y
			if (cullFlag & Camera::CULL_PLANE_Y)
			{
				float ytest = 0.5f * transPos.z *  camera.viewPlaneHeight / camera.viewDist;
				if (transPos.y - maxRadius > ytest || transPos.y + maxRadius < -ytest)
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

				// Calc Poly face nomral vector
				// u:p0->p1, v:p0->p2, n: uxv
				Vector4f u = vListTrans[vi1] - vListTrans[vi0];
				Vector4f v = vListTrans[vi2] - vListTrans[vi0];
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - vListTrans[vi0];
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
			Transform(m, TRANSFORM_TRANS_ONLY, true);
		}

		void Object::ConvertFromHomogeneous()
		{
			for (int i = 0; i < numVertices; ++i)
			{
				vListTrans[i] /= vListTrans[i].w;
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

				device.DrawLine(Point(vListTrans[i0].x, vListTrans[i0].y), Point(vListTrans[i1].x, vListTrans[i1].y), p->shadeColor);
				device.DrawLine(Point(vListTrans[i1].x, vListTrans[i1].y), Point(vListTrans[i2].x, vListTrans[i2].y), p->shadeColor);
				device.DrawLine(Point(vListTrans[i2].x, vListTrans[i2].y), Point(vListTrans[i0].x, vListTrans[i0].y), p->shadeColor);
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

				device.DrawTriangle(Point(vListTrans[i0].x, vListTrans[i0].y), Point(vListTrans[i1].x, vListTrans[i1].y), Point(vListTrans[i2].x, vListTrans[i2].y), p->shadeColor);
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
				pf.prev = NULL;
				pf.next = NULL;
			}
			else
			{
				pf.prev = pPolyFace[numPolyFaces - 1];
				pf.next = NULL;
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

			pf.attr = poly.attr;
			pf.state = poly.state;
			pf.color = poly.color;
			pf.shadeColor = poly.shadeColor;

			pf.vlist[0] = poly.vlist[poly.vert[0]];
			pf.vlist[1] = poly.vlist[poly.vert[1]];
			pf.vlist[2] = poly.vlist[poly.vert[2]];

			pf.tlist[0] = poly.vlist[poly.vert[0]];
			pf.tlist[1] = poly.vlist[poly.vert[1]];
			pf.tlist[2] = poly.vlist[poly.vert[2]];

			if (numPolyFaces == 0)
			{
				pf.prev = NULL;
				pf.next = NULL;
			}
			else
			{
				pf.prev = pPolyFace[numPolyFaces - 1];
				pf.next = NULL;
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
				Vector4f* oldList = poly->vlist;

				if (insertLocal)
					poly->vlist = const_cast<Vector4f*>(object.vListLocal);
				else
					poly->vlist = const_cast<Vector4f*>(object.vListTrans);

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
						pf->vlist[j] *= m;
						break;
					case TRANSFORM_TRANS_ONLY:
						pf->tlist[j] *= m;
						break;
					case TRANSFORM_LOCAL_TO_TRANS:
						pf->tlist[j] = pf->vlist[j] * m;
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
						pf->tlist[j] = pf->vlist[j] + worldPos;
					else
						pf->tlist[j] += worldPos;
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
					pf->tlist[j] *= camera.matrixCamera;
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
				Vector4f u = pf->tlist[1] - pf->tlist[0];
				Vector4f v = pf->tlist[2] - pf->tlist[0];
				Vector4f n = CrossProduct(u, v);

				Vector4f view = camera.pos - pf->tlist[0];

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
					pf->tlist[j] /= pf->tlist[j].w;
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

				Poly poly;
				poly.attr = pf->attr;
				poly.color = pf->color;
				poly.state = pf->state;
				poly.vert[0] = 0;
				poly.vert[1] = 1;
				poly.vert[2] = 2;
				poly.vlist = pf->tlist;

				TLBase::Lighting(camera, lights, poly);

				pf->shadeColor = poly.shadeColor;
			}
		}

		void RenderList::RenderWire(const Device& device) const
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (! pf || ! (pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawLine(Point(pf->tlist[0].x, pf->tlist[0].y), Point(pf->tlist[1].x, pf->tlist[1].y), pf->shadeColor);
				device.DrawLine(Point(pf->tlist[1].x, pf->tlist[1].y), Point(pf->tlist[2].x, pf->tlist[2].y), pf->shadeColor);
				device.DrawLine(Point(pf->tlist[2].x, pf->tlist[2].y), Point(pf->tlist[0].x, pf->tlist[0].y), pf->shadeColor);
			}
		}

		void RenderList::RenderSolid(const Device& device) const
		{
			for (int i = 0; i < numPolyFaces; ++i)
			{
				PolyFace* pf = pPolyFace[i];
				if (!pf || !(pf->state & POLY_STATE_ACTIVE) || (pf->state & POLY_STATE_CLIPPED) || (pf->state & POLY_STATE_BACKFACE))
					continue;

				device.DrawTriangle(Point(pf->tlist[0].x, pf->tlist[0].y), Point(pf->tlist[1].x, pf->tlist[1].y), Point(pf->tlist[2].x, pf->tlist[2].y), pf->shadeColor);
			}
		}

		//////////////////////////////////////////////////////////////////////////
#pragma warning(push,3)
#pragma warning(disable:4100)
		void TLBase::Lighting(const Camera& camera, const std::vector<Light>& lights, Poly& poly)
		{
			if (!(poly.state & POLY_STATE_ACTIVE) || (poly.state & POLY_STATE_CLIPPED) || (poly.state & POLY_STATE_BACKFACE))
				return;

			if (!((poly.attr & POLY_ATTR_SHADE_FLAT) || (poly.attr & POLY_ATTR_SHADE_GOURAUD)))
				return;

			int rBase = poly.color.r;
			int gBase = poly.color.g;
			int bBase = poly.color.b;

			int i0 = poly.vert[0];
			int i1 = poly.vert[1];
			int i2 = poly.vert[2];

			const Vector4f& v0 = poly.vlist[i0];
			const Vector4f& v1 = poly.vlist[i1];
			const Vector4f& v2 = poly.vlist[i2];

			// Init color
			int rSum = 0;
			int gSum = 0;
			int bSum = 0;

			// Normal vector
			Vector4f u = v1 - v0;
			Vector4f v = v2 - v1;
			Vector4f n = CrossProduct(u, v);
			n.Normalize();

			// Traverse all lights
			for (unsigned int i = 0; i < lights.size(); ++i)
			{
				const Light& light = lights[i];

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
					{
						// Infinite light, So need know the normal vector of poly (n),
						// and dir of light (l)

						// Diffuse formula:
						// Itotald = Rsdiffuse * Idiffuse * (n ， l)

						// Since light.dir is unit vector, dp = n ， l 
						float dp = DotProduct(n, light.dir);
						if (dp > 0)
						{
							// Infinite light:
							// I(d)dir  = Cldir  * I0dir

							rSum += int(light.color.r * rBase * dp / 256);
							gSum += int(light.color.g * gBase * dp / 256);
							bSum += int(light.color.b * bBase * dp / 256);
						}
					}
					break;
				case Light::ATTR_POINT:
					{
						// Point light, So need know the the normal vector of poly (n),
						// the dir of light (l) and the distance between light & poly (d)

						// Diffuse formula:
						// Itotald = Rsdiffuse * Idiffuse * (n ， l)

						Vector4f l = light.pos - v0;
						float dp = DotProduct(n, l);
						if (dp > 0)
						{
							// Point light: 
							// I(d)point  = I0point  * Clpoint  / (kc + kl * d + kq * d2)

							float d = l.Length();
							float dd = l.LengthSQ();
							float k = light.kc + light.kl * d + light.kq * dd;

							// a : (n ， l) / (Kc + kl * d + kq * d2)
							// We need to divide d since l isn't unit vector

							float a = dp / (d * k);

							rSum += int(light.color.r * rBase * a / 256);
							gSum += int(light.color.g * gBase * a / 256);
							bSum += int(light.color.b * bBase * a / 256);
						}
					}
					break;
				case Light::ATTR_SPOTLIGHT1:
					{
						// Simple spot light, use point light with dir for simulate spot light
						// Simple model: I(d)point = I0point  * Clpoint  / (kc + kl * d + kq * d2)

						// Diffuse formula:
						// Itotald = Rsdiffuse * Idiffuse * (n ， l)

						// For calc distance
						Vector4f l = light.pos - v0;

						float dp = DotProduct(n, light.dir);
						if (dp > 0)
						{
							float d = l.Length();
							float dd = l.LengthSQ();
							float k = light.kc + light.kl * d + light.kq * dd;

							float a = dp / k;

							rSum += int(light.color.r * rBase * a / 256);
							gSum += int(light.color.g * gBase * a / 256);
							bSum += int(light.color.b * bBase * a / 256);
						}
					}
					break;
				case Light::ATTR_SPOTLIGHT2:
					{
						// Simple spot light, do not distinguish umbra & penumbra
						// formula: I(d)spotlight = IOspotlight * Clsportlight * MAX(cos angle, 0)^pf /  (Kc + kl * d + kq * d2)
						// angle : angle between dir of light and the vector s to light

						// Diffuse formula:
						// Itotald = Rsdiffuse * Idiffuse * (n ， l)

						float dp = DotProduct(n, light.dir);
						if (dp > 0)
						{
							Vector4f s = light.pos - v0;
							float d = s.Length();

							float dpsl = DotProduct(s, light.dir) / d;
							if (dpsl > 0)
							{
								float dd = s.LengthSQ();
								float k = light.kc + light.kl * d + light.kq * dd;

								// (s ， l)^pf 
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
					}
					break;
				}
				poly.shadeColor = Color((unsigned char)rSum, (unsigned char)gSum, (unsigned char)bSum);
			}
		}
#pragma warning(pop)
	}
}