/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:07
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_glut.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_glut
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl glut
*********************************************************************/
#include "ogl_glut.h"
#include "ogl_app.h"

#include "DoPixel.h" // for str_format

#include <GL/glew.h>
#include <GL/freeglut.h>

namespace ogl
{
	static ICallbacks* s_pCallbacks = nullptr;
	static bool s_withDepth = false;
	static bool s_withStencil = false;

	static void RenderSceneCB()
	{
		s_pCallbacks->RenderSceneCB();
	}

	static void IdleCB()
	{
		s_pCallbacks->RenderSceneCB();
	}

	static void SpecialKeyboardCB(int key, int x, int y)
	{
		s_pCallbacks->KeyboardCB(key);
	}

	static void KeyboardCB(unsigned char key, int x, int y)
	{
		s_pCallbacks->KeyboardCB(key);
	}

	static void PassiveMouseCB(int x, int y)
	{
		s_pCallbacks->PassiveMouseCB(x, y);
	}

	static void MouseCB(int button, int state, int x, int y)
	{
		s_pCallbacks->MouseCB(button, state, x, y);
	}

	static void InitCallbacks()
	{
		glutDisplayFunc(RenderSceneCB);
		glutIdleFunc(IdleCB);
		glutSpecialFunc(SpecialKeyboardCB);
		glutKeyboardFunc(KeyboardCB);
		glutPassiveMotionFunc(PassiveMouseCB);
		glutMouseFunc(MouseCB);
	}

	void GLUTBackendInit(int argc, char** argv, bool withDepth, bool withStencil)
	{
		s_withDepth = withDepth;
		s_withStencil = withStencil;

		glutInit(&argc, argv);

		unsigned int displayMode = GLUT_DOUBLE | GLUT_RGBA;
		if (withDepth)
			displayMode |= GLUT_DEPTH;
		if (withStencil)
			displayMode |= GLUT_STENCIL;

		glutInitDisplayMode(displayMode);

		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	}

	bool GLUTBackendCreateWindow(unsigned int width, unsigned int height, bool fullScreen, const char* title)
	{
		if (fullScreen)
		{
			int bpp = 32;
			const char* p = dopixel::str_format("%dx%d:%d@60", width, height, bpp);
			glutGameModeString(p);
			glutEnterGameMode();
		}
		else
		{
			glutInitWindowSize(width, height);
			glutInitWindowPosition(0, 0);
			glutCreateWindow(title);
		}

		// Must be done after glut is initialized
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			fprintf(stderr, "error: '%s'\n", glewGetErrorString(res));
			return false;
		}

		return true;
	}

	void GLUTBackendRun(ICallbacks* callbacks)
	{
		if (!callbacks)
		{
			fprintf(stderr, "%s: callbacks not specified\n", __FUNCTION__);
			return;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		if (s_withDepth)
			glEnable(GL_DEPTH_TEST);

		s_pCallbacks = callbacks;
		InitCallbacks();
		glutMainLoop();
	}

	void GLUTBackendSwapBuffers()
	{
		glutSwapBuffers();
	}

	void GLUTBackendLeaveMainLoop()
	{
		glutLeaveMainLoop();
	}

}