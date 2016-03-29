/********************************************************************
	created:	2016/03/23
	created:	23:3:2016   11:28
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_cubemap_texture.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_cubemap_texture
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	ogl cubemap texture
*********************************************************************/
#ifndef __OGL_CUBEMAP_TEXTURE_H__
#define __OGL_CUBEMAP_TEXTURE_H__

#include <GL/glew.h>
#include <string>

namespace ogl
{
	class CubemapTexture
	{
	public:
		CubemapTexture( const std::string& posXFileName,
						const std::string& negXFileName,
						const std::string& posYFileName,
						const std::string& negYFileName,
						const std::string& posZFileName,
						const std::string& negZFileName);
		~CubemapTexture();

		bool Load();

		void Bind(GLenum textureUnit);

		enum { CUBEMAP_TEXTURE_NUM = 6 };
	private:
		std::string fileNames_[CUBEMAP_TEXTURE_NUM];
		GLuint textureObj_;
	};
}

#endif