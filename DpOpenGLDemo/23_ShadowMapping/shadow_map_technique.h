#pragma once

#include "DpMatrix44.h"
#include "ogl_technique.h"
using namespace dopixel::math;
using namespace ogl;

class ShadowMapTechnique : public Technique
{
public:
	ShadowMapTechnique();

	virtual bool Init() override;

	void SetWVP(const Matrix44f& WVP);
	void SetTextureUnit(unsigned int textureUnit);
private:
	GLuint WVPLocation_;
	GLuint textureLocation_;
};
