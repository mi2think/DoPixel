#include "lighting_technique.h"

#define INVALID_UNIFORM 0xffffffff

bool LightingTechnique::Init()
{
	if (!Technique::Init())
		return false;

	if (!AddShaderFromFile(GL_VERTEX_SHADER, "../DpOpenGLDemo/24_ShadowMapping2/lighting.vs"))
		return false;

	if (!AddShaderFromFile(GL_FRAGMENT_SHADER, "../DpOpenGLDemo/24_ShadowMapping2/lighting.ps"))
		return false;

	if (!Finalize())
		return false;

	WVPLocation_ = GetUniformLocation("gWVP");
	lightWVPLocation_ = GetUniformLocation("gLightWVP");
	worldMatrixLocation_ = GetUniformLocation("gWorld");
	samplerLocation_ = GetUniformLocation("gSampler");
	shadowMapLocation_ = GetUniformLocation("gShadowMap");
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

		p = dopixel::str_format("gSpotLights[%d].cutoff", i);
		light.cutoff = GetUniformLocation(p);

		if (light.color == INVALID_UNIFORM || light.ambientIntensity == INVALID_UNIFORM ||
			light.diffuseIntensity == INVALID_UNIFORM || light.position == INVALID_UNIFORM ||
			light.position == INVALID_UNIFORM || light.atten.kc == INVALID_UNIFORM ||
			light.atten.kl == INVALID_UNIFORM || light.atten.kq == INVALID_UNIFORM ||
			light.direction == INVALID_UNIFORM || light.cutoff == INVALID_UNIFORM)
			return false;
	}


	if (directionalLight_.ambientIntensity == INVALID_UNIFORM ||
		directionalLight_.diffuseIntensity == INVALID_UNIFORM ||
		directionalLight_.color == INVALID_UNIFORM ||
		directionalLight_.direction == INVALID_UNIFORM ||
		WVPLocation_ == INVALID_UNIFORM ||
		lightWVPLocation_ == INVALID_UNIFORM ||
		worldMatrixLocation_ == INVALID_UNIFORM ||
		samplerLocation_ == INVALID_UNIFORM ||
		shadowMapLocation_ == INVALID_UNIFORM ||
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

void LightingTechnique::SetLightWVP(const Matrix44f& lightWVP)
{
	glUniformMatrix4fv(lightWVPLocation_ , 1, GL_TRUE, &(lightWVP.m[0][0]));
}

void LightingTechnique::SetWorldMatrix(const Matrix44f& worldMatrix)
{
	glUniformMatrix4fv(worldMatrixLocation_, 1, GL_TRUE, &(worldMatrix.m[0][0]));
}

void LightingTechnique::SetTextureUnit(unsigned int textureUnit)
{
	glUniform1i(samplerLocation_, textureUnit);
}

void LightingTechnique::SetShadowMapTextureUnit(unsigned int textureUnit)
{
	glUniform1i(shadowMapLocation_, textureUnit);
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
		glUniform1f(loc.cutoff, cos(light.cutoff));
	}
}