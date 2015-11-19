/********************************************************************
	created:	2015/11/17
	created:	17:11:2015   19:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpSceneManager.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpSceneManager
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Scene Manager
*********************************************************************/
#include "DpSceneManager.h"

namespace dopixel
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
	}

	bool SceneManager::OnEvent(const Event& event)
	{
		for (auto& node : rootNodes_)
		{
			node->OnEvent(event);
		}
		return true;
	}

	void SceneManager::OnUpdate(const Timestep& timestep)
	{
		// animate
		for (auto& node : rootNodes_)
		{
			node->OnAnimate(timestep);
		}

		// update nodes category
		meshNodes_.clear();
		cameraNodes_.clear();
		for (auto& node : rootNodes_)
		{
			node->OnRegisterSceneNode(this);
		}
	}

	void SceneManager::OnRender(Renderer& renderer)
	{
		// camera nodes
		for (auto& cameraNode : cameraNodes_)
		{
			cameraNode->OnRender(renderer);
		}

		// mesh nodes
		// TODO:
		// sort mesh by z order from camera
		for (auto& meshNode : meshNodes_)
		{
			meshNode->OnRender(renderer);
		}
	}

	void SceneManager::AddRootNode(const SceneNodeRef& node)
	{
		auto it = std::find(rootNodes_.begin(), rootNodes_.end(), node);
		if (it == rootNodes_.end())
		{
			rootNodes_.push_back(node);
		}
	}

	void SceneManager::RemoveRootNode(const SceneNodeRef& node)
	{
		auto it = std::find(rootNodes_.begin(), rootNodes_.end(), node);
		if (it != rootNodes_.end())
		{
			rootNodes_.erase(it);
		}
	}

	bool SceneManager::IsCulled(SceneNode* node)
	{
		return false;
	}

	void SceneManager::RegisterSceneNode(SceneNode* node, SceneNodeType nodeType)
	{
		switch (nodeType)
		{
		case SceneNodeType::MeshNode:
			if (!IsCulled(node))
			{
				meshNodes_.push_back(node);
			}
			break;
		case SceneNodeType::CameraNode:
			if (!IsCulled(node))
			{
				meshNodes_.push_back(node);
			}
			break;
		}
	}
}