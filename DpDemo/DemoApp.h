#ifndef __DEMO_APP__
#define __DEMO_APP__

#include "DpNoCopy.h"
#include "DpDevice.h"
#include "DpColor.h"
#include "DpCamera.h"
#include "DpObject.h"
#include "DpLight.h"
#include "DpLoadModel.h"

#include <windows.h>
#include <map>
#include <vector>

using namespace DoPixel::Core;
using namespace DoPixel::Math;

class DemoApp
{
public:
	DemoApp() {}
	virtual ~DemoApp() {}

	void Create(int wndWidth, int wndHeight, const char* wndName, bool bWindow = true);

	void CreateFullScreen(const char* wndName);

	bool Loop();

	virtual void OnCreate() {}

	virtual bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void Run(float fElapsedTime) {}

	virtual void Render(float fElapsedTime) {}

protected:
	Device device;

	LightManager lightFactory;

	int clientWidth;
	int clientHeight;
	int startClock;
};


#endif