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

namespace dopixel
{
	using namespace dopixel::math;

	class Material
	{
	public:
		Material(const string& name)
			: name_(name)
			, ambientColor_(0.0f, 0.0f, 0.0f)
			, diffuseColor_(0.0f, 0.0f, 0.0f)
			, emissiveColor_(0.0f, 0.0f, 0.0f)
			, specularColor_(0.0f, 0.0f, 0.0f)
			, shininess_(0.0f)
		{}

		const string& GetName() const { return name_; }

		void SetAmbientColor(const Vector3f& color) { ambientColor_ = color; }
		void SetDiffuseColor(const Vector3f& color) { diffuseColor_ = color; }
		void SetEmissiveColor(const Vector3f& color) { emissiveColor_ = color; }
		void SetSpecularColor(const Vector3f& color) { specularColor_ = color; }

		const Vector3f& GetAmbientColor() const { return ambientColor_; }
		const Vector3f& GetDiffuseColor() const { return diffuseColor_; }
		const Vector3f& GetEmissiveColor() const { return emissiveColor_; }
		const Vector3f& GetSpecularColor() const { return specularColor_; }

		void SetTexture(TextureRef texture) { texture_ = texture; }
		const TextureRef& GetTexture() const { return texture_; }
	private:
		string name_;
		Vector3f ambientColor_;
		Vector3f diffuseColor_;
		Vector3f emissiveColor_;
		Vector3f specularColor_;
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