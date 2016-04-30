/********************************************************************
	created:	2016/04/27
	created:	27:4:2016   16:05
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpCameraController.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpCameraController
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Camera Controller
*********************************************************************/
#include "DpCameraController.h"
#include "DpCamera.h"
#include "DpMouseEvent.h"

namespace dopixel
{
	CameraController::CameraController()
		: dirtyBits_(NoDirty)
		, up_(0, 1, 0)
	{
	}

	CameraController::~CameraController()
	{
	}

	void CameraController::Attach(const CameraRef& camera)
	{
		camera_ = camera;
		dirtyBits_ |= (ProjMarixDirty | ViewFrustumDirty);
	}

	void CameraController::Detach()
	{
		camera_ = nullptr;
	}

	const math::Matrix44f& CameraController::GetViewMatrix() const
	{
		if (dirtyBits_ & ViewMarixDirty)
		{
			dirtyBits_ &= ~ViewMarixDirty;
			UpdateViewMatrix();
		}
		return viewMatrix_;
	}

	void CameraController::SetEyePos(const math::Vector3f& eye)
	{
		SetView(eye, lookAt_, up_);
	}

	void CameraController::SetLookAt(const math::Vector3f& lookAt)
	{
		SetView(eye_, lookAt, up_);
	}

	void CameraController::SetUp(const math::Vector3f& up)
	{
		SetView(eye_, lookAt_, up);
	}

	void CameraController::SetView(const math::Vector3f& eye, const math::Vector3f& lookAt, const math::Vector3f& up)
	{
		eye_ = eye;
		lookAt_ = lookAt;
		up_ = up;
		dirtyBits_ |= (ViewMarixDirty | ProjMarixDirty);

		PostSetView();
	}

	void CameraController::SetPerspective(float fovy, float aspect, float near, float far)
	{
		return camera_->SetPerspective(fovy, aspect, near, far);
	}

	const math::Vector3f& CameraController::GetEyePos() const
	{
		return eye_;
	}

	const math::Matrix44f& CameraController::GetProjMatrix() const
	{
		ASSERT(camera_ != nullptr);
		return camera_->GetProjectionMatrix();
	}

	const math::Frustum& CameraController::GetViewFrustum() const
	{
		ASSERT(camera_ != nullptr);
		if (dirtyBits_ & ViewFrustumDirty)
		{
			dirtyBits_ &= ~ViewFrustumDirty;
			UpdateViewFrustum();
		}
		return viewFrustum_;
	}

	void CameraController::UpdateViewMatrix() const
	{
		math::Matrix44f translationTrans;
		MaxtrixTranslation(translationTrans, -eye_);

		// n: target
		math::Vector3f n = lookAt_ - eye_;
		n.Normalize();
		// v: up
		math::Vector3f v = up_;
		v.Normalize();
		// u: right
		// u = v x n
		math::Vector3f u = CrossProduct(v, n);
		// v = n x u
		v = CrossProduct(n, u);

		math::Matrix44f rotateTrans;
		auto& m = rotateTrans.m;
		m[0][0] = u.x;   m[0][1] = v.x;   m[0][2] = n.x;   m[0][3] = 0.0f;
		m[1][0] = u.y;   m[1][1] = v.y;   m[1][2] = n.y;   m[1][3] = 0.0f;
		m[2][0] = u.z;   m[2][1] = v.z;   m[2][2] = n.z;   m[2][3] = 0.0f;
		m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;

		math::MatrixMultiply(viewMatrix_, translationTrans, rotateTrans);
	}

	void CameraController::UpdateViewFrustum() const
	{
		math::Matrix44f viewProject;
		math::MatrixMultiply(viewProject, GetViewMatrix(), GetProjMatrix());
		viewFrustum_.ExtractFrustum(viewProject);
	}

	//////////////////////////////////////////////////////////////////////////

	ModelViewCameraController::ModelViewCameraController()
		: wheelDelta_(0.0f)
		, radius_(5.0f)
		, minRadius_(1.0f)
		, maxRadius_(FLT_MAX)
		, update_(false)
	{

	}

	ModelViewCameraController::~ModelViewCameraController()
	{

	}

	void ModelViewCameraController::PostSetView()
	{
		radius_ = math::Distance(eye_, lookAt_);
	}

	void ModelViewCameraController::SetRadius(float fDefaultRadius, float fMinRadius, float fMaxRadius)
	{
		radius_ = fDefaultRadius;
		minRadius_ = fMinRadius;
		maxRadius_ = fMaxRadius;
	}

	void ModelViewCameraController::SetWindow(int width, int height)
	{
		arcBall_.SetWidth(width);
		arcBall_.SetHeight(height);
	}

	void ModelViewCameraController::OnUpdate(const Timestep& timestep)
	{
		if (!update_)
			return;
		update_ = false;

		// zoom
		if (! math::Equal(wheelDelta_, 0.0f))
		{
			radius_ -= wheelDelta_ * radius_ * 0.1f / 3;
			radius_ = math::Clamp(radius_, minRadius_, maxRadius_);
		}
		wheelDelta_ = 0;

		// get model rotate matrix
		math::Matrix44f modelRotateMatrix = arcBall_.GetRotationMatrix();
		// orthonormalize matrix
		math::Vector3f* xAixs = (math::Vector3f*)&modelRotateMatrix.m11;
		math::Vector3f* yAixs = (math::Vector3f*)&modelRotateMatrix.m21;
		math::Vector3f* zAixs = (math::Vector3f*)&modelRotateMatrix.m31;
		xAixs->Normalize();
		*yAixs = CrossProduct(*zAixs, *xAixs);
		yAixs->Normalize();
		*zAixs = CrossProduct(*xAixs, *yAixs);

		// arc ball just rotate model, we want rotate camera
		// so we need inverse matrix
		math::Matrix44f cameraRoateMatrix;
		math::MatrixInverse(cameraRoateMatrix, modelRotateMatrix);

		math::Vector4f up(0.0f, 1.0f, 0.0f, 1.0f);
		up *= cameraRoateMatrix;
		up_.x = up.x;
		up_.y = up.y;
		up_.z = up.z;

		math::Vector4f ahead(0.0f, 0.0f, 1.0f, 1.0f);
		ahead *= cameraRoateMatrix;

		eye_ = lookAt_ - math::Vector3f(ahead.x, ahead.y, ahead.z) * radius_;
		GetViewMatrix();

		LOG_INFO("eye:%f,%f,%f	radius: %f", eye_.x, eye_.y, eye_.z, radius_);
	}

	bool ModelViewCameraController::OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &ModelViewCameraController::OnMouseWheelEvent);

		if (arcBall_.OnEvent(event) || dispatch.GetResult())
		{
			dirtyBits_ |= (ViewMarixDirty | ProjMarixDirty);
			update_ = true;
		}
		return true;
	}

	bool ModelViewCameraController::OnMouseWheelEvent(const MouseWheelEvent& event)
	{
		wheelDelta_ += event.GetWheelDelta();
		return true;
	}
}