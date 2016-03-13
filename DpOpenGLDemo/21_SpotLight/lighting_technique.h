#pragma once

#include "ogl_technique.h"
using namespace ogl;

#define INVALID_UNIFORM 0xffffffff

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

struct SpotLight : public PointLight
{
	Vector3f direction_;
	float theta_;
	float phi_;
	float pf_;

	SpotLight()
		: direction_(0, 0, 0)
		, theta_(0)
		, phi_(0)
		, pf_(1)
	{}
};

class LightingTechnique : public Technique
{
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;
	static const unsigned int MAX_SPOT_LIGHTS = 2;

	LightingTechnique() {}

	virtual bool Init() override;

	void SetWVP(const Matrix44f& WVP);
	void SetWorldMatrix(const Matrix44f& worldMatrix);
	void SetTextureUnit(unsigned int textureUnit);
	void SetEyeWorldPos(const Vector3f& eyeWorldPos);
	void SetSpecularIntensity(float specularIntensity);
	void SetSpecularPower(float power);
	void SetDirectionalLight(const DirectionalLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);
	void SetSpotLights(unsigned int numLights, const SpotLight* pLights);
private:
	GLuint WVPLocation_;
	GLuint worldMatrixLocation_;
	GLuint samplerLocation_;
	GLuint eyeWorldPosLocation_;
	GLuint specularIntensityLocation_;
	GLuint specularPowerLocation_;

	GLuint numPointLightsLocation_;
	GLuint numSpotLightsLocation_;

	struct  
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		GLuint direction;
	} directionalLight_;

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
	} pointLightsLocation_[MAX_POINT_LIGHTS];

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
		
		GLuint direction;
		GLuint cosThetaOver2;
		GLuint cosPhiOver2;
		GLuint pf;
	} spotLightsLocation_[MAX_SPOT_LIGHTS];
};

bool LightingTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/21_SpotLight/lighting.vs"))
		return false;

	if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/21_SpotLight/lighting.ps"))
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
	numSpotLightsLocation_ = GetUniformLocation("gNumSpotLights");

	directionalLight_.color = GetUniformLocation("gDirectionalLight.base.color");
	directionalLight_.ambientIntensity = GetUniformLocation("gDirectionalLight.base.ambientIntensity");
	directionalLight_.diffuseIntensity = GetUniformLocation("gDirectionalLight.base.diffuseIntensity");
	directionalLight_.direction = GetUniformLocation("gDirectionalLight.direction");

	for (unsigned int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		auto& light = pointLightsLocation_[i];

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

	for (unsigned int i = 0; i < MAX_SPOT_LIGHTS; ++i)
	{
		auto& light = spotLightsLocation_[i];

		const char* p = dopixel::str_format("gSpotLights[%d].pl.base.color", i);
		light.color = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.base.ambientIntensity", i);
		light.ambientIntensity = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.base.diffuseIntensity", i);
		light.diffuseIntensity = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.position", i);
		light.position = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.atten.kc", i);
		light.atten.kc = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.atten.kl", i);
		light.atten.kl = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pl.atten.kq", i);
		light.atten.kq = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].direction", i);
		light.direction = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].cosThetaOver2", i);
		light.cosThetaOver2 = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].cosPhiOver2", i);
		light.cosPhiOver2 = GetUniformLocation(p);

		p = dopixel::str_format("gSpotLights[%d].pf", i);
		light.pf = GetUniformLocation(p);

		if (light.color == INVALID_UNIFORM || light.ambientIntensity == INVALID_UNIFORM ||
			light.diffuseIntensity == INVALID_UNIFORM || light.position == INVALID_UNIFORM ||
			light.position == INVALID_UNIFORM || light.atten.kc == INVALID_UNIFORM ||
			light.atten.kl == INVALID_UNIFORM || light.atten.kq == INVALID_UNIFORM ||
			light.direction == INVALID_UNIFORM  || light.cosThetaOver2 == INVALID_UNIFORM ||
			light.cosPhiOver2 == INVALID_UNIFORM || light.pf == INVALID_UNIFORM)
			return false;
	}


	if (directionalLight_.ambientIntensity == INVALID_UNIFORM ||
		directionalLight_.diffuseIntensity == INVALID_UNIFORM ||
		directionalLight_.color == INVALID_UNIFORM ||
		directionalLight_.direction == INVALID_UNIFORM ||
		WVPLocation_ == INVALID_UNIFORM ||
		worldMatrixLocation_ == INVALID_UNIFORM ||
		samplerLocation_ == INVALID_UNIFORM ||
		eyeWorldPosLocation_ == INVALID_UNIFORM ||
		specularIntensityLocation_ == INVALID_UNIFORM ||
		specularPowerLocation_ == INVALID_UNIFORM ||
		numPointLightsLocation_ == INVALID_UNIFORM ||
		numSpotLightsLocation_ == INVALID_UNIFORM)
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
	glUniform3f(directionalLight_.color, light.color_.x, light.color_.y, light.color_.z);
	glUniform1f(directionalLight_.ambientIntensity, light.ambientIntensity_);
	glUniform1f(directionalLight_.diffuseIntensity, light.diffuseIntensity_);
	Vector3f dir = light.direction_;
	dir.Normalize();
	glUniform3f(directionalLight_.direction, dir.x, dir.y, dir.z);
}

void LightingTechnique::SetPointLights(unsigned int numLights, const PointLight* pLights)
{
	glUniform1i(numPointLightsLocation_, numLights);
	for (unsigned int i = 0; i < numLights; ++i)
	{
		const auto& loc = pointLightsLocation_[i];
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

void LightingTechnique::SetSpotLights(unsigned int numLights, const SpotLight* pLights)
{
	glUniform1i(numSpotLightsLocation_, numLights);
	for (unsigned int i = 0; i < numLights; ++i)
	{
		const auto& loc = spotLightsLocation_[i];
		const auto& light = pLights[i];

		glUniform1f(loc.ambientIntensity, light.ambientIntensity_);
		glUniform1f(loc.diffuseIntensity, light.diffuseIntensity_);
		glUniform3f(loc.color, light.color_.x, light.color_.y, light.color_.z);
		glUniform3f(loc.position, light.position_.x, light.position_.y, light.position_.z);
		glUniform1f(loc.atten.kc, light.attenuation_.kc);
		glUniform1f(loc.atten.kl, light.attenuation_.kl);
		glUniform1f(loc.atten.kq, light.attenuation_.kq);

		Vector3f dir = light.direction_;
		dir.Normalize();
		glUniform3f(loc.direction, dir.x, dir.y, dir.z);

		glUniform1f(loc.cosThetaOver2, cos(light.theta_ / 2.0f));
		glUniform1f(loc.cosPhiOver2, cos(light.phi_ / 2.0f));

		glUniform1f(loc.pf, light.pf_);
	}
}
