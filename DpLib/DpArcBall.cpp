/********************************************************************
	created:	2016/04/28
	created:	28:4:2016   23:54
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpArcBall.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpArcBall
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Arc Ball
*********************************************************************/
#include "DpArcBall.h"
#include "DpKeyEvent.h"
#include "DpMouseEvent.h"

namespace dopixel
{
	ArcBall::ArcBall()
		: width_(0)
		, height_(0)
		, radius_(1.0f)
		, drag_(false)
		, matrixValid_(true)
	{
		Reset();
	}

	ArcBall::~ArcBall()
	{

	}

	void ArcBall::Reset()
	{
		pressBtnQuat_.Identity();
		currentQuat_.Identity();
		rotationMatrix_.Identity();
		drag_ = false;
		radius_ = 1.0f;
	}

	void ArcBall::SetWidth(int width)
	{
		width_ = width;
	}

	void ArcBall::SetHeight(int height)
	{
		height_ = height;
	}

	void ArcBall::SetRadius(float radius)
	{
		radius_ = radius;
	}

	bool ArcBall::OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &ArcBall::OnMousePressEvent);
		dispatch.Dispatch(this, &ArcBall::OnMouseMoveEvent);
		dispatch.Dispatch(this, &ArcBall::OnMouseReleaseEvent);
		return dispatch.GetResult();
	}

	bool ArcBall::OnMousePressEvent(const MousePressEvent& event)
	{
		int x = event.GetX();
		int y = event.GetY();

		clickPos_.x = x;
		clickPos_.y = y;

		// begin to drag
		if (event.GetButtonType() == MOUSE_LBUTTON)
		{
			if (x >= 0 && x < width_ && y >= 0 && y < height_)
			{
				drag_ = true;
				pressBtnQuat_ = currentQuat_;
				pressBtnPos_ = ScreenToVector(clickPos_);
			}
		}

		return false;
	}

	bool ArcBall::OnMouseMoveEvent(const MouseMoveEvent& event)
	{
		int x = event.GetX();
		int y = event.GetY();

		if (drag_)
		{
			currentPos_ = ScreenToVector(math::Point(x, y));
			currentQuat_ = pressBtnQuat_ * QuatFromBallPoints(pressBtnPos_, currentPos_);
			matrixValid_ = false;
			return true;
		}
		return false;
	}

	bool ArcBall::OnMouseReleaseEvent(const MouseReleaseEvent& event)
	{
		// end drag
		if (event.GetButtonType() == MOUSE_LBUTTON && drag_)
		{
			drag_ = false;
		}
		return false;
	}

	const math::Matrix44f& ArcBall::GetRotationMatrix() const
	{
		if (!matrixValid_)
		{
			math::MatrixRotationQuaternion(rotationMatrix_, currentQuat_);
			matrixValid_ = true;
		}
		return rotationMatrix_;
	}

	math::Vector3f ArcBall::ScreenToVector(const math::Point& pos)
	{
		// using half arc ball outside screen
		// so dir of drag is same with dir of rotation

		// convert screen position to point on arc ball
		float halfWidth = width_ / 2.0f;
		float halfHeight = height_ / 2.0f;

		float x = -(pos.x - halfWidth) / (radius_ * halfWidth);
		float y = (pos.y - halfHeight) / (radius_ * halfHeight);
		float z = 0.0f;

		float mag = x * x + y * y;
		if (mag > 1.0f)
		{
			float scale = 1.0f / sqrtf(mag);
			x *= scale;
			y *= scale;
		}
		else
			z = sqrtf(1.0f - mag);

		return math::Vector3f(x, y, z);
	}

	math::Quaternion ArcBall::QuatFromBallPoints(const math::Vector3f& vFrom, const math::Vector3f& vTo) const
	{
		// think about range of x in screen space: [0, width]
		// map it to arc ball by ScreenToVector:   [-1, 1]
		// it rotate 180 in arc ball
		// we want model rotate 360, so we need double rotate angle

		// for rotate theta by axis v, quaternion is: [sin(theta/2) * v, cos(theta/2)]
		// we need double angle, so: [sin(theta) * v, cos(theta)]

		math::Vector3f axis = math::CrossProduct(vFrom, vTo);
		float dp = math::DotProduct(vFrom, vTo);
		math::Quaternion q = math::Quaternion(axis.x, axis.y, axis.z, dp);
		return q;
	}
}