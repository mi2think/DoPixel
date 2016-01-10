#ifndef __DEMO_APP__
#define __DEMO_APP__

#include "DpKeyEvent.h"
#include "DpMouseEvent.h"
#include "DpInputState.h"
#include "DpDevice.h"
#include "DpColor.h"
#include "DpCamera.h"
#include "DpObject.h"
#include "DpLight.h"
#include "DpLoadModel.h"

#include <windows.h>
#include <map>
#include <vector>

using namespace dopixel::core;
using namespace dopixel::math;

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

	virtual bool OnEvent(const Event& event) { return false; }
protected:
	int clientWidth;
	int clientHeight;
	int startClock;
};


#endif