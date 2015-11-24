/********************************************************************
	created:	2015/11/17
	created:	17:11:2015   14:50
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpSceneManager.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpSceneManager
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Scene Manager
*********************************************************************/
#ifndef __DP_SCENE_MANAGER_H__
#define __DP_SCENE_MANAGER_H__

#include "DoPixel.h"
#include "DpEvent.h"
#include "DpSceneNode.h"

namespace dopixel
{
	class SceneManager : public NoCopyable, public EventBroadcaster
	{
	public:
		SceneManager();
		~SceneManager();

		void AddRootNode(const SceneNodeRef& node);
		void RemoveRootNode(const SceneNodeRef& node);

		bool OnEvent(const Event& event);
		void OnUpdate(const Timestep& timestep);
		void OnPrepare(Renderer& renderer);
		void OnRender(Renderer& renderer);

		bool IsCulled(SceneNode* node);
		void RegisterSceneNode(SceneNode* node, SceneNodeType nodeType);

		void SetActiveCamera(SceneNode* cameraNode);
		CameraSceneNode* GetActiveCamera() const;
	private:
		// root nodes
		vector<SceneNodeRef> rootNodes_;

		// nodes category, include children
		vector<SceneNode*> meshNodes_;
		vector<SceneNode*> cameraNodes_;
		vector<SceneNode*> lightNodes_;
		// active camera node
		SceneNode* activeCamerNode_;
	};
}


#endif
