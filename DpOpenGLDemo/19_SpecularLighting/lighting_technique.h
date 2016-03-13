#pragma once

#include "ogl_technique.h"
using namespace ogl;

#define UNVALID_UNIFORM 0xffffffff

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

	virtual bool Init() override;

	void SetWVP(const Matrix44f& WVP);
	void SetWorldMatrix(const Matrix44f& worldMatrix);
	void SetTextureUnit(unsigned int textureUnit);
	void SetDirectionalLight(const DirectionalLight& light);

	void SetEyeWorldPos(const Vector3f& eyeWorldPos);
	void SetSpecularIntensity(float intensity);
	void SetSpecularPower(float power);
private:
	GLuint WVPLocation_;
	GLuint worldMatrixLocation_;
	GLuint samplerLocation_;
	
	GLuint eyeWorldPosLocation_;
	GLuint specularIntensityLocation_;
	GLuint specularPowerLocation_;

	struct
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint direction;
		GLuint diffuseIntensity;

		bool ChechValid() const
		{
			return color != UNVALID_UNIFORM && ambientIntensity != UNVALID_UNIFORM &&
				direction != UNVALID_UNIFORM && diffuseIntensity != UNVALID_UNIFORM;
		}
	} directionalLight_;
};

LightingTechnique::LightingTechnique()
	: WVPLocation_(0)
	, worldMatrixLocation_(0)
	, samplerLocation_(0)
	, eyeWorldPosLocation_(0)
	, specularIntensityLocation_(0)
	, specularPowerLocation_(0)
{

}

bool LightingTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/19_SpecularLighting/lighting.vs"))
		return false;

	if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/19_SpecularLighting/lighting.ps"))
		return false;

	if (!Finalize())
		return false;

	WVPLocation_ = GetUniformLocation("gWVP");
	worldMatrixLocation_ = GetUniformLocation("gWorld");
	samplerLocation_ = GetUniformLocation("gSampler");
	eyeWorldPosLocation_ = GetUniformLocation("gEyeWorldPos");
	specularIntensityLocation_ = GetUniformLocation("gSpecularIntensity");
	specularPowerLocation_ = GetUniformLocation("gSpecularPower");

	directionalLight_.color = GetUniformLocation("gDirectionalLight.color");
	directionalLight_.ambientIntensity = GetUniformLocation("gDirectionalLight.ambientIntensity");
	directionalLight_.direction = GetUniformLocation("gDirectionalLight.direction");
	directionalLight_.diffuseIntensity = GetUniformLocation("gDirectionalLight.diffuseIntensity");

	if (WVPLocation_ == UNVALID_UNIFORM ||
		worldMatrixLocation_ == UNVALID_UNIFORM ||
		samplerLocation_ == UNVALID_UNIFORM ||
		eyeWorldPosLocation_ == UNVALID_UNIFORM ||
		specularIntensityLocation_ == UNVALID_UNIFORM ||
		specularPowerLocation_ == UNVALID_UNIFORM ||
		!directionalLight_.ChechValid())
		return false;

	return true;
}

void LightingTechnique::SetWVP(const Matrix44f& WVP)
{
	glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, &(WVP.m[0][0]));
}

void LightingTechnique::SetWorldMatrix(const Matrix44f& worldMatrix)
{
	glUniformMatrix4fv(worldMatrixLocation_, 1, GL_TRUE, &(worldMatrix.m[0][0]));
}

void LightingTechnique::SetTextureUnit(unsigned int textureUnit)
{
	glUniform1i(samplerLocation_, textureUnit);
}

void LightingTechnique::SetDirectionalLight(const DirectionalLight& light)
{
	glUniform3f(directionalLight_.color, light.color.x, light.color.y, light.color.z);
	glUniform1f(directionalLight_.ambientIntensity, light.ambientIntensity);
	Vector3f direction = light.direction;
	direction.Normalize();
	glUniform3f(directionalLight_.direction, direction.x, direction.y, direction.z);
	glUniform1f(directionalLight_.diffuseIntensity, light.diffuseIntensity);
}

void LightingTechnique::SetEyeWorldPos(const Vector3f& eyeWorldPos)
{
	glUniform3f(eyeWorldPosLocation_, eyeWorldPos.x, eyeWorldPos.y, eyeWorldPos.z);
}

void LightingTechnique::SetSpecularIntensity(float intensity)
{
	glUniform1f(specularIntensityLocation_, intensity);
}

void LightingTechnique::SetSpecularPower(float power)
{
	glUniform1f(specularPowerLocation_, power);
}
