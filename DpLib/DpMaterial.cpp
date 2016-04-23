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
	Material::Material()
		: shininess_(0.0f)
	{
	}

	Material::Material(const string& name)
		: name_(name)
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

	void Material::SetShininess(float shininess)
	{
		shininess_ = shininess;
	}

	const float& Material::GetShininess() const
	{
		return shininess_;
	}

	void Material::SetColor(ColorUsage::Type usage, const math::Vector3f& color)
	{
		ASSERT(usage < ColorUsage::Max);
		colors_[usage] = color;
	}

	const math::Vector3f& Material::GetColor(ColorUsage::Type usage) const
	{
		ASSERT(usage < ColorUsage::Max);
		return colors_[usage];
	}

	void Material::SetTexture(TextureUsage::Type usage, const TextureRef& texture)
	{
		ASSERT(usage < TextureUsage::Max);
		texture_[usage] = texture;
	}

	const TextureRef& Material::GetTexture(TextureUsage::Type usage) const
	{
		ASSERT(usage < TextureUsage::Max);
		return texture_[usage];
	}

	//////////////////////////////////////////////////////////////////////////

	MaterialCache::MaterialCache()
	{
	}

	MaterialCache::~MaterialCache()
	{
		Clear();
	}

	void MaterialCache::AddMaterial(const string& name, const MaterialRef& material)
	{
		materials_[name] = material;
	}

	MaterialRef MaterialCache::GetMaterial(const string& name)
	{
		ASSERT(!name.empty());

		if (name.empty())
			return MaterialRef();

		auto it = materials_.find(name);
		if (it != materials_.end())
		{
			return it->second;
		}

		return MaterialRef();
	}

	void MaterialCache::Clear()
	{
		materials_.clear();
	}
}