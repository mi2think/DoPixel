/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   13:06
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpMesh.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpMesh
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Mesh
*********************************************************************/
#include "DpMesh.h"

namespace dopixel
{
	SubMesh::SubMesh(Mesh* mesh)
		: mesh_(mesh)
		, visible_(true)
	{
	}

	void SubMesh::CalculateBoundingBox()
	{
		ASSERT(vertexBuffer_);

		const auto& positions = vertexBuffer_->GetPositions();
		const int vertexCount = vertexBuffer_->GetVertexCount();
		const math::Vector3f* vec3 = positions->DataAs<math::Vector3f>();

		aabb_.Reset();
		if (indexBuffer_)
		{
			const unsigned int* indices = indexBuffer_->GetData();
			const int indexCount = indexBuffer_->GetIndexCount();
			for (int i = 0; i < indexCount; ++i)
			{
				unsigned int index = *(indices + i);
				aabb_.Add(*(vec3 + index));
			}
		}
		else
		{
			for (int i = 0; i < vertexCount; ++i)
				aabb_.Add(*(vec3 + i));
		}
	}

	Mesh::Mesh(const VertexBufferRef& vertexBuffer)
	{
		SubMeshRef submesh(new SubMesh(this));
		submesh->SetVertexBuffer(vertexBuffer);
		submesh->CalculateBoundingBox();
		submeshs_.push_back(submesh);
	}

	Mesh::Mesh(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer)
	{
		SubMeshRef submesh(new SubMesh(this));
		submesh->SetVertexBuffer(vertexBuffer);
		submesh->SetIndexBuffer(indexBuffer);
		submesh->CalculateBoundingBox();
		submeshs_.push_back(submesh);
	}

	Mesh::Mesh(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer, const MaterialRef& material)
	{
		SubMeshRef submesh(new SubMesh(this));
		submesh->SetVertexBuffer(vertexBuffer);
		submesh->SetIndexBuffer(indexBuffer);
		submesh->SetMaterial(material);
		submesh->CalculateBoundingBox();
		submeshs_.push_back(submesh);
	}

	Mesh::Mesh(const string& path)
		: name_(path)
	{
		Load(path);
	}

	Mesh::~Mesh()
	{
		Clear();
	}

	bool Mesh::Load(const string& path)
	{
		//TODO: load mesh from model file
		return true;
	}

	void Mesh::Clear()
	{
		submeshs_.clear();
	}

	void Mesh::CalculateBoundingBox()
	{
		aabb_.Reset();
		for (auto& submesh : submeshs_)
		{
			const auto& aabb = submesh->GetBoundingBox();
			aabb_.Add(aabb);
		}
	}

	const SubMeshRef& Mesh::GetSubMesh(int i) const
	{
		ASSERT(i < (int)submeshs_.size());
		return submeshs_[i];
	}

	MeshCache::MeshCache()
	{
	}

	MeshCache::~MeshCache()
	{
		Clear();
	}

	void MeshCache::Clear()
	{
		meshs_.clear();
	}

	MeshRef MeshCache::GetMesh(const string& path)
	{
		ASSERT(!path.empty());

		if (path.empty())
			return MeshRef();

		auto it = meshs_.find(path);
		if (it != meshs_.end())
		{
			return it->second;
		}
		else
		{
			MeshRef mesh(new Mesh(path));
			meshs_[path] = mesh;
			return mesh;
		}
	}
}
