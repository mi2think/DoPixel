/********************************************************************
	created:	2015/11/19
	created:	19:11:2015   0:34
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpSceneNodeAnimator.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpSceneNodeAnimator
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Scene Node Animator
*********************************************************************/
#ifndef __DP_SCENE_NODE_ANIMATOR_H__
#define __DP_SCENE_NODE_ANIMATOR_H__

#include "DoPixel.h"

namespace dopixel
{
	class SceneNodeAnimator
	{
	public:
		virtual ~SceneNodeAnimator() {}
		// return true if change position or rotation
		// or scale of node, else false
		virtual bool AnimateNode(SceneNode* node, const Timestep& timestep) = 0;
		virtual bool OnEvent(const Event& event) { return false; }
	};
}

#endif