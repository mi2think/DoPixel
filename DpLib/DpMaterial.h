/********************************************************************
	created:	2014/08/06
	created:	6:8:2014   21:10
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMaterial.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMaterial
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Def material
*********************************************************************/

#ifndef __DP_MATERIAL__
#define __DP_MATERIAL__

#include "DoPixel.h"
#include "DpVector3.h"
#include "DpTexture.h"

namespace dopixel
{
	class Material
	{
	public:
		Material();
		Material(const string& name);
		~Material();

		const string& GetName() const;
		
		void SetShininess(float shininess);
		const float& GetShininess() const;

		void SetColor(ColorUsage::Type usage, const math::Vector3f& color);
		const math::Vector3f& GetColor(ColorUsage::Type usage) const;

		void SetTexture(TextureUsage::Type usage, const TextureRef& texture);
		const TextureRef& GetTexture(TextureUsage::Type usage) const;
	private:
		string name_;
		float shininess_;
		math::Vector3f colors_[ColorUsage::Max];
		TextureRef texture_[TextureUsage::Max];
	};
}

#endif