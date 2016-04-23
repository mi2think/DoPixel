/********************************************************************
	created:	2016/04/17
	created:	17:4:2016   19:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLoader.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLoader
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Loader
*********************************************************************/
#include "DpLoader.h"
#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMaterial.h"
#include "DpVertexBuffer.h"
#include "DpIndexBuffer.h"
#include "DpTexture.h"
#include "DpMesh.h"
#include "DpFileStream.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace dopixel
{
	Mesh* loadmesh = nullptr;

	Loader::Loader()
	{
	}

	Loader::~Loader()
	{
	}

	bool Loader::Load(MeshRef& mesh, const string& path)
	{
		return Load(mesh.Get(), path);
	}

	bool Loader::Load(Mesh* mesh, const string& path)
	{
		loadmesh = mesh;

		auto slashIndex = path.find_last_of("/");
		if (slashIndex == string::npos)
			dir_ = ".";
		else if (slashIndex == 0)
			dir_ = "/";
		else
			dir_ = path.substr(0, slashIndex);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("error: ASSIMP read %s failed!\n", path.c_str());
			return false;
		}

		LoadNode(scene->mRootNode, scene);

		return true;
	}

	void Loader::LoadNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			int meshIndex = node->mMeshes[i];
			aiMesh* mesh = scene->mMeshes[meshIndex];
			SubMeshRef subMesh = LoadMesh(mesh, scene);
			loadmesh->AddSubMesh(subMesh);
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			LoadNode(node->mChildren[i], scene);
		}
	}

	SubMeshRef Loader::LoadMesh(aiMesh* mesh, const aiScene* scene)
	{
		SubMeshRef submesh(new SubMesh());
		// vertex buffer
		VertexBufferRef vertexBuffer(new VertexBuffer);

		int vertexCount = mesh->mNumVertices;
		Ref<VertexArray3f> positions(new VertexArray3f(vertexCount));
		Ref<VertexArray3f> normals(new VertexArray3f(vertexCount));
		Ref<VertexArray2f> uvs(new VertexArray2f(vertexCount));

		math::Vector3f* position = positions->DataAs<math::Vector3f>();
		math::Vector3f* normal = normals->DataAs<math::Vector3f>();
		math::Vector2f* uv = uvs->DataAs<math::Vector2f>();
		for (int i = 0; i < vertexCount; ++i)
		{
			position->x = mesh->mVertices[i].x;
			position->y = mesh->mVertices[i].y;
			position->z = mesh->mVertices[i].z;

			normal->x = mesh->mNormals[i].x;
			normal->y = mesh->mNormals[i].y;
			normal->z = mesh->mNormals[i].z;

			if (mesh->mTextureCoords[0])
			{
				uv->x = mesh->mTextureCoords[0][i].x;
				uv->y = mesh->mTextureCoords[0][i].y;
			}

			++position;
			++normal;
			++uv;
		}

		vertexBuffer->SetPositions(positions);
		vertexBuffer->SetNormals(normals);
		if (mesh->mTextureCoords[0])
		{
			vertexBuffer->SetTexCoords(uvs);
		}
		submesh->SetVertexBuffer(vertexBuffer);

		// index buffer
		int primitiveCount = mesh->mNumFaces;
		IndexBufferRef indexBuffer(new IndexBuffer(PrimitiveType::Triangles, primitiveCount));
		unsigned int* index = indexBuffer->GetData();
		for (int i = 0; i < primitiveCount; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			ASSERT(face.mNumIndices == 3);
			*index = face.mIndices[0];
			*(index + 1) = face.mIndices[1];
			*(index + 2) = face.mIndices[2];

			index += 3;
		}
		submesh->SetIndexBuffer(indexBuffer);

		// material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			MaterialRef material = LoadMaterial(mat);
			submesh->SetMaterial(material);
		}
		return submesh;
	}

	MaterialRef Loader::LoadMaterial(aiMaterial* mat)
	{
		aiString matName;
		aiGetMaterialString(mat, AI_MATKEY_NAME, &matName);

		MaterialRef material = MaterialCache::Instance().GetMaterial(matName.C_Str());
		if (material != nullptr)
		{
			return material;
		}
		
		material = new Material();

		// texture
		TextureRef diffuseTex = LoadTexture(mat, TextureUsage::Diffuse);
		TextureRef specularTex = LoadTexture(mat, TextureUsage::Specular);
		TextureRef normalTex = LoadTexture(mat, TextureUsage::Normal);
		material->SetTexture(TextureUsage::Diffuse, diffuseTex);
		material->SetTexture(TextureUsage::Specular, specularTex);
		material->SetTexture(TextureUsage::Normal, normalTex);

		aiReturn e = aiReturn_SUCCESS;

		// shininess
		float shininess = 0.0f;
		e = aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);
		ASSERT(e == aiReturn_SUCCESS);
		material->SetShininess(shininess);

		// color
		aiColor4D color;
		e = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color);
		ASSERT(e == aiReturn_SUCCESS);
		material->SetColor(ColorUsage::Ambient, math::Vector3f(color.r, color.g, color.b));

		e = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color);
		ASSERT(e == aiReturn_SUCCESS);
		material->SetColor(ColorUsage::Specular, math::Vector3f(color.r, color.g, color.b));

		e = aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &color);
		ASSERT(e == aiReturn_SUCCESS);
		material->SetColor(ColorUsage::Emissive, math::Vector3f(color.r, color.g, color.b));

		MaterialCache::Instance().AddMaterial(matName.C_Str(), material);
		return material;
	}

	TextureRef Loader::LoadTexture(aiMaterial* mat, TextureUsage::Type usage)
	{
		aiTextureType type = aiTextureType_DIFFUSE;
		switch (usage)
		{
		case TextureUsage::Diffuse:
			type = aiTextureType_DIFFUSE;
			break;
		case TextureUsage::Specular:
			type = aiTextureType_SPECULAR;
			break;
		case TextureUsage::Normal:
			type = aiTextureType_NORMALS;
			break;
		}

		auto textureCount = mat->GetTextureCount(type);
		if (textureCount > 0)
		{
			// just support one texture each type
			aiString str;
			mat->GetTexture(type, 0, &str);
			string path = dir_ + string("/") + str.C_Str();
			return TextureCache::Instance().GetTexture(path);
		}
		return TextureRef();
	}
}
