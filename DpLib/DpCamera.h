/********************************************************************
	created:	2014/06/29
	created:	29:6:2014   13:22
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpCamera.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpCamera
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Camera
*********************************************************************/

#ifndef __DP_CAMERA__
#define __DP_CAMERA__

#include "DpVector2.h"
#include "DpVector4.h"
#include "DpGeometry.h"
#include "DpMatrix44.h"
#include <cassert>

using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		class Camera
		{
		public:
			// Camera model
			enum { MODEL_EULER, MODEL_UVN, MODEL_UVNSimple,};
			// Euler rotate seq
			enum { ROTATE_SEQ_XYZ, ROTATE_SEQ_XZY, ROTATE_SEQ_YXZ, ROTATE_SEQ_YZX, ROTATE_SEQ_ZXY, ROTATE_SEQ_ZYX };
			// Cull object flag
			enum { CULL_PLANE_X = 0x1, CULL_PLANE_Y = 0x2, CULL_PLANE_Z = 0x4, CULL_PLANE_XYZ = CULL_PLANE_X | CULL_PLANE_Y | CULL_PLANE_Z };
			
			int state;
			int attr;

			Vector4f pos;	// pos in world
			
			Vector4f dir;	// Eular angle or UVN's look at

			Vector4f u;
			Vector4f v;
			Vector4f n;		//UVN's dir

			Vector4f target;//UVN's target pos

			float viewDist;

			float fov;		//visual field

			// 3D clip plane, if fov isn't 90, it must be common plane
			float nearClipZ;
			float farClipZ;

			Plane3D rightClipPlane;
			Plane3D leftClipPlane;
			Plane3D topClipPlane;
			Plane3D bottomClipPlane;

			// view plane's width and height
			float viewPlaneWidth;
			float viewPlaneHeight;

			// viewport, image will be rendered to the surface
			float viewportWidth;
			float viewportHeight;
			Vector2f viewportCenterPos;

			float aspectRatio;
			
			Matrix44f matrixCamera;
			Matrix44f matrixPerspective;
			Matrix44f matrixScreen;

			Camera() {}

			inline void InitCamera(int attr, const Vector4f& pos, const Vector4f& dir, const Vector4f& target, float nearClipZ, float farClipZ,
				float fov, float viewPlaneWidth, float viewPlaneHeight);

			inline void BuildCameraMatrixEuler(int rotateSeq);

			inline void BuildCameraMatrixUVN(int uvnType);

			// Build matrix
			inline void BuildCameraToPerspective(Matrix44f& m) const;

			inline void BuildPerspectiveToScreen(Matrix44f& m) const;

			inline void BuildCamerToScreen(Matrix44f& m) const;
		};

		inline void Camera::InitCamera(int attr, const Vector4f& pos, const Vector4f& dir, const Vector4f& target, float nearClipZ, float farClipZ, float fov, 
			float viewportWidth, float viewportHeight)
		{
			this->attr = attr;
			this->pos = pos;
			this->dir = dir;

			this->u = Vector4f(1,0,0,1);	// +X
			this->v = Vector4f(0,1,0,1);	// +Y
			this->n = Vector4f(0,0,1,1);	// +Z

			this->target = target;

			this->nearClipZ = nearClipZ;
			this->farClipZ = farClipZ;

			this->fov = fov;

			this->viewportWidth = viewportWidth;
			this->viewportHeight = viewportHeight;
			this->viewportCenterPos = Vector2f((viewportWidth - 1) / 2.0f, (viewportHeight - 1) / 2.0f);

			this->aspectRatio = viewportWidth / viewportHeight;

			// Set view plane 2 * (2 / ar)
			this->viewPlaneWidth = 2.0f;
			this->viewPlaneHeight = 2.0f / aspectRatio;

			// d = (w / 2) / tan(thetah / 2)
			this->viewDist = 0.5f * viewPlaneWidth / tan(angle2radian(fov / 2));

			if (fov == 90.0f)
			{
				Vector3f pt(0,0,0);

				this->rightClipPlane = Plane3D(pt, Vector3f(1, 0, -1));
				this->leftClipPlane = Plane3D(pt, Vector3f(-1, 0, -1));
				this->topClipPlane = Plane3D(pt, Vector3f(0, 1, -1));
				this->bottomClipPlane = Plane3D(pt, Vector3f(0, -1, -1));
			}
			else
			{
				Vector3f pt(0, 0, 0);

				this->rightClipPlane = Plane3D(pt, Vector3f(viewDist, 0, -viewPlaneWidth / 2));
				this->leftClipPlane = Plane3D(pt, Vector3f(-viewDist, 0, -viewPlaneWidth / 2));
				this->topClipPlane = Plane3D(pt, Vector3f(0, viewDist, -viewPlaneWidth / 2));
				this->bottomClipPlane = Plane3D(pt, Vector3f(0, -viewDist, -viewPlaneWidth / 2));
			}
		}

		inline void Camera::BuildCameraMatrixEuler(int rotateSeq)
		{
			/* eg. For ROTATE_SEQ_YXZ	
			Mcam = Mt(-1) * My(-1) * Mx(-1) * Mz(-1)
			for reduce the calc:
				a. Mcam = (Mz * Mx * My * Mt)(-1)
				b. Rotate matrix pass an -angle, and sinx = -sinx, cos-x = cosx
			*/

			attr = MODEL_EULER;

			Matrix44f mTInver(1, 0, 0, 0,
							  0, 1, 0, 0,
							  0, 0, 1, 0,
							  -pos.x, -pos.y, -pos.z, 1);

			float thetaX = dir.x;
			float thetaY = dir.y;
			float thetaZ = dir.z;

			float cosTheta = cos(thetaX);
			float sinTheta = -sin(thetaX);
			Matrix44f mRXInver(1, 0, 0, 0,
							   0, cosTheta, sinTheta, 0,
							   0, -sinTheta, cosTheta, 0,
							   0, 0, 0, 1);

			cosTheta = cos(thetaY);
			sinTheta = -sin(thetaY);
			Matrix44f mRYInver(cosTheta, 0, -sinTheta, 0,
							   0, 1, 0, 0,
							   sinTheta, 0, cosTheta, 0,
							   0, 0, 0, 1);

			cosTheta = cos(thetaZ);
			sinTheta = -sin(thetaZ);
			Matrix44f mRZInver(cosTheta, sinTheta, 0, 0,
							   -sinTheta, cosTheta, 0, 0,
							   0, 0, 1, 0,
							   0, 0, 0, 1);

			Matrix44f mTemp, mRotate;
			switch (rotateSeq)
			{
			case ROTATE_SEQ_XYZ:
				MatrixMultiply(mTemp, mRXInver, mRYInver);
				MatrixMultiply(mRotate, mTemp, mRZInver);
				break;
			case ROTATE_SEQ_XZY:
				MatrixMultiply(mTemp, mRXInver, mRZInver);
				MatrixMultiply(mRotate, mTemp, mRYInver);
				break;
			case ROTATE_SEQ_YXZ:
				MatrixMultiply(mTemp, mRYInver, mRXInver);
				MatrixMultiply(mRotate, mTemp, mRZInver);
				break;
			case ROTATE_SEQ_YZX:
				MatrixMultiply(mTemp, mRYInver, mRZInver);
				MatrixMultiply(mRotate, mTemp, mRXInver);
				break;
			case ROTATE_SEQ_ZXY:
				MatrixMultiply(mTemp, mRZInver, mRXInver);
				MatrixMultiply(mRotate, mTemp, mRYInver);
				break;
			case ROTATE_SEQ_ZYX:
				MatrixMultiply(mTemp, mRZInver, mRYInver);
				MatrixMultiply(mRotate, mTemp, mRXInver);
				break;
			}
			MatrixMultiply(matrixCamera, mTInver, mRotate);
		}

		inline void Camera::BuildCameraMatrixUVN(int uvnType)
		{
			assert(uvnType == MODEL_UVN || uvnType == MODEL_UVNSimple);
			attr = uvnType;

			Matrix44f mTInver(1, 0, 0, 0,
							  0, 1, 0, 0,
							  0, 0, 1, 0,
							  -pos.x, -pos.y, -pos.z, 1);
			
			if (uvnType == MODEL_UVN)
			{
				float phi = dir.x;
				float theta = dir.y;

				float sinPhi = sin(phi);
				float cosPhi = cos(phi);
				float sinTheta = sin(theta);
				float cosTheta = cos(theta);

				target.x = -1 * sinPhi * sinTheta;
				target.y = cosPhi;
				target.z = sinPhi * cosTheta;
			}

			n = target - pos;
			v = Vector4f(0, 1, 0, 1);
			u = CrossProduct(v, n);	// u = v x n
			v = CrossProduct(n, u);	// v = n x u

			u.Normalize();
			v.Normalize();
			n.Normalize();

			Matrix44f mRInver(u.x, v.x, n.x, 0,
							  u.y, v.y, n.y, 0,
							  u.z, v.z, n.z, 0,
							  0, 0, 0, 1);

			MatrixMultiply(matrixCamera, mTInver, mRInver);
		}

		inline void Camera::BuildCameraToPerspective(Matrix44f& m) const
		{
			m = Matrix44f(viewDist, 0, 0, 0,
				          0, viewDist * aspectRatio, 0, 0,
						  0, 0, 1, 1,
						  0, 0, 0, 0);
		}

		inline void Camera::BuildPerspectiveToScreen(Matrix44f& m) const
		{
			// By convert perspective pos to screen pos, We want:
			// Xsceen  = alpha + Xper * alpha
			// Yscreen = beta - Yper * beta

			float alpha = 0.5f * viewportWidth - 0.5f;
			float beta  = 0.5f * viewportHeight - 0.5f;

			m = Matrix44f(alpha, 0, 0, 0,
				          0, -beta, 0, 0,
						  0, 0, 1, 0,
						  alpha, beta, 0, 1);
		}

		inline void Camera::BuildCamerToScreen(Matrix44f& m) const
		{
			// Xscreen = alpha + d * Xc / Zc
			// Yscreen = beta - d * Yc / Zc

			float alpha = 0.5f * viewportWidth - 0.5f;
			float beta  = 0.5f * viewportHeight - 0.5f;

			m = Matrix44f(viewDist, 0, 0, 0,
						  0, -viewDist, 0, 0,
						  alpha, beta, 1, 1,
						  0, 0, 0, 0);
		}
	}
}


#endif