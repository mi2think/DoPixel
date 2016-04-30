/********************************************************************
	created:	2015/11/30
	created:	30:11:2015   12:04
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DemoApp.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DemoApp
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Demo App
*********************************************************************/
#ifndef __DEMO_APP__
#define __DEMO_APP__

#include "DoPixel.h"
#include "DpKeyEvent.h"
#include "DpMouseEvent.h"
#include "DpInputState.h"
#include "DpColor.h"
#include "DpCamera.h"
#include "DpArcBall.h"
#include "DpCameraController.h"
#include "DpVertexArray.h"
#include "DpVertexBuffer.h"
#include "DpIndexBuffer.h"
#include "DpTexture.h"
#include "DpMaterial.h"
#include "DpMesh.h"
#include "DpLoader.h"
#include "DpRenderer.h"

namespace dopixel
{
	class Window;

	class DemoApp
	{
	public:
		DemoApp();
		virtual ~DemoApp();

		void Create(int width, int height, const char* title, bool wndmode = true);

		void CreateFullScreen(const char* title);

		bool Loop();

		void Draw(const Timestep& timestep, unsigned char* buffer, int width, int height, int pitch);

		virtual void OnCreate();

		virtual void OnUpdate(const Timestep& timestep) {}

		virtual void OnRender(const Timestep& timestep) {}

		virtual bool OnEvent(const Event& event) { return false; }
	protected:
		int width_;
		int height_;
		Window* window_;
		RendererRef renderer_;
		CameraRef camera_;
		CameraControllerRef cameraController_;
	};
}

#endif