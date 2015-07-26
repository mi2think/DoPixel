/*
* 2015.7.26, mi2think@gmail.com
* Ϊ�˱���ʹ�ã���class����ʽ��װ����������ԭ����д�Ĵ��룬������/�� �� nehe ��ʾ��л��
* For convenient to use, using class to wrapper code written by 'Jeff Molofee', Thanks for he(her) job and 'nehe'
*********************************************************************************************************************************
*
*		��Щ��������Jeff Molofee ��2000��д�ģ��ٴ��ұ����лFredric Echols����Ϊ��ʹ��Щ���뿴���������͸�Ч����ʹ������ʹ�á�
*		����㷢����Щ�����������������������ҵ�վ�� http://nehe.gamedev.net/default.asp ��������֪����
*********************************************************************************************************************************
*		����ĵ�����DancingWind����ģ������ʲô������e-mail:zhouwei02@mails.tsinghua.edu.cn
*/

#include "OpenGLDemoApp.h"

#include <iostream>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

HWND g_hwnd = NULL;

HDC			hDC = NULL;		// ������ɫ��������
HGLRC		hRC = NULL;		// OpenGL��Ⱦ��������
HINSTANCE	hInstance;		// ��������ʵ��

bool	keys[256];			// ������̰���������
bool	active = TRUE;		// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen = TRUE;	// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// WndProc�Ķ���

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// ����OpenGL���ڴ�С
{
	if (height == 0)										// ��ֹ�����
	{
		height = 1;										// ��Height��Ϊ1
	}

	glViewport(0, 0, width, height);						// ���õ�ǰ���ӿ�

	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
	glLoadIdentity();									// ����ͶӰ����

	// �����ӿڵĴ�С
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// ѡ��ģ�͹۲����
	glLoadIdentity();									// ����ģ�͹۲����
}

int InitGL(GLvoid)										// �˴���ʼ��OpenGL������������
{
	glShadeModel(GL_SMOOTH);							// ������Ӱƽ��
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);				// ��ɫ����
	glClearDepth(1.0f);									// ������Ȼ���
	glEnable(GL_DEPTH_TEST);							// ������Ȳ���
	glDepthFunc(GL_LEQUAL);								// ������Ȳ��Ե�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// ����ϵͳ��͸�ӽ�������
	return TRUE;										// ��ʼ�� OK
}

void PrintGLInfo()
{
	std::cout << "GL Version:" << glGetString(GL_VERSION) << "\n";
	std::cout << "GLSL Version:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

int DrawGLScene(GLvoid)									// �����￪ʼ�������еĻ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �����Ļ����Ȼ���
	glLoadIdentity();									// ���õ�ǰ��ģ�͹۲����
	return TRUE;										// һ�� OK
}

GLvoid KillGLWindow(GLvoid)								// �������ٴ���
{
	if (fullscreen)										// ���Ǵ���ȫ��ģʽ��?
	{
		ChangeDisplaySettings(NULL, 0);					// �ǵĻ����л�������
		ShowCursor(TRUE);								// ��ʾ���ָ��
	}

	if (hRC)											//����ӵ��OpenGL��������?
	{
		if (!wglMakeCurrent(NULL, NULL))					// �����ܷ��ͷ�DC��RC������?
		{
			MessageBox(NULL, "�ͷ�DC��RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// �����ܷ�ɾ��RC?
		{
			MessageBox(NULL, "�ͷ�RCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// ��RC��Ϊ NULL
	}

	if (hDC && !ReleaseDC(g_hwnd, hDC))					// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL, "�ͷ�DCʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// �� DC ��Ϊ NULL
	}

	if (g_hwnd && !DestroyWindow(g_hwnd))					// �ܷ����ٴ���?
	{
		MessageBox(NULL, "�ͷŴ��ھ��ʧ�ܡ�", "�رմ���", MB_OK | MB_ICONINFORMATION);
		g_hwnd = NULL;										// �� hWnd ��Ϊ NULL
	}

	if (!UnregisterClass("OpenG", hInstance))			// �ܷ�ע����?
	{
		MessageBox(NULL, "����ע�������ࡣ", "�رմ���", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// �� hInstance ��Ϊ NULL
	}
}

/*	���������������OpenGL���ڣ�����Ϊ��									*
*	title			- ���ڱ���												*
*	width			- ���ڿ��												*
*	height			- ���ڸ߶�												*
*	bits			- ��ɫ��λ�8/16/32��									*
*	fullscreenflag	- �Ƿ�ʹ��ȫ��ģʽ��ȫ��ģʽ(TRUE)������ģʽ(FALSE)		*
*	pThis			- Userdata												*/

BOOL CreateGLWindow(const char* title, int width, int height, int bits, bool fullscreenflag, void* pThis)
{
	GLuint		PixelFormat;			// �������ƥ��Ľ��
	WNDCLASS	wc;						// ������ṹ
	DWORD		dwExStyle;				// ��չ���ڷ��
	DWORD		dwStyle;				// ���ڷ��
	RECT		WindowRect;				// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left = (long)0;			// ��Left   ��Ϊ 0
	WindowRect.right = (long)width;		// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top = (long)0;				// ��Top    ��Ϊ 0
	WindowRect.bottom = (long)height;		// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen = fullscreenflag;			// ����ȫ��ȫ����־

	hInstance = GetModuleHandle(NULL);				// ȡ�����Ǵ��ڵ�ʵ��
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc������Ϣ
	wc.cbClsExtra = 0;									// �޶��ⴰ������
	wc.cbWndExtra = 0;									// �޶��ⴰ������
	wc.hInstance = hInstance;							// ����ʵ��
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// װ��ȱʡͼ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// װ�����ָ��
	wc.hbrBackground = NULL;									// GL����Ҫ����
	wc.lpszMenuName = NULL;									// ����Ҫ�˵�
	wc.lpszClassName = "OpenGL";								// �趨������

	if (!RegisterClass(&wc))									// ����ע�ᴰ����
	{
		MessageBox(NULL, "ע�ᴰ��ʧ��", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// �˳�������FALSE
	}

	if (fullscreen)												// Ҫ����ȫ��ģʽ��?
	{
		DEVMODE dmScreenSettings;								// �豸ģʽ
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth = width;				// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight = height;				// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel = bits;					// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬��
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
			if (MessageBox(NULL, "ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��", "NeHe G", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				//����û�ѡ�񴰿�ģʽ������fullscreen ��ֵ��ΪFALSE,�����������
				fullscreen = FALSE;		// ѡ�񴰿�ģʽ(Fullscreen=FALSE)
			}
			else
			{
				//����û�ѡ���˳���������Ϣ���ڸ�֪�û����򽫽�����������FALSE���߳��򴰿�δ�ܳɹ������������˳���
				MessageBox(NULL, "���򽫱��ر�", "����", MB_OK | MB_ICONSTOP);
				return FALSE;									// �˳������� FALSE
			}
		}
	}

	if (fullscreen)												// �Դ���ȫ��ģʽ��?
	{
		dwExStyle = WS_EX_APPWINDOW;								// ��չ������
		dwStyle = WS_POPUP;										// ������
		ShowCursor(FALSE);										// �������ָ��
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ��չ������
		dwStyle = WS_OVERLAPPEDWINDOW;							// ������
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڴﵽ����Ҫ��Ĵ�С

	// ��������
	if (!(g_hwnd = CreateWindowEx(dwExStyle,							// ��չ������
		"OpenGL",							// ������
		title,								// ���ڱ���
		dwStyle |							// ����Ĵ���������
		WS_CLIPSIBLINGS |					// ����Ĵ���������
		WS_CLIPCHILDREN,					// ����Ĵ���������
		0, 0,								// ����λ��
		WindowRect.right - WindowRect.left,	// ��������õĴ��ڿ��
		WindowRect.bottom - WindowRect.top,	// ��������õĴ��ڸ߶�
		NULL,								// �޸�����
		NULL,								// �޲˵�
		hInstance,							// ʵ��
		NULL)))								// ����WM_CREATE�����κζ���
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "���ڴ�������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	SetWindowLong(g_hwnd, GWL_USERDATA, (LONG)pThis);

	static	PIXELFORMATDESCRIPTOR pfd =				//pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// ������ʽ�������Ĵ�С
		1,											// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,							// ����֧��˫����
		PFD_TYPE_RGBA,								// ���� RGBA ��ʽ
		bits,										// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,							// ���Ե�ɫ��λ
		0,											// ��Alpha����
		0,											// ����Shift Bit
		0,											// ���ۼӻ���
		0, 0, 0, 0,									// ���Ծۼ�λ
		16,											// 16λ Z-���� (��Ȼ���) 
		0,											// ���ɰ建��
		0,											// �޸�������
		PFD_MAIN_PLANE,								// ����ͼ��
		0,											// ��ʹ���ص���
		0, 0, 0										// ���Բ�����
	};

	if (!(hDC = GetDC(g_hwnd)))							// ȡ���豸��������ô?
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "���ܴ���һ�������豸������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "���ܴ���һ����ƥ������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "�����������ظ�ʽ", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// �ܷ�ȡ��OpenGL��Ⱦ������?
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "���ܴ���OpenGL��Ⱦ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// ���Լ�����ɫ������
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "���ܼ��ǰ��OpenGL��Ȼ������", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	ShowWindow(g_hwnd, SW_SHOW);						// ��ʾ����
	SetForegroundWindow(g_hwnd);						// ����������ȼ�
	SetFocus(g_hwnd);									// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);					// ����͸�� GL ��Ļ

	if (!InitGL())									// ��ʼ���½���GL����
	{
		KillGLWindow();								// ������ʾ��
		MessageBox(NULL, "��ʼ��ʧ��", "����", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// ���� FALSE
	}

	PrintGLInfo();

	return TRUE;									// �ɹ�
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// ���ڵľ��	
	UINT	uMsg,			// ���ڵ���Ϣ
	WPARAM	wParam,			// ���ӵ���Ϣ����
	LPARAM	lParam)			// ���ӵ���Ϣ����
{
	switch (uMsg)									// ���Windows��Ϣ
	{
		case WM_ACTIVATE:							// ���Ӵ��ڼ�����Ϣ
		{
			if (!HIWORD(wParam))					// �����С��״̬
			{
				active = TRUE;						// �����ڼ���״̬
			}
			else
			{
				active = FALSE;						// �����ټ���
			}

			return 0;								// ������Ϣѭ��
		}

		case WM_SYSCOMMAND:							// ϵͳ�ж�����
		{
			switch (wParam)							// ���ϵͳ����
			{
			case SC_SCREENSAVE:					// ����Ҫ����?
			case SC_MONITORPOWER:				// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;							// ��ֹ����
			}
			break;									// �˳�
		}

		case WM_CLOSE:								// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);						// �����˳���Ϣ
			return 0;								// ����
		}

		case WM_KEYDOWN:							// �м�����ô?
		{
			keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
			return 0;								// ����
		}

		case WM_KEYUP:								// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
			return 0;								// ����
		}

		case WM_SIZE:								// ����OpenGL���ڴ�С
		{
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width,HiWord=Height
			return 0;								// ����
		}
	}

	OpenGLDemoApp* app = (OpenGLDemoApp*)GetWindowLong(g_hwnd, GWL_USERDATA);
	if (app && g_hwnd == hWnd)
		app->MsgProc(hWnd, uMsg, wParam, lParam);

	// �� DefWindowProc��������δ�������Ϣ��
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

OpenGLDemoApp::OpenGLDemoApp()
	: clientWidth(0)
	, clientHeight(0)
{

}

void OpenGLDemoApp::Create(int wndWidth, int wndHeight, const char* wndName, bool bWindow)
{
	CreateGLWindow(wndName, wndWidth, wndHeight, 32, !bWindow, this);

	OnCreate();
}

void OpenGLDemoApp::CreateFullScreen(const char* wndName)
{
	int cx_screen = GetSystemMetrics(SM_CXSCREEN);
	int cy_screen = GetSystemMetrics(SM_CYSCREEN);

	Create(cx_screen, cy_screen, wndName, false);
}

bool OpenGLDemoApp::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void OpenGLDemoApp::Run(float fElapsedTime)
{
	fps.Run(fElapsedTime);
}

void OpenGLDemoApp::Render(float fElapsedTime)
{

}

bool OpenGLDemoApp::Loop()
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
			if (active)
			{
				float timestep = timer.Tick();

				Run(timestep);

				DrawGLScene();					// ���Ƴ���

				Render(timestep);

				SwapBuffers(hDC);				// �������� (˫����)

				timer.Reset();
			}

			//if (keys[VK_F1])						// F1��������ô?
			//{
			//	keys[VK_F1] = FALSE;					// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
			//	KillGLWindow();						// ���ٵ�ǰ�Ĵ���
			//	fullscreen = !fullscreen;				// �л� ȫ�� / ���� ģʽ
			//	// �ؽ� OpenGL ����
			//	if (!CreateGLWindow("NeHe's OpenGL ������", 640, 480, 16, fullscreen))
			//	{
			//		return 0;						// �������δ�ܴ����������˳�
			//	}
			//}
		}
	}

	// �رճ���
	KillGLWindow();

	return false;
}
