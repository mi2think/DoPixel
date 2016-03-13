/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   19:56
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_app.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_app
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl app
*********************************************************************/
#ifndef __OGL_APP_H__
#define __OGL_APP_H__

namespace ogl
{
	class App
	{
	protected:
		App();

		void CalcFPS();

		void RenderFPS();

		float GetRunningTime();
	private:
		long long frameTime_;
		long long startTime_;
		int frameCount_;
		int fps_;
	};

	// Call back
	class ICallbacks
	{
	public:
		virtual void KeyboardCB(int key) {}

		virtual void PassiveMouseCB(int x, int y) {}

		virtual void RenderSceneCB() {}

		virtual void MouseCB(int button, int state, int x, int y) {}
	};
}

#endif