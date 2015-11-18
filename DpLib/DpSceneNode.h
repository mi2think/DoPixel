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
	class CameraSceneNode;

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

		// relative to parent
		virtual void SetPosition(const math::Vector3f& position) { position_ = position; Internal(); }
		virtual void SetRotation(const math::Vector3f& rotation) { rotation_ = rotation; Internal(); }
		virtual void SetScale(const math::Vector3f& scale) { scale_ = scale; Internal(); }
		const math::Vector3f& GetPosotion() const { return position_; }
		const math::Vector3f& GetRotation() const { return rotation_; }
		const math::Vector3f& GetScale() const { return scale_; }

		const math::Matrix44f& GetWorldMatrix() const { return worldMatrix_; }
		math::Vector3f GetWorldPosition() const { return worldMatrix_.GetTranslation(); }
		void UpdateWorldMatrix();

		void AddNode(const SceneNodeRef& node);
		void RemoveNode(const SceneNodeRef& node);
		void RemoveAllNodes();
		
		void AddAnimator(const SceneNodeAnimatorRef& animator);
		void RemoveAnimator(const SceneNodeAnimatorRef& animator);
		void RemoveAllAnimators();

		virtual void OnAddNode(const SceneNodeRef& node);
		virtual void OnRemoveNode(const SceneNodeRef& node);

		virtual void OnAnimate(const Timestep& timestep);
		virtual bool OnEvent(const Event& event);

		virtual MeshSceneNode* AsMeshNode() { return nullptr; }
		virtual CameraSceneNode* AsCameraNode() { return nullptr; }
	protected:
		virtual void Internal();

		string name_;
		bool visible_;
		// relative to parent
		math::Vector3f position_;
		math::Vector3f rotation_;
		math::Vector3f scale_;
		math::Matrix44f relativeMatrix_;
		// absolute!
		math::Matrix44f worldMatrix_;
		// relations
		SceneNode* parent_;
		vector<SceneNodeRef> children_;
		// animators
		vector<SceneNodeAnimatorRef> animators_;
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

	class CameraSceneNode : public SceneNode
	{
	public:
		CameraSceneNode(const string& name, const CameraRef& camera,
			const math::Vector3f& position, const math::Vector3f& target);
		~CameraSceneNode();

		void SetTarget(const math::Vector3f& target);
		void SetUp(const math::Vector3f& up);
		const math::Vector3f& GetTarget() const;
		const math::Vector3f& GetUp() const;

		const math::Matrix44f& GetViewMatrix() const;
		const math::Matrix44f& GetProjectionMatrix() const;
		const CameraRef& GetCamera() const;

		virtual void SetPosition(const math::Vector3f& position);
		virtual void SetRotation(const math::Vector3f& rotation);

		void UpdateViewMatrix();

		virtual CameraSceneNode* AsCameraNode() { return this; }
	private:
		CameraRef camera_;
		// note: SceneNode has position 
		math::Vector3f target_;
		math::Vector3f up_;
		// view matrix
		math::Matrix44f viewMatrix_;
	};
}

#endif
