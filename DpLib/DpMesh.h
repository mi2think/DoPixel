/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   13:02
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpMesh.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpMesh
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Mesh
*********************************************************************/
#ifndef __DP_MESH_H__
#define __DP_MESH_H__

#include "DoPixel.h"
#include "DpAABB.h"

namespace dopixel
{
	class SubMesh
	{
	public:
		SubMesh(Mesh* mesh);
		~SubMesh();

		Mesh* GetMesh() const { return mesh_; }

		const math::AABB& GetBoundingBox() const { return aabb_; }
		void CalculateBoundingBox();

		void SetVisible(bool visible) { visible_ = visible; }
		bool GetVisible() const { return visible_; }

		const MaterialRef& GetMaterial() const { return material_; }
		const IndexBufferRef& GetIndexBuffer() const { return indexBuffer_; }
		const VertexBufferRef& GetVertexBuffer() const { return vertexBuffer_; }

		void SetMaterial(const MaterialRef& material);
		void SetIndexBuffer(const IndexBufferRef& indexBuffer);
		void SetVertexBuffer(const VertexBufferRef& vertexBuffer);
	private:
		// owning mesh
		Mesh* mesh_;
		
		bool visible_;
		math::AABB aabb_;

		MaterialRef material_;
		IndexBufferRef indexBuffer_;
		VertexBufferRef vertexBuffer_;
	};

	class Mesh
	{
	public:
		// only one submesh
		Mesh(const VertexBufferRef& vertexBuffer);
		Mesh(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer);
		Mesh(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer, const MaterialRef& material);
		// from model file
		Mesh(const string& path);
		~Mesh();

		bool Load(const string& path);
		void Clear();

		const string& GetName() const { return name_; }
		const math::AABB& GetBoundingBox() const { return aabb_; }
		void CalculateBoundingBox();

		int GetSubMeshCount() const { return submeshs_.size(); }
		const SubMeshRef& GetSubMesh(int i) const;
	private:
		string name_;
		vector<SubMeshRef> submeshs_;
		math::AABB aabb_;
	};

	// mesh cache
	class MeshCache
	{
	public:
		static MeshCache& Instance() { static MeshCache cache; return cache; }
		MeshCache();
		~MeshCache();

		void Clear();
		MeshRef GetMesh(const string& path);
	private:
		map<string, MeshRef> meshs_;
	};
}


#endif
