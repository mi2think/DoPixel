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
		Material(const string& name);
		~Material();

		const string& GetName() const;

		void SetAmbientColor( const math::Vector3f& color);
		void SetDiffuseColor( const math::Vector3f& color);
		void SetEmissiveColor(const math::Vector3f& color);
		void SetSpecularColor(const math::Vector3f& color);
		void SetShininess(float shininess);

		const math::Vector3f& GetAmbientColor()  const;
		const math::Vector3f& GetDiffuseColor()  const;
		const math::Vector3f& GetEmissiveColor() const;
		const math::Vector3f& GetSpecularColor() const;
		const float& GetShininess() const;

		void SetTexture(TextureRef texture);
		const TextureRef& GetTexture() const;
	private:
		string name_;
		math::Vector3f ambientColor_;
		math::Vector3f diffuseColor_;
		math::Vector3f emissiveColor_;
		math::Vector3f specularColor_;
		float shininess_;
		TextureRef texture_;
	};

	// material cache
	class MaterialCache
	{
	public:
		MaterialCache();
		~MaterialCache();

		void Clear();
		MaterialRef Find(const string& name) const;
		void Add(const MaterialRef& material);
	private:
		map<string, MaterialRef> materials_;
	};
}

#endif