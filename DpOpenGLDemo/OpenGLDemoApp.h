#ifndef __OpenGL_DEMO_APP__
#define __OpenGL_DEMO_APP__

#include "DpGeometry.h"
#include "DpTimer.h"
#include "DpFps.h"
#include "DpColor.h"

#include "glew.h"
#include "glut.h"

using namespace DoPixel::Core;
using namespace DoPixel::Math;

class OpenGLDemoApp
{
public:
	OpenGLDemoApp();
	virtual ~OpenGLDemoApp() {}

	void Create(int wndWidth, int wndHeight, const char* wndName, bool bWindow = true);

	void CreateFullScreen(const char* wndName);

	bool Loop();

	virtual void OnCreate() {}

	virtual bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void Run(float fElapsedTime);

	virtual void Render(float fElapsedTime);
protected:
	int clientWidth;
	int clientHeight;

	Timer timer;
	Fps fps;
};


#endif