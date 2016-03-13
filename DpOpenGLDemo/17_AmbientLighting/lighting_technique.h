#pragma once

#include "ogl_technique.h"
using namespace ogl;

const char* vs = "#version 330\n"
"layout(location = 0) in vec3 Position;\n"
"layout(location = 1) in vec2 TexCoord;\n"
"uniform mat4 gWVP;"
"out vec2 TexCoord0;"
"void main()"
"{"
"gl_Position = gWVP * vec4(Position, 1.0);"
"TexCoord0 = TexCoord;"
"}";

const char* ps = "#version 330\n"
"in vec2 TexCoord0;"
"struct DirectionalLight { vec3 color; float ambientIntensity; };"
"uniform DirectionalLight gDirectionalLight;"
"uniform sampler2D gSampler;"
"void main()"
"{"
"gl_FragColor = texture2D(gSampler, TexCoord0.xy) * vec4(gDirectionalLight.color, 1.0f) * gDirectionalLight.ambientIntensity;"
"}";

struct DirectionalLight
{
	Vector3f color;
	float ambientIntensity;
};

class LightingTechnique : public Technique
{
public:
	LightingTechnique();

	virtual bool Init();

	void SetWVP(const Matrix44f& WVP);
	void SetTextureUnit(unsigned int textureUnit);
	void SetDirectionalLight(const DirectionalLight& light);
private:
	GLuint WVPLocation_;
	GLuint samplerLocation_;
	GLuint dirLightColorLocation_;
	GLuint dirLightAmbientIntensityLocation_;
};

LightingTechnique::LightingTechnique()
	: WVPLocation_(0)
	, samplerLocation_(0)
	, dirLightColorLocation_(0)
	, dirLightAmbientIntensityLocation_(0)
{

}

bool LightingTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShader(GL_VERTEX_SHADER, vs))
		return false;

	if (!AddShader(GL_FRAGMENT_SHADER, ps))
		return false;

	if (!Finalize())
		return false;

	WVPLocation_ = GetUniformLocation("gWVP");
	samplerLocation_ = GetUniformLocation("gSampler");
	dirLightColorLocation_ = GetUniformLocation("gDirectionalLight.color");
	dirLightAmbientIntensityLocation_ = GetUniformLocation("gDirectionalLight.ambientIntensity");

	if (WVPLocation_ == 0xffffffff ||
		samplerLocation_ == 0xffffffff ||
		dirLightColorLocation_ == 0xffffffff ||
		dirLightAmbientIntensityLocation_ == 0xffffffff)
		return false;

	return true;
}

void LightingTechnique::SetWVP(const Matrix44f& WVP)
{
	glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
}

void LightingTechnique::SetTextureUnit(unsigned int textureUnit)
{
	glUniform1i(samplerLocation_, textureUnit);
}

void LightingTechnique::SetDirectionalLight(const DirectionalLight& light)
{
	glUniform3f(dirLightColorLocation_, light.color.x, light.color.y, light.color.z);
	glUniform1f(dirLightAmbientIntensityLocation_, light.ambientIntensity);
}