/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   15:40
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpCamera.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpCamera
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Camera
*********************************************************************/
#include "DpCamera.h"
#include <cassert>

namespace dopixel
{
	namespace core
	{

		void Camera::InitCamera(int attr, const Vector4f& pos, const Vector4f& dir, const Vector4f& target, float nearClipZ, float farClipZ, float fov,
			float viewportWidth, float viewportHeight)
		{
			this->attr = attr;
			this->pos = pos;
			this->dir = dir;

			this->u = Vector4f(1, 0, 0, 1);	// +X
			this->v = Vector4f(0, 1, 0, 1);	// +Y
			this->n = Vector4f(0, 0, 1, 1);	// +Z

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
				Vector3f pt(0, 0, 0);

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

		void Camera::BuildCameraMatrixEuler(int rotateSeq)
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

		void Camera::BuildCameraMatrixUVN(int uvnType)
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

		void Camera::BuildCameraToPerspective(Matrix44f& m) const
		{
			float k = 1.0f / tan(angle2radian(fov * 0.5f));

			m = Matrix44f(k / aspectRatio, 0, 0, 0,
				0, k, 0, 0,
				0, 0, 1, 1,
				0, 0, 0, 0);
		}

		void Camera::BuildPerspectiveToScreen(Matrix44f& m) const
		{
			float halfOfWidth = viewportWidth * 0.5f;
			float halfOfHeight = viewportHeight * 0.5f;

			m = Matrix44f(halfOfWidth, 0, 0, 0,
				0, -halfOfHeight, 0, 0,
				0, 0, 1, 0,
				halfOfWidth, halfOfHeight, 0, 1);
		}

		void Camera::BuildCamerToScreen(Matrix44f& m) const
		{
			Matrix44f m1;
			BuildCameraToPerspective(m1);
			Matrix44f m2;
			BuildPerspectiveToScreen(m2);

			MatrixMultiply(m, m1, m2);
		}


	}
}