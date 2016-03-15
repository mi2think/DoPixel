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
#include "ogl_lights_common.h"

#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

namespace ogl
{
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
			GLuint cutoff;
		} spotLightsLocation_[MAX_SPOT_LIGHTS];
	};
}

#endif
