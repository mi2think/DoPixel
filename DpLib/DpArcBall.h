/********************************************************************
	created:	2016/04/28
	created:	28:4:2016   23:54
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpArcBall.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpArcBall
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Arc Ball
*********************************************************************/
#ifndef __DP_ARCBALL_H__
#define __DP_ARCBALL_H__

#include "DoPixel.h"
#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMatrix44.h"
#include "DpQuaternion.h"

namespace dopixel
{
	class ArcBall
	{
	public:
		ArcBall();
		~ArcBall();

		void Reset();

		void SetWidth(int width);
		void SetHeight(int height);

		const math::Matrix44f& GetRotationMatrix() const;

		bool OnEvent(const Event& event);

		math::Quaternion QuatFromBallPoints(const math::Vector3f& vFrom, const math::Vector3f& vTo) const;
	private:
		bool OnMousePressEvent(const MousePressEvent& event);
		bool OnMouseMoveEvent(const MouseMoveEvent& event);
		bool OnMouseReleaseEvent(const MouseReleaseEvent& event);

		math::Vector3f ScreenToVector(const math::Point& pos);

		// arc ball's width and height
		int width_;
		int height_;

		// arc ball's radius
		float radius_;

		// mouse click click
		math::Point clickPos_;
		// is drag
		bool drag_;

		// arc ball's press btn and current position
		math::Vector3f pressBtnPos_;
		math::Vector3f currentPos_;

		// arc ball' press btn and current quaternion
		math::Quaternion pressBtnQuat_;
		math::Quaternion currentQuat_;

		// arc ball' orientation
		mutable math::Matrix44f rotationMatrix_;
		mutable bool matrixValid_;
	};
}


#endif

