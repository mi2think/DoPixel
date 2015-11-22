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
#include "DpGeometry.h"

namespace dopixel
{
	enum class SceneNodeType
	{
		Unknown,
		MeshNode,
		CameraNode,
		LightNode
	};

	class MeshSceneNode;
	class CameraSceneNode;
	class LightSceneNode;

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
		virtual void SetPosition(const math::Vector3f& position);
		virtual void SetRotation(const math::Vector3f& rotation);
		virtual void SetScale(const math::Vector3f& scale);
		const math::Vector3f& GetPosotion() const;
		const math::Vector3f& GetRotation() const;
		const math::Vector3f& GetScale() const;

		const math::Matrix44f& GetWorldMatrix() const;
		math::Vector3f GetWorldPosition() const;
		void UpdateWorldMatrix();

		void AddNode(const SceneNodeRef& node);
		void RemoveNode(const SceneNodeRef& node);
		void RemoveAllNodes();
		
		void AddAnimator(const SceneNodeAnimatorRef& animator);
		void RemoveAnimator(const SceneNodeAnimatorRef& animator);
		void RemoveAllAnimators();

		virtual void OnAddNode(const SceneNodeRef& node);
		virtual void OnRemoveNode(const SceneNodeRef& node);

		virtual void OnRegisterSceneNode(SceneManager* manager);
		virtual void OnUpdate(const Timestep& timestep);
		virtual bool OnEvent(const Event& event);
		virtual void OnRender(const Renderer& renderer) = 0;

		virtual MeshSceneNode* AsMeshNode() { return nullptr; }
		virtual CameraSceneNode* AsCameraNode() { return nullptr; }
		virtual LightSceneNode* AsLightNode() { return nullptr; }
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

		const MeshRef& GetMesh() const { return mesh_; }
		const math::AABB& GetBoundingBox() const;

		virtual void OnRegisterSceneNode(SceneManager* manager);
		virtual void OnRender(Renderer& renderer);

		virtual MeshSceneNode* AsMeshNode() { return this; }
	private:
		virtual void Internal();

		MeshRef mesh_;
		math::AABB aabb_;
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
		const math::Frustum& GetViewFrustum() const;
		const CameraRef& GetCamera() const;

		virtual void SetPosition(const math::Vector3f& position);
		virtual void SetRotation(const math::Vector3f& rotation);

		void UpdateViewMatrix();
		void UpdateViewFrustum();

		virtual void OnRegisterSceneNode(SceneManager* manager);
		virtual void OnRender(Renderer& renderer);

		virtual CameraSceneNode* AsCameraNode() { return this; }
	private:
		CameraRef camera_;
		// note: SceneNode has position 
		math::Vector3f target_;
		math::Vector3f up_;
		// view matrix
		math::Matrix44f viewMatrix_;
		// view frustum
		math::Frustum viewFrustum_;
	};

	class LightSceneNode : public SceneNode
	{
	public:
		LightSceneNode(const string& name, const LightRef light);
		~LightSceneNode();

		const LightRef& GetLight() const;

		void SetOpenLight(bool open);
		bool GetOpenLight() const;

		virtual void OnRegisterSceneNode(SceneManager* manager);
		virtual void OnRender(Renderer& renderer);

		virtual LightSceneNode* AsLightNode() { return this; }
	private:
		LightRef light_;
		bool open_;
	};
}

#endif
