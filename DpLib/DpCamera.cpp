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

namespace dopixel
{
	Camera::Camera()
		: fovy_(0.0f)
		, aspect_(0.0f)
		, znear_(0.0f)
		, zfar_(0.0f)
		, matrixValid_(false)
	{
	}

	float Camera::GetFovy() const
	{
		return fovy_;
	}

	float Camera::GetAspectRatio() const
	{
		return aspect_;
	}

	float Camera::GetNearClip() const
	{
		return znear_;
	}

	float Camera::GetFarClip() const
	{
		return zfar_;
	}

	void Camera::SetFovy(float fovy)
	{
		fovy_ = fovy;
		matrixValid_ = false;
	}

	void Camera::SetAspectRatio(float aspect)
	{
		aspect_ = aspect;
		matrixValid_ = false;
	}

	void Camera::SetNearClip(float near)
	{
		znear_ = near;
		matrixValid_ = false;
	}

	void Camera::SetFarClip(float far)
	{
		zfar_ = far;
		matrixValid_ = false;
	}

	void Camera::SetPerspective(float fovy, float aspect, float near, float far)
	{
		fovy_ = fovy;
		aspect_ = aspect;
		znear_ = near;
		zfar_ = far;
		matrixValid_ = false;
	}

	const math::Matrix44f& Camera::GetProjectionMatrix() const
	{
		if (!matrixValid_)
		{
			float k = 1.0f / tan(angle2radian(fovy_ * 0.5f));
			float zdist = zfar_ - znear_;

			projectionMatrix_ = math::Matrix44f(k / aspect_, 0, 0, 0,
				0, k, 0, 0,
				0, 0, zfar_ / zdist, 1,
				0, 0, -zfar_ * znear_ / zdist, 0);

			matrixValid_ = true;
		}

		return projectionMatrix_;
	}
}