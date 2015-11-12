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

using namespace dopixel::math;

namespace dopixel
{
	class Camera
	{
	public:
		// Camera model
		enum { MODEL_EULER, MODEL_UVN, MODEL_UVNSimple, };
		// Euler rotate seq
		enum { ROTATE_SEQ_XYZ, ROTATE_SEQ_XZY, ROTATE_SEQ_YXZ, ROTATE_SEQ_YZX, ROTATE_SEQ_ZXY, ROTATE_SEQ_ZYX };

		Camera() {}

		void InitCamera(int attr, const Vector4f& pos, const Vector4f& dir, const Vector4f& target, float nearClipZ, float farClipZ,
			float fov, float viewPlaneWidth, float viewPlaneHeight);

		void BuildCameraMatrixEuler(int rotateSeq = ROTATE_SEQ_ZYX);

		void BuildCameraMatrixUVN(int uvnType);

		// Build matrix
		void BuildCameraToPerspective(Matrix44f& m) const;

		void BuildPerspectiveToScreen(Matrix44f& m) const;

		void BuildCamerToScreen(Matrix44f& m) const;
	public:
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
	};
}


#endif