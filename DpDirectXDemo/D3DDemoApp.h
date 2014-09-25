#ifndef __D3DDEMO_APP__
#define __D3DDEMO_APP__

#include "DpGeometry.h"
#include "DpTimer.h"
#include "DpFps.h"
#include "DpColor.h"
#include "DpD3DDevice.h"

#include <windows.h>

using namespace DoPixel::D3D;
using namespace DoPixel::Core;
using namespace DoPixel::Math;

class D3DDemoApp
{
public:
	D3DDemoApp() {}
	virtual ~D3DDemoApp() {}

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