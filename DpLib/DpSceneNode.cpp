/********************************************************************
	created:	2015/11/17
	created:	17:11:2015   20:15
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpSceneNode.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpSceneNode
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Scene Node
*********************************************************************/
#include "DpSceneNode.h"
#include "DpSceneNodeAnimator.h"
#include "DpCamera.h"
#include "DpMesh.h"
#include "DpSceneManager.h"

namespace dopixel
{
	SceneNode::SceneNode(const string& name)
		: name_(name)
		, parent_(nullptr)
		, visible_(true)
		, position_(0.0f, 0.0f, 0.0f)
		, rotation_(0.0f, 0.0f, 0.0f)
		, scale_(1.0f, 1.0f, 1.0f)
	{
		relativeMatrix_.Identity();
		worldMatrix_.Identity();
	}

	SceneNode::~SceneNode()
	{
		RemoveAllNodes();
	}

	bool SceneNode::GetTrulyVisible() const
	{
		return visible_ && (parent_ != nullptr ? parent_->GetTrulyVisible() : true);
	}

	void SceneNode::SetPosition(const math::Vector3f& position)
	{
		position_ = position;
		Internal();
	}

	void SceneNode::SetRotation(const math::Vector3f& rotation)
	{ 
		rotation_ = rotation;
		Internal();
	}

	void SceneNode::SetScale(const math::Vector3f& scale)
	{
		scale_ = scale;
		Internal();
	}

	const math::Vector3f& SceneNode::GetPosotion() const
	{
		return position_;
	}

	const math::Vector3f& SceneNode::GetRotation() const
	{
		return rotation_;
	}

	const math::Vector3f& SceneNode::GetScale() const
	{
		return scale_;
	}

	void SceneNode::Internal()
	{
		// relative matrix
		relativeMatrix_.Identity();
		MatrixRotationZYX(relativeMatrix_, angle2radian(rotation_.x), angle2radian(rotation_.y), angle2radian(rotation_.z));
		relativeMatrix_.SetTranslation(position_);	
		math::Matrix44f t;
		MatrixScaling(t, scale_);
		relativeMatrix_ *= t;

		// world matrix
		UpdateWorldMatrix();
	}

	void SceneNode::UpdateWorldMatrix()
	{
		if (parent_)
			worldMatrix_ = parent_->GetWorldMatrix() * relativeMatrix_;
		else
			worldMatrix_ = relativeMatrix_;
		// update children
		for (auto& node : children_)
		{
			node->UpdateWorldMatrix();
		}
	}

	const math::Matrix44f& SceneNode::GetWorldMatrix() const
	{
		return worldMatrix_;
	}

	math::Vector3f SceneNode::GetWorldPosition() const
	{
		return worldMatrix_.GetTranslation();
	}

	void SceneNode::AddNode(const SceneNodeRef& node)
	{
		auto it = std::find(children_.begin(), children_.end(), node);
		if (it == children_.end())
		{
			node->parent_ = this;
			children_.push_back(node);
			OnAddNode(node);
		}
	}

	void SceneNode::RemoveNode(const SceneNodeRef& node)
	{
		auto it = std::find(children_.begin(), children_.end(), node);
		if (it != children_.end())
		{
			(*it)->parent_ = nullptr;
			OnRemoveNode((*it));
			children_.erase(it);
		}
	}

	void SceneNode::RemoveAllNodes()
	{
		for (auto& node : children_)
		{
			node->parent_ = nullptr;
			OnRemoveNode(node);
		}
		children_.clear();
	}

	void SceneNode::AddAnimator(const SceneNodeAnimatorRef& animator)
	{
		auto it = std::find(animators_.begin(), animators_.end(), animator);
		if (it == animators_.end())
		{
			animators_.push_back(animator);
		}
	}

	void SceneNode::RemoveAnimator(const SceneNodeAnimatorRef& animator)
	{
		auto it = std::find(animators_.begin(), animators_.end(), animator);
		if (it != animators_.end())
		{
			animators_.erase(it);
		}
	}

	void SceneNode::RemoveAllAnimators()
	{
		animators_.clear();
	}


	void SceneNode::OnAddNode(const SceneNodeRef& node)
	{
	}

	void SceneNode::OnRemoveNode(const SceneNodeRef& node)
	{
	}

	void SceneNode::OnAnimate(const Timestep& timestep)
	{
		if (visible_)
		{
			bool results = false;
			for (auto& animator : animators_)
			{
				results |= animator->AnimateNode(this, timestep);
			}

			// update world matrix
			if (results)
			{
				Internal();
			}

			// children may have animator
			for (auto& node : children_)
			{
				node->OnAnimate(timestep);
			}
		}
	}

	bool SceneNode::OnEvent(const Event& event)
	{
		for (auto& animator : animators_)
		{
			if (animator->OnEvent(event))
				return true;
		}
		return false;
	}

	void SceneNode::OnRegisterSceneNode(SceneManager* manager)
	{
		for (auto& node : children_)
		{
			node->OnRegisterSceneNode(manager);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	MeshSceneNode::MeshSceneNode(const string& name, const string& meshPath)
		: SceneNode(name)
	{
		mesh_ = MeshCache::Instance().GetMesh(meshPath);
	}

	MeshSceneNode::~MeshSceneNode()
	{
	}

	const math::AABB& MeshSceneNode::GetBoundingBox() const
	{
		ASSERT(mesh_ != nullptr);
		return mesh_->GetBoundingBox();
	}

	void MeshSceneNode::OnRegisterSceneNode(SceneManager* manager)
	{
		if (visible_)
		{
			manager->RegisterSceneNode(this, SceneNodeType::MeshNode);
			SceneNode::OnRegisterSceneNode(manager);
		}
	}

	void MeshSceneNode::OnRender(Renderer& renderer)
	{
		if (mesh_)
		{
			int submeshCount = mesh_->GetSubMeshCount();
			for (int i = 0; i < submeshCount; ++i)
			{
				const auto& submesh = mesh_->GetSubMesh(i);
				//TODO:
				//render sub mesh
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	CameraSceneNode::CameraSceneNode(const string& name, const CameraRef& camera,
		const math::Vector3f& position, const math::Vector3f& target)
		: SceneNode(name)
		, camera_(camera)
		, target_(target)
		, up_(0.0f, 1.0f, 0.0f)
	{
		SetPosition(position);
	}

	CameraSceneNode::~CameraSceneNode()
	{
	}

	void CameraSceneNode::SetTarget(const math::Vector3f& target)
	{
		target_ = target;
		UpdateViewMatrix();
		// TODO:
		// if we bind target and rotation, we need change rotation too.
	}

	void CameraSceneNode::SetUp(const math::Vector3f& up)
	{
		up_ = up;
		UpdateViewMatrix();
	}

	const math::Vector3f& CameraSceneNode::GetTarget() const
	{
		return target_;
	}

	const math::Vector3f& CameraSceneNode::GetUp() const
	{
		return up_;
	}

	const math::Matrix44f& CameraSceneNode::GetViewMatrix() const
	{
		return viewMatrix_;
	}

	const math::Matrix44f& CameraSceneNode::GetProjectionMatrix() const
	{
		return camera_->GetProjectionMatrix();
	}

	const CameraRef& CameraSceneNode::GetCamera() const
	{
		return camera_;
	}

	void CameraSceneNode::SetPosition(const math::Vector3f& position)
	{
		SceneNode::SetPosition(position);
		UpdateViewMatrix();
	}

	void CameraSceneNode::SetRotation(const math::Vector3f& rotation)
	{
		SceneNode::SetRotation(rotation);
		// TODO:
		// if we bind target and rotation, we need change target 
		// and then update view matrix too.
	}

	void CameraSceneNode::UpdateViewMatrix()
	{
		Matrix44f translationTrans;
		MaxtrixTranslation(translationTrans, -GetWorldPosition());

		// n: target
		Vector3f n = target_;
		n.Normalize();
		// v: up
		Vector3f v = up_;
		v.Normalize();
		// u: right
		// u = v x n
		Vector3f u = CrossProduct(v, n);
		// v = n x u
		v = CrossProduct(n, u);

		Matrix44f rotateTrans;
		auto& m = rotateTrans.m;
		m[0][0] = u.x;   m[0][1] = v.x;   m[0][2] = n.x;   m[0][3] = 0.0f;
		m[1][0] = u.y;   m[1][1] = v.y;   m[1][2] = n.y;   m[1][3] = 0.0f;
		m[2][0] = u.z;   m[2][1] = v.z;   m[2][2] = n.z;   m[2][3] = 0.0f;
		m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;

		MatrixMultiply(viewMatrix_, translationTrans, rotateTrans);
	}

	void CameraSceneNode::OnRegisterSceneNode(SceneManager* manager)
	{
		if (visible_)
		{
			manager->RegisterSceneNode(this, SceneNodeType::CameraNode);
			SceneNode::OnRegisterSceneNode(manager);
		}
	}

	void CameraSceneNode::OnRender(Renderer& renderer)
	{
	}
}