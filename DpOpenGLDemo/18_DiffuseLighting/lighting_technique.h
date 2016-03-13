#pragma once

#include "ogl_technique.h"
using namespace ogl;

const char* vs = "#version 330\n"
"layout(location = 0) in vec3 Position;\n"
"layout(location = 1) in vec2 TexCoord;\n"
"layout(location = 2) in vec3 Normal;\n"
"uniform mat4 gWVP;"
"uniform mat4 gWorld;"
"out vec2 TexCoord0;"
"out vec3 Normal0;"
"void main()"
"{"
"gl_Position = gWVP * vec4(Position, 1.0);"
"TexCoord0 = TexCoord;"
"Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;"
"}";

const char* ps = "#version 330\n"
"in vec2 TexCoord0;"
"in vec3 Normal0;"
"struct DirectionalLight { vec3 color; float ambientIntensity; vec3 direction; float diffuseIntensity; };"
"uniform DirectionalLight gDirectionalLight;"
"uniform sampler2D gSampler;"
"void main()"
"{"
"vec4 ambientColor = vec4(gDirectionalLight.color, 1.0f) * gDirectionalLight.ambientIntensity;"
"float k = dot(normalize(Normal0), -gDirectionalLight.direction);"
"vec4 diffuseColor;"
"if (k > 0) { diffuseColor = vec4(gDirectionalLight.color, 1.0f) * gDirectionalLight.diffuseIntensity * k; }"
"else { diffuseColor = vec4(0,0,0,0); }"
"gl_FragColor = texture2D(gSampler, TexCoord0.xy) * (ambientColor + diffuseColor);"
"}";

struct DirectionalLight
{
	Vector3f color;
	float ambientIntensity;
	Vector3f direction;
	float diffuseIntensity;
};

class LightingTechnique : public Technique
{
public:
	LightingTechnique();

	virtual bool Init();

	void SetWVP(const Matrix44f& WVP);
	void SetWorldMatrix(const Matrix44f& worldMatrix);
	void SetTextureUnit(unsigned int textureUnit);
	void SetDirectionalLight(const DirectionalLight& light);
private:
	GLuint WVPLocation_;
	GLuint worldMatrixLocation_;
	GLuint samplerLocation_;

	struct
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint direction;
		GLuint diffuseIntensity;
		bool CheckValid() const
		{
			const unsigned int unvalidVal = 0xffffffff;
			return color != unvalidVal && ambientIntensity != unvalidVal && direction != unvalidVal && diffuseIntensity != unvalidVal;
		}
	} dirLightLocation_;
};

LightingTechnique::LightingTechnique()
	: WVPLocation_(0)
	, worldMatrixLocation_(0)
	, samplerLocation_(0)
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
	worldMatrixLocation_ = GetUniformLocation("gWorld");
	samplerLocation_ = GetUniformLocation("gSampler");

	dirLightLocation_.color = GetUniformLocation("gDirectionalLight.color");
	dirLightLocation_.ambientIntensity = GetUniformLocation("gDirectionalLight.ambientIntensity");
	dirLightLocation_.direction = GetUniformLocation("gDirectionalLight.direction");
	dirLightLocation_.diffuseIntensity = GetUniformLocation("gDirectionalLight.diffuseIntensity");

	if (WVPLocation_ == 0xffffffff ||
		worldMatrixLocation_ == 0xffffffff ||
		samplerLocation_ == 0xffffffff ||
		!dirLightLocation_.CheckValid())
		return false;

	return true;
}

void LightingTechnique::SetWorldMatrix(const Matrix44f& worldMatrix)
{
	glUniformMatrix4fv(worldMatrixLocation_, 1, GL_TRUE, &(worldMatrix.m[0][0]));
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
	glUniform3f(dirLightLocation_.color, light.color.x, light.color.y, light.color.z);
	glUniform1f(dirLightLocation_.ambientIntensity, light.ambientIntensity);
	glUniform3f(dirLightLocation_.direction, light.direction.x, light.direction.y, light.direction.z);
	glUniform1f(dirLightLocation_.diffuseIntensity, light.diffuseIntensity);
}