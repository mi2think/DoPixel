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

#include "DoPixel.h"
#include "DpMatrix44.h"

namespace dopixel
{
	class Camera
	{
	public:
		Camera();

		float GetFovy() const;
		float GetAspectRatio() const;
		float GetNearClip() const;
		float GetFarClip() const;

		void SetFovy(float fovy);
		void SetAspectRatio(float aspect);
		void SetNearClip(float near);
		void SetFarClip(float far);
		void SetPerspective(float fovy, float aspect, float near, float far);

		const math::Matrix44f& GetProjectionMatrix() const;
	private:
		// field of view, angle
		float fovy_;
		// aspect ratio
		float aspect_;
		// near z
		float znear_;
		// far z
		float zfar_;
		// projection matrix
		mutable math::Matrix44f projectionMatrix_;
		mutable bool matrixValid_;
	};
}

#endif