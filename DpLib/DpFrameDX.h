/********************************************************************
	created:	2016/01/07
	created:	7:1:2016   20:49
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpFrameDX.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpFrameDX
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	DX Frame
*********************************************************************/
#ifndef __DP_FRAME_DX_H__
#define __DP_FRAME_DX_H__

#include "DoPixel.h"
#include "DpTimer.h"
#include <windows.h>

namespace dopixel
{
	class DemoApp;

	class Window
	{
	public:
		Window();
		~Window();

		void Create(int width, int height, const char* title, bool wndmode);
		void CreateFullScreen(const char* title);
		bool MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool Loop();

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		HWND GetHandle() const;

		void SetApp(DemoApp* app);
		void SetupDX();
		void UpdateDX(const Timestep& timestep);
		void DisplayDX();
		void ReleaseDX();
	private:
		int width_;
		int height_;
		bool wndmode_;
		Timer timer_;
		DemoApp* app_;
	};
}

#endif