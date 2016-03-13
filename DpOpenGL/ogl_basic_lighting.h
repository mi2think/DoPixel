/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   22:04
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_basic_lighting.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_basic_lighting
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl basic lighting
*********************************************************************/
#ifndef __OGL_BASIC_LIGHTING_H__
#define __OGL_BASIC_LIGHTING_H__

#include "ogl_technique.h"

#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

namespace ogl
{
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

	class BasicLightingTechnique : public Technique
	{
	public:
		static const unsigned int MAX_POINT_LIGHTS = 2;
		static const unsigned int MAX_SPOT_LIGHTS = 2;

		BasicLightingTechnique() {}

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
}

#endif
