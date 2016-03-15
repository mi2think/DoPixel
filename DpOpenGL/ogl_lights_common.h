/********************************************************************
	created:	2016/03/15
	created:	15:3:2016   10:56
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_lights_common.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_lights_common
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	ogl lights common
*********************************************************************/
#ifndef __OGL_LIGHTS_COMMON_H__
#define __OGL_LIGHTS_COMMON_H__

#include "DpVector3.h"
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
}

#endif