#include "D3DDemoApp.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "DpDirectX.lib")

HWND g_hwnd = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	D3DDemoApp* app = (D3DDemoApp*)GetWindowLong(g_hwnd, GWL_USERDATA);
	if (app && g_hwnd == hWnd)
		app->MsgProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

D3DDemoApp::D3DDemoApp()
	: clientWidth(0)
	, clientHeight(0)
	, device(nullptr)
{}

void D3DDemoApp::Create(int wndWidth, int wndHeight, const char* wndName, bool bWindow)
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
		return;
	}

	g_hwnd = CreateWindow("DEMO APP", wndName, (bWindow ? (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU) : (WS_POPUP | WS_VISIBLE)),
		CW_USEDEFAULT, CW_USEDEFAULT, wndWidth, wndHeight, NULL, NULL, wc.hInstance, NULL);


	if (!g_hwnd)
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
	GetD3DDevice()->SetClipRect(RectF(0, 0, clientWidth, clientHeight));

	device = GetD3DDevice()->GetD3DD();

	OnCreate();
}

void D3DDemoApp::CreateFullScreen(const char* wndName)
{
	int cx_screen = GetSystemMetrics(SM_CXSCREEN);
	int cy_screen = GetSystemMetrics(SM_CYSCREEN);

	Create(cx_screen, cy_screen, wndName, false);
}

bool D3DDemoApp::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		break;

	}
	return true;
}

void D3DDemoApp::Run(float fElapsedTime)
{
	fps.Run(fElapsedTime);
}

void D3DDemoApp::Render(float fElapsedTime)
{
	GetD3DDevice()->DrawText(avar("fps:%f  elapsed time:%f", fps.GetFps(), fElapsedTime), Point(0, 0), Color::black);
}

bool D3DDemoApp::Loop()
{
	timer.Tick();

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
			float timestep = timer.Tick();

			Run(timestep);

			GetD3DDevice()->BeginScene(0xffffffff);

			Render(timestep);

			GetD3DDevice()->EndScene();

			timer.Reset();
		}
	}

	Release();
	GetD3DDevice()->Release();

	return false;
}
