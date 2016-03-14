/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:35
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_mesh.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_mesh
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl mesh
*********************************************************************/
#include "ogl_mesh.h"
#include "ogl_texture.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <cassert>

namespace ogl
{
#define INVALID_OGL_VALUE	0xffffffff
#define INVALID_MATERIAL 0xffffffff

	Mesh::MeshEntry::MeshEntry()
	{
		VB_ = INVALID_OGL_VALUE;
		IB_ = INVALID_OGL_VALUE;
		numIndices_ = 0;
		materialIndex_ = INVALID_MATERIAL;
	}

	Mesh::MeshEntry::~MeshEntry()
	{
		if (VB_ != INVALID_OGL_VALUE)
		{
			glDeleteBuffers(1, &VB_);
		}
		if (IB_ != INVALID_OGL_VALUE)
		{
			glDeleteBuffers(1, &IB_);
		}
	}

	void Mesh::MeshEntry::Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices)
	{
		numIndices_ = indices.size();

		glGenBuffers(1, &VB_);
		glBindBuffer(GL_ARRAY_BUFFER, VB_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &IB_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices_, &indices[0], GL_STATIC_DRAW);
	}

	Mesh::Mesh()
	{
	}

	Mesh::~Mesh()
	{
		Clear();
	}

	void Mesh::Clear()
	{
		for (auto texture : textures_)
		{
			delete texture;
		}
		textures_.clear();
		entries_.clear();
	}

	bool Mesh::LoadMesh(const std::string& fileName)
	{
		Clear();

		bool ret = false;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
		if (scene)
		{
			ret = InitFromScene(scene, fileName);
		}
		else
		{
			fprintf(stderr, "error parsing '%s' : '%s'", fileName.c_str(), importer.GetErrorString());
		}
		return ret;
	}

	bool Mesh::InitFromScene(const aiScene* scene, const std::string& fileName)
	{
		entries_.resize(scene->mNumMeshes);
		textures_.resize(scene->mNumMaterials);

		for (unsigned int i = 0; i < entries_.size(); ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			InitMesh(i, mesh);
		}

		return InitMaterials(scene, fileName);
	}

	void Mesh::InitMesh(unsigned int index, const aiMesh* mesh)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		const aiVector3D zero(0.0f, 0.0f, 0.0f);
		// vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			const aiVector3D* pos = &(mesh->mVertices[i]);
			const aiVector3D* normal = &(mesh->mNormals[i]);
			const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;

			Vertex v(Vector3f(pos->x, pos->y, pos->z), Vector2f(texCoord->x, texCoord->y), Vector3f(normal->x, normal->y, normal->z));
			vertices.push_back(v);
		}
		// indices
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		entries_[index].materialIndex_ = mesh->mMaterialIndex;
		entries_[index].Init(vertices, indices);
	}

	bool Mesh::InitMaterials(const aiScene* scene, const std::string& fileName)
	{
		auto slashIndex = fileName.find_last_of("/");
		std::string dir;
		if (slashIndex == std::string::npos)
			dir = ".";
		else if (slashIndex == 0)
			dir = "/";
		else
			dir = fileName.substr(0, slashIndex);

		bool ret = true;
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			const aiMaterial* material = scene->mMaterials[i];
			textures_[i] = nullptr;

			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString path;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
				{
					std::string fullPath = dir + "/" + path.data;
					textures_[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());
					if (!textures_[i]->Load())
					{
						fprintf(stderr, "error loading texture '%s'\n", fullPath.c_str());
						delete textures_[i];
						textures_[i] = nullptr;
						ret = false;
					}
					else
					{
						fprintf(stdout, "loaded texture '%s'\n", fullPath.c_str());
					}
				}
			}

			// loading a white texture in case model does not include its own texture
			//if (!textures_[i])
			//{
			//	textures_[i] = new Texture(GL_TEXTURE_2D, "../Resource/white.png");
			//	ret = textures_[i]->Load();
			//}
		}

		return ret;
	}

	void Mesh::Render()
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		for (unsigned int i = 0; i < entries_.size(); ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, entries_[i].VB_);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries_[i].IB_);

			const unsigned int materialIndex = entries_[i].materialIndex_;
			if (materialIndex < textures_.size() && textures_[materialIndex] != nullptr)
			{
				textures_[materialIndex]->Bind(GL_TEXTURE0);
			}

			glDrawElements(GL_TRIANGLES, entries_[i].numIndices_, GL_UNSIGNED_INT, 0);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}
