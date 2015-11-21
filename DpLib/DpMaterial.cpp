/********************************************************************
	created:	2015/07/25
	created:	25:7:2015   21:12
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpMaterial.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpMaterial
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Def material
*********************************************************************/

#include "DpMaterial.h"

namespace dopixel
{
	Material::Material(const string& name)
		: name_(name)
		, ambientColor_(0.0f, 0.0f, 0.0f)
		, diffuseColor_(0.0f, 0.0f, 0.0f)
		, emissiveColor_(0.0f, 0.0f, 0.0f)
		, specularColor_(0.0f, 0.0f, 0.0f)
		, shininess_(0.0f)
	{
	}

	Material::~Material()
	{
	}

	const string& Material::GetName() const
	{
		return name_;
	}

	void Material::SetAmbientColor(const math::Vector3f& color)
	{
		ambientColor_ = color;
	}

	void Material::SetDiffuseColor(const math::Vector3f& color)
	{
		diffuseColor_ = color;
	}

	void Material::SetEmissiveColor(const math::Vector3f& color)
	{
		emissiveColor_ = color;
	}

	void Material::SetSpecularColor(const math::Vector3f& color)
	{
		specularColor_ = color; 
	}

	void Material::SetShininess(float shininess)
	{
		shininess_ = shininess;
	}

	const math::Vector3f& Material::GetAmbientColor() const
	{
		return ambientColor_;
	}

	const math::Vector3f& Material::GetDiffuseColor() const
	{
		return diffuseColor_;
	}

	const math::Vector3f& Material::GetEmissiveColor() const
	{
		return emissiveColor_;
	}

	const math::Vector3f& Material::GetSpecularColor() const
	{
		return specularColor_;
	}

	const float& Material::GetShininess() const
	{
		return shininess_;
	}

	void Material::SetTexture(TextureRef texture)
	{
		texture_ = texture;
	}

	const TextureRef& Material::GetTexture() const
	{
		return texture_;
	}

	MaterialCache::MaterialCache()
	{
	}

	MaterialCache::~MaterialCache()
	{
		Clear();
	}

	MaterialRef MaterialCache::Find(const string& name) const
	{
		auto it = materials_.find(name);
		if (it != materials_.end())
			return it->second;
		return MaterialRef();
	}

	void MaterialCache::Add(const MaterialRef& material)
	{
		const auto& name = material->GetName();
		MaterialRef ref = Find(name);
		if (! ref)
		{
			materials_[name] = material;
		}
	}

	void MaterialCache::Clear()
	{
		materials_.clear();
	}
}