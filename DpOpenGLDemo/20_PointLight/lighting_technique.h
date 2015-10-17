#pragma once

#include "Pipeline.h"

struct BaseLight
{
	Vector3f color_;
	float ambientIntensity_;
	float diffuseIntensity_;

	BaseLight()
		: color_(0, 0, 0)
		, ambientIntensity_(0)
		, diffuseIntensity_(0)
	{}
};

struct DirectionalLight : public BaseLight
{
	Vector3f direction_;

	DirectionalLight()
		: direction_(0, 0, 0)
	{}
};

struct PointLight : public BaseLight
{
	struct Attenuation
	{
		float kc;
		float kl;
		float kq;
		Attenuation()
			: kc(1)
			, kl(0)
			, kq(0)
		{}
	};

	Vector3f position_;
	Attenuation attenuation_;

	PointLight()
		: position_(0, 0, 0)
	{}
};

class LightingTechnique : public Technique
{
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;

	LightingTechnique() {}

	virtual bool Init() override;

	void SetWVP(const Matrix44f& WVP);
	void SetWorldMatrix(const Matrix44f& worldMatrix);
	void SetTextureUnit(unsigned int textureUnit);
	void SetEyeWorldPos(const Vector3f& eyeWorldPos);
	void SetSpecularIntensity(float specularIntensity);
	void SetSpecularPower(float specularPower);
	void SetDirectionalLight(const DirectionalLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);
private:
	GLuint WVPLocation_;
	GLuint worldMatrixLocation_;
	GLuint samplerLocation_;
	GLuint eyeWorldPosLocation_;
	GLuint specularIntensityLocation_;
	GLuint specularPowerLocation_;
	GLuint numPointLightsLocation_;

	struct
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		GLuint direction;
	} dirLightLocation_;

	struct
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		GLuint position;
		struct
		{
			GLuint kc;
			GLuint kl;
			GLuint kq;
		} atten;
	} pointLightsLocaion_[MAX_POINT_LIGHTS];
};

bool LightingTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/20_PointLight/lighting.vs"))
		return false;

	if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/20_PointLight/lighting.ps"))
		return false;

	if (!Finalize())
		return false;

	WVPLocation_ = GetUniformLocation("gWVP");
	worldMatrixLocation_ = GetUniformLocation("gWorld");
	samplerLocation_ = GetUniformLocation("gSampler");
	eyeWorldPosLocation_ = GetUniformLocation("gEyeWorldPos");
	specularIntensityLocation_ = GetUniformLocation("gSpecularIntensity");
	specularPowerLocation_ = GetUniformLocation("gSpecularPower");
	numPointLightsLocation_ = GetUniformLocation("gNumPointLights");
	dirLightLocation_.color = GetUniformLocation("gDirectionalLight.base.color");
	dirLightLocation_.ambientIntensity = GetUniformLocation("gDirectionalLight.base.ambientIntensity");
	dirLightLocation_.diffuseIntensity = GetUniformLocation("gDirectionalLight.base.diffuseIntensity");
	dirLightLocation_.direction = GetUniformLocation("gDirectionalLight.direction");

	for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		auto& light = pointLightsLocaion_[i];

		const char* p = dopixel::str_format("gPointLights[%d].base.color", i);
		light.color = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].base.ambientIntensity", i);
		light.ambientIntensity = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].base.diffuseIntensity", i);
		light.diffuseIntensity = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].position", i);
		light.position = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].atten.kc", i);
		light.atten.kc = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].atten.kl", i);
		light.atten.kl = GetUniformLocation(p);

		p = dopixel::str_format("gPointLights[%d].atten.kq", i);
		light.atten.kq = GetUniformLocation(p);

		if (light.color == INVALID_UNIFORM || light.ambientIntensity == INVALID_UNIFORM ||
			light.diffuseIntensity == INVALID_UNIFORM || light.position == INVALID_UNIFORM ||
			light.position == INVALID_UNIFORM || light.atten.kc == INVALID_UNIFORM ||
			light.atten.kl == INVALID_UNIFORM || light.atten.kq == INVALID_UNIFORM)
			return false;
	}

	if (dirLightLocation_.ambientIntensity == INVALID_UNIFORM ||
		dirLightLocation_.diffuseIntensity == INVALID_UNIFORM ||
		dirLightLocation_.color == INVALID_UNIFORM ||
		dirLightLocation_.direction == INVALID_UNIFORM ||
		WVPLocation_ == INVALID_UNIFORM ||
		worldMatrixLocation_ == INVALID_UNIFORM ||
		samplerLocation_ == INVALID_UNIFORM ||
		eyeWorldPosLocation_ == INVALID_UNIFORM ||
		specularIntensityLocation_ == INVALID_UNIFORM ||
		specularPowerLocation_ == INVALID_UNIFORM ||
		numPointLightsLocation_ == INVALID_UNIFORM)
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

void LightingTechnique::SetEyeWorldPos(const Vector3f& eyeWorldPos)
{
	glUniform3f(eyeWorldPosLocation_, eyeWorldPos.x, eyeWorldPos.y, eyeWorldPos.z);
}

void LightingTechnique::SetSpecularIntensity(float specularIntensity)
{
	glUniform1f(specularIntensityLocation_, specularIntensity);
}

void LightingTechnique::SetSpecularPower(float specularPower)
{
	glUniform1f(specularPowerLocation_, specularPower);
}

void LightingTechnique::SetDirectionalLight(const DirectionalLight& light)
{
	glUniform3f(dirLightLocation_.color, light.color_.x, light.color_.y, light.color_.z);
	glUniform1f(dirLightLocation_.ambientIntensity, light.ambientIntensity_);
	glUniform1f(dirLightLocation_.diffuseIntensity, light.diffuseIntensity_);
	Vector3f dir = light.direction_;
	dir.Normalize();
	glUniform3f(dirLightLocation_.direction, dir.x, dir.y, dir.z);
}

void LightingTechnique::SetPointLights(unsigned int numLights, const PointLight* pLights)
{
	glUniform1i(numPointLightsLocation_, numLights);
	for (unsigned int i = 0; i < numLights; ++i)
	{
		const auto& loc = pointLightsLocaion_[i];
		const auto& light = pLights[i];

		glUniform1f(loc.ambientIntensity, light.ambientIntensity_);
		glUniform1f(loc.diffuseIntensity, light.diffuseIntensity_);
		glUniform3f(loc.color, light.color_.x, light.color_.y, light.color_.z);
		glUniform3f(loc.position, light.position_.x, light.position_.y, light.position_.z);
		glUniform1f(loc.atten.kc, light.attenuation_.kc);
		glUniform1f(loc.atten.kl, light.attenuation_.kl);
		glUniform1f(loc.atten.kq, light.attenuation_.kq);
	}
}