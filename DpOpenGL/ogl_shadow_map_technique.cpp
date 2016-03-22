/********************************************************************
	created:	2016/03/15
	created:	15:3:2016   14:21
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_shadow_map_technique.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_shadow_map_technique
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	ogl shadow map technique
*********************************************************************/
#include "ogl_shadow_map_technique.h"

namespace ogl
{
#define INVALID_UNIFORM 0xffffffff

	ShadowMapTechnique::ShadowMapTechnique()
	{

	}

	bool ShadowMapTechnique::Init(const char* vsFileName, const char* psFileName, const char* wvpName)
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

		if (WVPLocation_ == INVALID_UNIFORM)
			return false;

		return true;
	}

	void ShadowMapTechnique::SetWVP(const Matrix44f& WVP)
	{
		glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
	}
}