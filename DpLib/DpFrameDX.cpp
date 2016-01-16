/********************************************************************
	created:	2016/01/07
	created:	7:1:2016   20:50
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpFrameDX.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpFrameDX
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	DX Frame
*********************************************************************/
#include "DpFrameDX.h"
#include "DpDXUtils.h"
#include "DemoApp.h"

#include <windowsx.h>	// for GET_X_LPARAM and GET_Y_LPARAM

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")


namespace dopixel
{
	void CheckWin32Error(const char* file, int line)
	{
		DWORD error = GetLastError();
		if (error == 0)
			return;

		char buf[128] = { 0 };
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, error,
			LANG_SYSTEM_DEFAULT, buf, sizeof(buf), nullptr);
		const char* str = str_format("%s(%d):\n%s\n", file, line, buf);
		MessageBox(nullptr, str, "error", MB_OK);
	}

	#define CHECK_WIN32_ERROR()  CheckWin32Error(__FILE__, __LINE__)

	//////////////////////////////////////////////////////////////////////////
	HWND g_hwnd = NULL;
	Window* g_Window = nullptr;
	static int s_clickCount = 0;
	IDirect3DVertexBuffer9* g_pVB = nullptr;
	IDirect3DTexture9* g_pTex = nullptr;

	struct Vertex 
	{
		static DWORD FVF;
		float x, y, z;
		float u, v;
		Vertex(float _x, float _y, float _z, float _u, float _v)
			: x(_x)
			, y(_y)
			, z(_z)
			, u(_u)
			, v(_v)
		{}
	};
	DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (g_Window && g_hwnd == hWnd)
		{
			g_Window->MsgProc(hWnd, uMsg, wParam, lParam);
		}
		else
		{
			if (GetCapture() != g_hwnd && s_clickCount > 0)
				s_clickCount = 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	Window::Window()
		: width_(0)
		, height_(0)
		, wndmode_(true)
		, app_(nullptr)
	{
	}

	Window::~Window()
	{
		g_Window = nullptr;
	}

	void Window::SetApp(DemoApp* app)
	{
		app_ = app;
	}

	HWND Window::GetHandle() const
	{
		return g_hwnd;
	}

	void Window::Create(int width, int height, const char* title, bool wndmode)
	{
		WNDCLASS wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WndProc;
		wc.lpszClassName = "DEMO APP";
		wc.lpszMenuName = NULL;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClass(&wc))
		{
			CHECK_WIN32_ERROR();
			return;
		}

		int w = width;
		int h = height;
		if (wndmode)
		{
			RECT rect = { 0, 0, width, height };
			AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0);
			w = rect.right - rect.left;
			h = rect.bottom - rect.top;
		}

		g_hwnd = CreateWindow("DEMO APP", title, (wndmode ? (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU) : (WS_POPUP | WS_VISIBLE)),
			(GetSystemMetrics(SM_CXSCREEN) - w) / 2, (GetSystemMetrics(SM_CYSCREEN) - h) / 2, w, h, NULL, NULL, wc.hInstance, NULL);
		if (!g_hwnd)
		{
			CHECK_WIN32_ERROR();
			return;
		}

		g_Window = this;

		width_ = width;
		height_ = height;
		wndmode_ = wndmode;

		ShowWindow(g_hwnd, SW_SHOW);
		UpdateWindow(g_hwnd);

		SetupDX();
	}

	void Window::CreateFullScreen(const char* title)
	{
		int cx_screen = GetSystemMetrics(SM_CXSCREEN);
		int cy_screen = GetSystemMetrics(SM_CYSCREEN);

		Create(cx_screen, cy_screen, title, false);
	}

	bool Window::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			KeyPressEvent event(wParam);
			app_->OnEvent(event);
			InputState::OnKeyPress(event);
			if (event.GetKey() == KEY_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			KeyReleaseEvent event(wParam);
			app_->OnEvent(event);
			InputState::OnKeyRelease(event);
		}
		break;
		case WM_MOUSEMOVE:
		{
			MouseMoveEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			app_->OnEvent(event);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			SetCapture(g_hwnd);
			++s_clickCount;
			MousePressEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_LBUTTON);
			InputState::OnMousePress(event);
			app_->OnEvent(event);
		}
		break;
		case WM_LBUTTONUP:
		{
			if (--s_clickCount < 1)
			{
				s_clickCount = 0;
				ReleaseCapture();
			}
			MouseReleaseEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_LBUTTON);
			InputState::OnMouseRelease(event);
			app_->OnEvent(event);
		}
		break;
		case WM_RBUTTONDOWN:
		{
			SetCapture(g_hwnd);
			++s_clickCount;
			MousePressEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_RBUTTON);
			InputState::OnMousePress(event);
			app_->OnEvent(event);
		}
		break;
		case WM_RBUTTONUP:
		{
			if (--s_clickCount < 1)
			{
				s_clickCount = 0;
				ReleaseCapture();
			}
			MouseReleaseEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_RBUTTON);
			InputState::OnMouseRelease(event);
			app_->OnEvent(event);
		}
		break;
		case WM_MBUTTONDOWN:
		{
			SetCapture(g_hwnd);
			++s_clickCount;
			MousePressEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_MBUTTON);
			InputState::OnMousePress(event);
			app_->OnEvent(event);
		}
		break;
		case WM_MBUTTONUP:
		{
			if (--s_clickCount < 1)
			{
				s_clickCount = 0;
				ReleaseCapture();
			}
			MouseReleaseEvent event(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_MBUTTON);
			InputState::OnMouseRelease(event);
			app_->OnEvent(event);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			MouseWheelEvent event(GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA);
			app_->OnEvent(event);
		}
		break;
		}
		return true;
	}

	void Window::SetupDX()
	{
		SetupD3D();
		SetupD3DDevice(g_hwnd, width_, height_, wndmode_);

		if (!g_pD3DD)
			return;

		D3DVIEWPORT9 vp;
		vp.X = 0;
		vp.Y = 0;
		vp.Width = width_;
		vp.Height = height_;
		vp.MinZ = 0;
		vp.MaxZ = 1;
		CHECK_HR = g_pD3DD->SetViewport(&vp);

		D3DXMATRIX mat;
		D3DXMatrixOrthoLH(&mat, 2, 2, 1, 2);
		CHECK_HR = g_pD3DD->SetTransform(D3DTS_PROJECTION, &mat);

		CHECK_HR = g_pD3DD->CreateVertexBuffer(sizeof(Vertex) * 4, D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &g_pVB, nullptr);
		if (g_pVB)
		{
			Vertex* v = nullptr;
			CHECK_HR = g_pVB->Lock(0, 0, (void**)&v, 0);
			float z = 1.5f;
			float offsetX = -1.0f / width_;
			float offsetY = -1.0f / height_;
			v[0] = Vertex(-1 + offsetX, 1 + offsetY, z, 0, 0);
			v[1] = Vertex( 1 + offsetX, 1 + offsetY, z, 1, 0);
			v[2] = Vertex(-1 + offsetX,-1 + offsetY, z, 0, 1);
			v[3] = Vertex( 1 + offsetX,-1 + offsetY, z, 1, 1);
			CHECK_HR = g_pVB->Unlock();
		}

		CHECK_HR = g_pD3DD->CreateTexture(width_, height_, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTex, nullptr);
		CHECK_HR = g_pD3DD->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	}

	bool Window::Loop()
	{
		MSG msg;		
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				float time = timer_.Tick();
				int milliseconds = math::Clamp<int>(int(time * 1000.0f), 0, 100);
				Timestep timestep(milliseconds);

				app_->OnUpdate(timestep);
				UpdateDX(timestep);
				DisplayDX();

				timer_.Reset();
			}
		}

		DestoryDX();

		return false;
	}

	void Window::UpdateDX(const Timestep& timestep)
	{
		if (g_pD3DD && g_pTex)
		{
			D3DLOCKED_RECT lockRect = { 0 };
			CHECK_HR = g_pTex->LockRect(0, &lockRect, nullptr, D3DLOCK_DISCARD);
			app_->Draw(timestep, (unsigned char*)lockRect.pBits, width_, height_, lockRect.Pitch);
			CHECK_HR = g_pTex->UnlockRect(0);
		}
	}

	void Window::DisplayDX()
	{
		if (g_pD3DD)
		{
			CHECK_HR = g_pD3DD->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
			CHECK_HR = g_pD3DD->BeginScene();

			CHECK_HR = g_pD3DD->SetTexture(0, g_pTex);
			CHECK_HR = g_pD3DD->SetStreamSource(0, g_pVB, 0, sizeof(Vertex));
			CHECK_HR = g_pD3DD->SetFVF(Vertex::FVF);
			CHECK_HR = g_pD3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			CHECK_HR = g_pD3DD->EndScene();
			CHECK_HR = g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	void Window::DestoryDX()
	{
		SAFE_RELEASE(g_pVB);
		SAFE_RELEASE(g_pTex);
		DestoryDirectX();
	}
}