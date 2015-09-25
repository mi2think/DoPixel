#include "DemoApp.h"
#include "DpD3DDevice.h"

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <algorithm>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "DpDirectX.lib")

using namespace dopixel::d3d;

HWND g_hwnd = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DemoApp* app = (DemoApp*)GetWindowLong(g_hwnd, GWL_USERDATA);
	if (app && g_hwnd == hWnd)
		app->MsgProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DemoApp::Create(int wndWidth, int wndHeight, const char* wndName, bool bWindow)
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

	if (! RegisterClass(&wc))
	{
		return;
	}

	g_hwnd = CreateWindow("DEMO APP", wndName,  (bWindow ?  (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU) : (WS_POPUP | WS_VISIBLE)), 
		CW_USEDEFAULT, CW_USEDEFAULT, wndWidth, wndHeight, NULL, NULL, wc.hInstance, NULL);


	if (! g_hwnd)
	{
		return;
	}

	SetWindowLong(g_hwnd, GWL_USERDATA, (LONG)this);

	if (bWindow)
	{
		RECT rcWindow;
		GetWindowRect(g_hwnd, &rcWindow);

		RECT rcClient;
		GetClientRect(g_hwnd, &rcClient);

		int borderWidth = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
		int borderHeight = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
		MoveWindow(g_hwnd, 0, 0, borderWidth + wndWidth, borderHeight + wndHeight, false);

		GetClientRect(g_hwnd, &rcClient);

		clientWidth = rcClient.right - rcClient.left;
		clientHeight = rcClient.bottom - rcClient.top;

		ShowWindow(g_hwnd, SW_SHOW);
	}
	else
	{
		clientWidth = wndWidth;
		clientHeight = wndHeight;
	}

	GetD3DDevice()->Init(g_hwnd, clientWidth, clientHeight, bWindow);
	device.Create(clientWidth, clientHeight);
	device.SetClipRect(RectI(0, 0, clientWidth, clientHeight));

	OnCreate();
}

void DemoApp::CreateFullScreen(const char* wndName)
{
	int cx_screen = GetSystemMetrics(SM_CXSCREEN);
	int cy_screen = GetSystemMetrics(SM_CYSCREEN);

	Create(cx_screen, cy_screen, wndName, false);
}

bool DemoApp::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			KeyPressEvent event(wParam);
			OnEvent(event);
			KeyState::OnKeyPress(event);
			if (event.GetKey() == KEY_ESCAPE)
				SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			KeyReleaseEvent event(wParam);
			OnEvent(event);
			KeyState::OnKeyRelease(event);
		}
		break;
	}
	return true;
}

bool DemoApp::Loop()
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
			Run(1.0f);

			startClock = GetTickCount();

			GetD3DDevice()->BeginScene(0);

			IDirect3DSurface9* pDrawSurface = NULL;
			IDirect3DTexture9* pTexture = GetD3DDevice()->GetClientTexture();
			{
				pTexture->GetSurfaceLevel(0, &pDrawSurface);
				D3DSURFACE_DESC surfaceDesc;
				pDrawSurface->GetDesc(&surfaceDesc);
				D3DLOCKED_RECT lockRect;
				pDrawSurface->LockRect(&lockRect, NULL, D3DLOCK_DISCARD);
				// Clear
				unsigned char* p = (unsigned char*)lockRect.pBits;
				for (unsigned int i = 0; i < surfaceDesc.Height; ++i)
				{
					memset(p, 0xff, surfaceDesc.Width * 4);
					p += lockRect.Pitch;
				}

				device.Init((unsigned char*)lockRect.pBits, lockRect.Pitch / 4);
				device.BeginScene();

				Render(1.0f);

				device.EndScene();
				pDrawSurface->UnlockRect();
			}

			Vector2f pos(0,0);

			D3DDevice::stQuadTex qt;
			qt.vt[0].x = pos.x - 0.5f;		qt.vt[0].y = pos.y - 0.5f;					qt.vt[0].z = 0.0f;		qt.vt[0].w = 1.0f;	qt.vt[0].u = 0.0f;	qt.vt[0].v = 0.0f; qt.vt[0].color = 0xffffffff;
			qt.vt[1].x = pos.x + clientWidth - 0.5f;	qt.vt[1].y = pos.y - 0.5f;					qt.vt[1].z = 0.0f;		qt.vt[1].w = 1.0f;	qt.vt[1].u = 1.0f;	qt.vt[1].v = 0.0f; qt.vt[1].color = 0xffffffff;
			qt.vt[2].x = pos.x + clientWidth - 0.5f;	qt.vt[2].y = pos.y + clientHeight - 0.5f;	    qt.vt[2].z = 0.0f;		qt.vt[2].w = 1.0f;	qt.vt[2].u = 1.0f;	qt.vt[2].v = 1.0f; qt.vt[2].color = 0xffffffff;
			qt.vt[3].x = pos.x - 0.5f;				qt.vt[3].y = pos.y + clientHeight - 0.5f;	    qt.vt[3].z = 0.0f;		qt.vt[3].w = 1.0f;	qt.vt[3].u = 0.0f;	qt.vt[3].v = 1.0f; qt.vt[3].color = 0xffffffff;
			qt.tex = pTexture;

			GetD3DDevice()->DrawQuadTex(qt);

			GetD3DDevice()->EndScene();

			while (GetTickCount() - startClock < 33)
				;
		}
	}

	GetD3DDevice()->Release();

	return false;
}
