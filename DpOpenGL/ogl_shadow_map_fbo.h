/********************************************************************
	created:	2016/03/15
	created:	15:3:2016   7:46
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_shadow_map_fbo.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_shadow_map_fbo
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl shadow map fbo
*********************************************************************/
#ifndef __OGL_SHADOW_MAP_FBO_H__
#define __OGL_SHADOW_MAP_FBO_H__

#include <GL/glew.h>

namespace ogl
{
	class ShadowMapFBO
	{
	public:
		ShadowMapFBO();
		~ShadowMapFBO();

		bool Init(int width, int height);
		void BindForWriting();
		void BindForReading(GLenum textureUnit);
	private:
		GLuint fbo_;
		GLuint shadowMap_;
	};
}

#endif
