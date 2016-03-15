#pragma once

#include "ogl_technique.h"
#include "ogl_lights_common.h"
using namespace ogl;

class LightingTechnique : public Technique
{
public:
	static const unsigned int MAX_POINT_LIGHTS = 2;
	static const unsigned int MAX_SPOT_LIGHTS = 2;

	LightingTechnique() {}

	virtual bool Init() override;

	void SetWVP(const Matrix44f& WVP);
	void SetLightWVP(const Matrix44f& lightWVP);
	void SetWorldMatrix(const Matrix44f& worldMatrix);
	void SetTextureUnit(unsigned int textureUnit);
	void SetShadowMapTextureUnit(unsigned int textureUnit);
	void SetEyeWorldPos(const Vector3f& eyeWorldPos);
	void SetSpecularIntensity(float specularIntensity);
	void SetSpecularPower(float power);
	void SetDirectionalLight(const DirectionalLight& light);
	void SetPointLights(unsigned int numLights, const PointLight* pLights);
	void SetSpotLights(unsigned int numLights, const SpotLight* pLights);
private:
	GLuint WVPLocation_;
	GLuint lightWVPLocation_;
	GLuint worldMatrixLocation_;
	GLuint samplerLocation_;
	GLuint shadowMapLocation_;
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
		GLuint cutoff;
	} spotLightsLocation_[MAX_SPOT_LIGHTS];
};

