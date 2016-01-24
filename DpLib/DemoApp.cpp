/********************************************************************
	created:	2015/11/30
	created:	30:11:2015   12:05
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DemoApp.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DemoApp
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Demo App
*********************************************************************/
#include "DemoApp.h"
#include "DpDXUtils.h"
#include "DpFrameDX.h"

namespace dopixel
{
	DemoApp::DemoApp()
		: width_(0)
		, height_(0)
		, window_(new Window())
		, renderer_(new Renderer())
		, camera_(new Camera())
	{
	}

	DemoApp::~DemoApp()
	{
	}

	void DemoApp::Create(int width, int height, const char* title, bool wndmode)
	{
		window_->Create(width, height, title, wndmode);
		OnCreate();
	}

	void DemoApp::CreateFullScreen(const char* title)
	{
		window_->CreateFullScreen(title);
		OnCreate();
	}

	void DemoApp::OnCreate()
	{
		window_->SetApp(this);

		width_ = window_->GetWidth();
		height_ = window_->GetHeight();
		// default camera
		camera_->SetFovy(90);
		camera_->SetNearClip(1);
		camera_->SetFarClip(1000);
		camera_->SetAspectRatio((float)width_ / height_);

		cameraNode_ = new CameraSceneNode("camera", camera_);
	}

	bool DemoApp::Loop()
	{
		return window_->Loop();
	}

	void DemoApp::Draw(const Timestep& timestep, unsigned char* buffer, int width, int height, int pitch)
	{
		renderer_->SetFrameBuffer(buffer, width, height, pitch);
		renderer_->BeginScene();

		OnRender(timestep);

		renderer_->EndScene();
	}
}


