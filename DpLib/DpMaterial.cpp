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
#include "DpTexture.h"

namespace dopixel
{
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