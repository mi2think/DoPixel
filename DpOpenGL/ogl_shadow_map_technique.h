/********************************************************************
	created:	2016/03/15
	created:	15:3:2016   14:15
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_shadow_map_technique.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_shadow_map_technique
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	ogl shadow map technique
*********************************************************************/
#ifndef __OGL_SHADOW_MAP_TECHNIQUE_H__
#define __OGL_SHADOW_MAP_TECHNIQUE_H__

#include "DpMatrix44.h"
#include "ogl_technique.h"
using namespace dopixel::math;
using namespace ogl;

namespace ogl
{
	class ShadowMapTechnique : public Technique
	{
	public:
		ShadowMapTechnique();

		bool Init(const char* vsFileName, const char* psFileName, const char* wvpName, const char* shadowMapName);

		void SetWVP(const Matrix44f& WVP);
		void SetTextureUnit(unsigned int textureUnit);
	private:
		GLuint WVPLocation_;
		GLuint textureLocation_;
	};
}

#endif


