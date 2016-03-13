/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:55
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_camera.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_camera
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl camera
*********************************************************************/
#include "ogl_camera.h"
#include <GL/freeglut.h> // for key define

#include "DpQuaternion.h"

namespace ogl
{
#define MARGIN 10
#define EDGE_STEP 0.5f

	Camera::Camera(int windowWidth, int windowHight)
		: position_(0, 0, 0)
		, target_(0, 0, 1)
		, up_(0, 1, 0)
		, stepScale_(1.0f)
		, windowWidth_(windowWidth)
		, windowHeight_(windowHight)
		, onLEdge_(false)
		, onREdge_(false)
		, onTEdge_(false)
		, onBEdge_(false)
		, needUpdate_(false)
	{
		target_.Normalize();
		up_.Normalize();
		Init();
	}

	Camera::Camera(const Vector3f& pos, const Vector3f& target, const Vector3f& up, int windowWidth, int windowHight)
		: position_(pos)
		, target_(target)
		, up_(up)
		, stepScale_(1.0f)
		, windowWidth_(windowWidth)
		, windowHeight_(windowHight)
		, onLEdge_(false)
		, onREdge_(false)
		, onTEdge_(false)
		, onBEdge_(false)
	{
		target_.Normalize();
		up_.Normalize();
		Init();
	}

	void Camera::Init()
	{
		// horizontal angle define by the following circle
		//         90
		//          |
		// 180 -----|----- x 0
		//          |
		//          z
		//         270

		// r = 1
		// z = rsin(angle)
		// x = rcos(angle)

		// when we rotate 90 from (1, 0, 0) around (0, 1, 0), using quaternion will get (0, 0, -1)

		// asin: [-90, 90], when z in [-1, 1]
		Vector3f htarget = target_;
		htarget.y = 0;
		htarget.Normalize();

		if (htarget.z >= 0.0f)
		{
			if (htarget.x >= 0.0f)
			{
				// angle should in [270, 360]
				angleH_ = 360 - radian2angle(asin(htarget.z));
			}
			else
			{
				// angle should in [180, 270]
				angleH_ = 180 + radian2angle(asin(htarget.z));
			}
		}
		else
		{
			if (htarget.x >= 0.0f)
			{
				// angle should in [0, 90]
				angleH_ = radian2angle(asin(-htarget.z));
			}
			else
			{
				// angle should in [90, 180]
				angleH_ = 90 + radian2angle(asin(-htarget.z));
			}
		}

		// vertical angle we define when (up, down) -> [-90, 90]
		angleV_ = -radian2angle(asin(target_.y));

		// init mouse pos to center of window
		mousePos_.x = windowWidth_ / 2;
		mousePos_.y = windowHeight_ / 2;
	}

	bool Camera::OnKayboard(int key)
	{
		switch (key)
		{
		case GLUT_KEY_UP:
			position_ += target_ * stepScale_;
			break;
		case GLUT_KEY_DOWN:
			position_ -= target_ * stepScale_;
			break;
		case GLUT_KEY_LEFT:
		{
			Vector3f left = CrossProduct(target_, up_);
			left.Normalize();
			position_ += left * stepScale_;
		}
		break;
		case GLUT_KEY_RIGHT:
		{
			Vector3f right = CrossProduct(up_, target_);
			right.Normalize();
			position_ += right * stepScale_;
		}
		break;
		default:
			break;
		}
		return false;
	}

	void Camera::OnMouse(int x, int y)
	{
		const int deltaX = x - mousePos_.x;
		const int deltaY = y - mousePos_.y;

		mousePos_.x = x;
		mousePos_.y = y;

		angleH_ += (float)deltaX / 20.0f;
		angleV_ += (float)deltaY / 20.0f;

		if (deltaX == 0)
		{
			if (x <= MARGIN)
				onLEdge_ = true;
			else if (x >= windowWidth_ - MARGIN)
				onREdge_ = true;
		}
		else
		{
			onLEdge_ = false;
			onREdge_ = false;
		}

		if (deltaY == 0)
		{
			if (y <= MARGIN)
				onTEdge_ = true;
			else if (y >= windowHeight_ - MARGIN)
				onBEdge_ = true;
		}
		else
		{
			onTEdge_ = false;
			onBEdge_ = false;
		}

		Update();
	}

	void Camera::Update()
	{
		// vertical axis
		const Vector3f VAxis(0.0f, 1.0f, 0.0f);

		// rotate the view vector by the horizontal angle around the vertical axis
		Vector3f view(1.0f, 0.0f, 0.0f);
		view = QuaternionRotateVector(view, VAxis, angle2radian(angleH_));

		// rotate the view vector by the vertical angle around the horizontal axis
		Vector3f HAxis = CrossProduct(VAxis, view);
		HAxis.Normalize();
		view = QuaternionRotateVector(view, HAxis, angle2radian(angleV_));

		target_ = view;
		target_.Normalize();

		up_ = CrossProduct(target_, HAxis);
		up_.Normalize();
	}

	void Camera::OnRender()
	{
		bool shouldUpdate = false;

		if (onLEdge_)
			angleH_ -= EDGE_STEP;
		else if (onREdge_)
			angleH_ += EDGE_STEP;

		shouldUpdate = (onLEdge_ || onREdge_);

		if (onTEdge_)
		{
			if (angleV_ > -90.0f)
			{
				angleV_ -= EDGE_STEP;
				shouldUpdate = true;
			}
		}
		else if (onBEdge_)
		{
			if (angleV_ < 90.0f)
			{
				angleV_ += EDGE_STEP;
				shouldUpdate = true;
			}
		}

		if (shouldUpdate)
		{
			Update();
		}
	}

}