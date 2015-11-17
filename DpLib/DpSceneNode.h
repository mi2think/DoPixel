/********************************************************************
	created:	2015/11/17
	created:	17:11:2015   14:48
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpSceneNode.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpSceneNode
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Scene Node
*********************************************************************/
#ifndef __DP_SCENE_NODE_H__
#define __DP_SCENE_NODE_H__

#include "DoPixel.h"
#include "DpAABB.h"
#include "DpVector3.h"
#include "DpMatrix44.h"

namespace dopixel
{
	class MeshSceneNode;
	class SceneNode
	{
	public:
		SceneNode(const string& name);
		virtual ~SceneNode();
		
		const string& GetName() const { return name_; }

		bool GetVisible() const { return visible_; }
		bool SetVisible(bool visible) { visible_ = visible; }
		// visible only if parent is visible
		bool GetTrulyVisible() const;

		void SetPosition(const math::Vector3f& position) { position_ = position; Internal(); }
		void SetRotation(const math::Vector3f& rotation) { rotation_ = rotation; Internal(); }
		void SetScale(const math::Vector3f& scale) { scale_ = scale; Internal(); }
		const math::Vector3f& GetPosotion() const { return position_; }
		const math::Vector3f& GetRotation() const { return rotation_; }
		const math::Vector3f& GetScale() const { return scale_; }

		const math::Matrix44f GetRelativeMatrix() const { return relativeMatrix_; }
		const math::Matrix44f& GetWorldMatrix() const { return worldMatrix_; }
		math::Vector3f GetWorldPosition() const { return worldMatrix_.GetTranslation(); }
		void UpdateWorldMatrix();

		void AddNode(const SceneNodeRef& node);
		void RemoveNode(const SceneNodeRef& node);
		void RemoveAllNodes();
		
		virtual void OnAddNode(const SceneNodeRef& node);
		virtual void OnRemoveNode(const SceneNodeRef& node);

		virtual MeshSceneNode* AsMeshNode() { return nullptr; }
	private:
		void Internal();

		string name_;
		bool visible_;
		// relative to parent
		math::Vector3f position_;
		math::Vector3f rotation_;
		math::Vector3f scale_;
		math::Matrix44f relativeMatrix_;
		// absolute!
		math::Matrix44f worldMatrix_;

		SceneNode* parent_;
		vector<SceneNodeRef> children_;
	};

	class MeshSceneNode : public SceneNode
	{
	public:
		MeshSceneNode(const string& name, const string& meshPath);
		~MeshSceneNode();

		const math::AABB& GetBoundingBox() const;
		const MeshRef& GetMesh() const { return mesh_; }

		virtual MeshSceneNode* AsMeshNode() { return this; }
	private:
		MeshRef mesh_;
	};
}

#endif
