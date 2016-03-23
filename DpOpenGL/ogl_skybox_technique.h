/********************************************************************
	created:	2016/03/23
	created:	23:3:2016   22:34
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_skybox_technique.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_skybox_technique
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl skybox technique
*********************************************************************/
#ifndef __OGL_SKYBOX_TECHNIQUE_H__
#define __OGL_SKYBOX_TECHNIQUE_H__

#include "DpMatrix44.h"
#include "ogl_technique.h"
using namespace dopixel::math;
using namespace ogl;

namespace ogl
{
	class SkyboxTechnique : public Technique
	{
	public:
		SkyboxTechnique();

		bool Init(const char* vsFileName, const char* psFileName, const char* wvpName, const char* cubemapTextureName);

		void SetWVP(const Matrix44f& WVP);

		void SetTextureUnit(unsigned int textureUnit);
	private:
		GLuint WVPLocation_;
		GLuint textureLocation_;
	};
}

#endif