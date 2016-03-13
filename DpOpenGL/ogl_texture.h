/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:22
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_texture.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_texture
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl texture
*********************************************************************/
#ifndef __OGL_TEXTURE_H__
#define __OGL_TEXTURE_H__

#include <GL/glew.h>
#include <string>

namespace ogl
{
	class Texture
	{
	public:
		Texture(GLenum textureTarget, const std::string& fileName);
		~Texture();

		bool Load();

		void Bind(GLenum textureUnit);
	private:
		std::string fileName_;
		int width_;
		int height_;
		unsigned char* data_;

		GLenum textureTarget_;
		GLuint textureObj_;
	};
}

#endif