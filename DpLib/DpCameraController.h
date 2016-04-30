/********************************************************************
	created:	2016/04/27
	created:	27:4:2016   15:53
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpCameraController.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpCameraController
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Camera Controller
*********************************************************************/
#ifndef __DP_CAMERA_CONTROLLER_H__
#define __DP_CAMERA_CONTROLLER_H__

#include "DoPixel.h"
#include "DpVector3.h"
#include "DpMatrix44.h"
#include "DpArcBall.h"
#include "DpGeometry.h"

namespace dopixel
{
	class CameraController
	{
	public:
		CameraController();
		virtual ~CameraController();

		void Attach(const CameraRef& camera);
		void Detach();

		void SetEyePos(const math::Vector3f& eye);
		void SetLookAt(const math::Vector3f& lookAt);
		void SetUp(const math::Vector3f& up);

		const math::Vector3f& GetEyePos() const;

		void SetView(const math::Vector3f& eye, const math::Vector3f& lookAt, const math::Vector3f& up);
		void SetPerspective(float fovy, float aspect, float near, float far);

		const math::Matrix44f& GetViewMatrix() const;
		const math::Matrix44f& GetProjMatrix() const;
		const math::Frustum& GetViewFrustum() const;

		virtual void SetRadius(float fDefaultRadius, float fMinRadius, float fMaxRadius) {}
		virtual void SetWindow(int width, int height) {}
		virtual void PostSetView() {}

		virtual void OnUpdate(const Timestep& timestep) {}
		virtual bool OnEvent(const Event& event) { return false; }
	protected:
		void UpdateViewMatrix() const;
		void UpdateViewFrustum() const;

		CameraRef camera_;
		// position
		math::Vector3f eye_;
		math::Vector3f lookAt_;
		math::Vector3f up_;
		// view matrix
		mutable math::Matrix44f viewMatrix_;
		// view frustum
		mutable math::Frustum viewFrustum_;

		enum DirtyBits
		{
			NoDirty = 0x0,
			ViewMarixDirty = 0x1,
			ProjMarixDirty = 0x2,
			ViewFrustumDirty = 0x4,
			AllDirty = ViewMarixDirty | ProjMarixDirty | ViewFrustumDirty
		};
		mutable int dirtyBits_;
	};

	class ModelViewCameraController : public CameraController
	{
	public:
		ModelViewCameraController();
		~ModelViewCameraController();

		virtual void PostSetView() override;
		virtual void SetRadius(float fDefaultRadius, float fMinRadius, float fMaxRadius);
		virtual void SetWindow(int width, int height) override;
		virtual void OnUpdate(const Timestep& timestep) override;
		virtual bool OnEvent(const Event& event) override;
	private:
		bool OnMouseWheelEvent(const MouseWheelEvent& event);

		ArcBall arcBall_;
		float wheelDelta_;
		// from camera to model
		float radius_;
		float minRadius_;
		float maxRadius_;
		bool update_;
	};
}

#endif