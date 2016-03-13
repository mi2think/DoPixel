/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:06
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_glut.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_glut
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl glut
*********************************************************************/
#ifndef __OGL_GLUT_H__
#define __OGL_GLUT_H__

namespace ogl
{
	class ICallbacks;

	void GLUTBackendInit(int argc, char** argv, bool withDepth, bool withStencil);
	bool GLUTBackendCreateWindow(unsigned int width, unsigned int height, bool fullScreen, const char* title);
	void GLUTBackendRun(ICallbacks* callbacks);
	void GLUTBackendSwapBuffers();
	void GLUTBackendLeaveMainLoop();
}

#endif