/********************************************************************
	created:	2014/08/06
	created:	6:8:2014   22:06
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpLight.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpLight
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Def light
*********************************************************************/

#ifndef __DP_LIGHT__
#define __DP_LIGHT__

#include "DpColor.h"
#include "DpVector4.h"
#include <cassert>
using namespace DoPixel::Math;

namespace DoPixel
{
	namespace Core
	{
		class Light
		{
		public:
			enum
			{
				ATTR_AMBIENT	= 0x1,
				ATTR_INFINITE	= 0x2,
				ATTR_POINT		= 0x4,
				ATTR_SPOTLIGHT1 = 0x8,	// Simple spot light (Use point light with dir)
				ATTR_SPOTLIGHT2 = 0x10,	// Simple spot light

				STATE_ON		= 0x1,	// Open Light
				STATE_OFF		= 0x2,

				MAX_LIGHTS = 8,
			};

			unsigned int id;
			int attr;
			int state;
			Vector4f pos;
			Vector4f dir;	// unit vector
			Color color;

			// Decay factor
			float kc;
			float kl;
			float kq;
			
			// Spot light param
			float angleInner;
			float angleOuter;
			float pf;

			Light()
			: id(0)
			, attr(0)
			, state(0)
			, kc(0)
			, kl(0)
			, kq(0)
			, angleInner(0)
			, angleOuter(0)
			, pf(0)
			{ 
			}
		};

		class LightManager
		{
		public:
			inline void Clear();

			inline unsigned int AddAmbientLight(const Color& color);

			inline unsigned int AddSunLight(const Vector4f& dir, const Color& color);

			inline unsigned int AddPointLight(const Vector4f& pos, const Color& color, float kc, float kl, float kq);

			inline unsigned int AddSpotLight(int attr, const Vector4f& pos, const Vector4f& dir, const Color& color, float kc, float kl, float kq, float angleInner, float angleOuter, float pf);

			inline Light* GetLight(unsigned int lightId);

			std::vector<Light>& GetLights() { return lights; }
		private:
			std::vector<Light> lights;
			unsigned int lightID;
		};
		
		inline void LightManager::Clear()
		{
			lights.clear();

			lightID = 0;
		}

		inline unsigned int LightManager::AddAmbientLight(const Color& color)
		{
			Light light;

			light.id = ++lightID;
			light.attr = Light::ATTR_AMBIENT;
			light.state = Light::STATE_ON;
			light.color = color;

			lights.push_back(light);
			return light.id;
		}

		inline unsigned int LightManager::AddSunLight(const Vector4f& dir, const Color& color)
		{
			Light light;

			light.id = ++lightID;
			light.attr = Light::ATTR_INFINITE;
			light.state = Light::STATE_ON;
			light.dir = dir;
			light.color = color;
			light.dir.Normalize();

			lights.push_back(light);
			return light.id;
		}

		inline unsigned int LightManager::AddPointLight(const Vector4f& pos, const Color& color, float kc, float kl, float kq)
		{
			Light light;

			light.id = ++lightID;
			light.attr = Light::ATTR_POINT;
			light.state = Light::STATE_ON;
			light.pos = pos;
			light.color = color;
			light.kc = kc;
			light.kl = kl;
			light.kq = kq;
	
			lights.push_back(light);
			return light.id;
		}

		inline unsigned int LightManager::AddSpotLight(int attr, const Vector4f& pos, const Vector4f& dir, const Color& color, float kc, float kl, float kq, float angleInner, float angleOuter, float pf)
		{
			assert(attr == Light::ATTR_SPOTLIGHT1 || attr == Light::ATTR_SPOTLIGHT2);

			Light light;

			light.id = ++lightID;
			light.attr = attr;
			light.state = Light::STATE_ON;
			light.pos = pos;
			light.dir = dir;
			light.color = color;
			light.kc = kc;
			light.kl = kl;
			light.kq = kq;
			light.angleInner = angleInner;
			light.angleOuter = angleOuter;
			light.pf = pf;
			light.dir.Normalize();

			lights.push_back(light);
			return light.id;
		}

		inline Light* LightManager::GetLight(unsigned int lightId)
		{
			for (unsigned int i = 0; i < lights.size(); ++i)
			{
				Light& light = lights[i];
				if (light.id == lightId)
					return &light;
			}
			return nullptr;
		}
	}
}


#endif