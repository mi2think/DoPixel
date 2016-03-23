/********************************************************************
	created:	2016/03/23
	created:	23:3:2016   22:40
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_skybox_technique.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_skybox_technique
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl skybox technique
*********************************************************************/
#include "ogl_skybox_technique.h"

namespace ogl
{
#define INVALID_UNIFORM 0xffffffff

	SkyboxTechnique::SkyboxTechnique()
	{

	}

	bool SkyboxTechnique::Init(const char* vsFileName, const char* psFileName, const char* wvpName, const char* cubemapTextureName)
	{
		if (!Technique::Init())
			return false;

		if (!AddShaderFromFile(GL_VERTEX_SHADER, vsFileName))
			return false;

		if (!AddShaderFromFile(GL_FRAGMENT_SHADER, psFileName))
			return false;

		if (!Finalize())
			return false;

		WVPLocation_ = GetUniformLocation(wvpName);
		textureLocation_ = GetUniformLocation(cubemapTextureName);

		if (WVPLocation_ == INVALID_UNIFORM || textureLocation_ == INVALID_UNIFORM)
			return false;

		return true;
	}

	void SkyboxTechnique::SetWVP(const Matrix44f& WVP)
	{
		glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
	}

	void SkyboxTechnique::SetTextureUnit(unsigned int textureUnit)
	{
		glUniform1i(textureLocation_, textureUnit);
	}
}