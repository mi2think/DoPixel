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
		, drawGridLine_(false)
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
		camera_->SetFarClip(5000);
		camera_->SetAspectRatio((float)width_ / height_);

		GenGridLine();
	}

	bool DemoApp::Loop()
	{
		bool b = window_->Loop();
		// release cache
		MeshCache::Instance().Clear();
		MaterialCache::Instance().Clear();
		TextureCache::Instance().Clear();
		return b;
	}

	void DemoApp::Draw(const Timestep& timestep, unsigned char* buffer, int width, int height, int pitch)
	{
		renderer_->SetFrameBuffer(buffer, width, height, pitch);
		renderer_->BeginScene();

		if (drawGridLine_)
			renderer_->DrawPrimitive(gridLineVBuf_, gridLineIBuf_);

		OnRender(timestep);

		renderer_->EndScene();
	}

	void DemoApp::GenGridLine()
	{
		//    z axis
		//		|
		//  |-|-|-|-|
		//--|-|-|-|-|--  x axis
		//  |-|-|-|-|
		//      |
		
		const int eachSideLinesOfAxis = 8;
		const int xAxisLines = eachSideLinesOfAxis * 2 + 1;
		const int zAxisLines = xAxisLines;
		const int gapOfLines = 1;
		const int vertexNum = xAxisLines * 2 + zAxisLines * 2;

		const math::Vector3f lineColor(0.5f, 0.5f, 0.5f);
		const math::Vector3f xAxisLineColor(1.0f, 0.0f, 0.0f);
		const math::Vector3f zAxisLineColor(0.0f, 1.0f, 0.0f);

		gridLineVBuf_ = new VertexBuffer(PrimitiveType::Lines);
		gridLineIBuf_ = new IndexBuffer(PrimitiveType::Lines, xAxisLines + zAxisLines);

		Ref<VertexArray3f> positions(new VertexArray3f(vertexNum));
		Ref<VertexArray3f> colors(new VertexArray3f(vertexNum));
		gridLineVBuf_->SetPositions(positions);
		gridLineVBuf_->SetColors(colors);

		math::Vector3f* position = positions->DataAs<math::Vector3f>();
		math::Vector3f* color = colors->DataAs<math::Vector3f>();


		float limitPos = eachSideLinesOfAxis * gapOfLines;
		int k = 0;

		// generate parallel x lines
		float zpos = -eachSideLinesOfAxis * gapOfLines;
		for (int i = 0; i < xAxisLines; ++i, k += 2)
		{
			math::Vector3f pointColor = (i == eachSideLinesOfAxis ? xAxisLineColor : lineColor);
			// two points of line
			*(position + k) = math::Vector3f(-limitPos, 0, zpos);
			*(position + k + 1) = math::Vector3f(limitPos, 0, zpos);
			*(color + k) = pointColor;
			*(color + k + 1) = pointColor;

			zpos += gapOfLines;
		}

		// generate parallel z lines
		float xpos = -eachSideLinesOfAxis * gapOfLines;
		for (int i = 0; i < zAxisLines; ++i, k += 2)
		{
			math::Vector3f pointColor = (i == eachSideLinesOfAxis ? zAxisLineColor : lineColor);
			// two points of line
			*(position + k) = math::Vector3f(xpos, 0, -limitPos);
			*(position + k + 1) = math::Vector3f(xpos, 0, limitPos);
			*(color + k) = pointColor;
			*(color + k + 1) = pointColor;

			xpos += gapOfLines;
		}

		// index buf
		unsigned int* indices = gridLineIBuf_->GetData();
		int indexCount = gridLineIBuf_->GetIndexCount();
		for (int i = 0; i < indexCount; ++i)
		{
			*(indices + i) = i;
		}
	}
}


