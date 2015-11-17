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

		void AddNode(const SceneNodeRef& node);
		void RemoveNode(const SceneNodeRef& node);
		void RemoveAllNodes();
		
		virtual void OnAddNode(const SceneNodeRef& node);
		virtual void OnRemoveNode(const SceneNodeRef& node);

		virtual MeshSceneNode* AsMeshNode() { return nullptr; }
	private:
		string name_;
		bool visible_;

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
