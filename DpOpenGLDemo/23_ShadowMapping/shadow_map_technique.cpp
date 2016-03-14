#include "shadow_map_technique.h"

#define INVALID_UNIFORM 0xffffffff

ShadowMapTechnique::ShadowMapTechnique()
{

}

bool ShadowMapTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/23_ShadowMapping/shadow_map.vs"))
		return false;

	if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/23_ShadowMapping/shadow_map.ps"))
		return false;

	if (!Finalize())
		return false;

	WVPLocation_ = GetUniformLocation("gWVP");
	textureLocation_ = GetUniformLocation("gShadowMap");

	if (WVPLocation_ == INVALID_UNIFORM || textureLocation_ == INVALID_UNIFORM)
		return false;

	return true;
}

void ShadowMapTechnique::SetWVP(const Matrix44f& WVP)
{
	glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
}

void ShadowMapTechnique::SetTextureUnit(unsigned int textureUnit)
{
	glUniform1i(textureLocation_, textureUnit);
}
