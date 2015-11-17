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
#include "DpMesh.h"

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

	void SceneNode::OnAddNode(const SceneNodeRef& node)
	{
	}

	void SceneNode::OnRemoveNode(const SceneNodeRef& node)
	{
	}

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

}