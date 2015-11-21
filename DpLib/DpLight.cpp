/********************************************************************
	created:	2015/11/20
	created:	20:11:2015   20:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLight.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLight
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Light
*********************************************************************/
#include "DpLight.h"
#include "DpMaterial.h"

namespace dopixel
{
	Light::Light(
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		const math::Vector3f& attenuation,
		float range)
		: ambient_(ambient)
		, diffuse_(diffuse)
		, specular_(specular)
		, attenuation_(attenuation)
		, range_(range)
	{
	}

	Light::~Light()
	{
	}

	DirectionalLight::DirectionalLight(
		const math::Vector3f& lightWorldDir,
		const math::Vector3f& ambient,
		const math::Vector3f& diffuse,
		const math::Vector3f& specular,
		const math::Vector3f& attenuation,
		float range)
		: Light(ambient, diffuse, specular, attenuation, range)
		, lightWorldDir_(lightWorldDir)
	{
		ASSERT(lightWorldDir_.IsNormalized());
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	void DirectionalLight::BeginLighting(const math::Vector3f& eyeWorldPos, const MaterialRef& material) const
	{
		invLightWorldDir_ = -lightWorldDir_;
		eyeWorldPos_ = eyeWorldPos;
		material_ = material.Get();
	}

	math::Vector3f DirectionalLight::Illuminate(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// ambient color
		math::Vector3f color = material_->GetAmbientColor();
		color *= ambient_;

		float dp = DotProduct(invLightWorldDir_, normal);
		if (dp > math::EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetDiffuseColor();
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;
		}
		return color;
	}

	math::Vector3f DirectionalLight::IlluminateWithSpecular(const math::Vector3f& pos, const math::Vector3f& normal) const
	{
		// ambient color
		math::Vector3f color = material_->GetAmbientColor();
		color *= ambient_;

		float dp = DotProduct(invLightWorldDir_, normal);
		if (dp > math::EPSILON_E5)
		{
			// diffuse color
			math::Vector3f diffuseColor = material_->GetDiffuseColor();
			diffuseColor *= diffuse_;

			color += diffuseColor * dp;

			// specular color
			math::Vector3f vertexToEye = (eyeWorldPos_ - pos);
			vertexToEye.Normalize();
			math::Vector3f lightReflect = (lightWorldDir_ - 2 * normal * DotProduct(lightWorldDir_, normal));
			lightReflect.Normalize();
			float k = DotProduct(vertexToEye, lightReflect);
			if (k > math::EPSILON_E5)
			{
				k = pow(k, material_->GetShininess());
				math::Vector3f specularColor = material_->GetSpecularColor();
				specularColor *= specular_;

				color += specularColor * k;
			}
		}
		return color;
	}

	void DirectionalLight::EndLighting() const
	{
		material_ = nullptr;
		invLightWorldDir_.Zero();
	}
}