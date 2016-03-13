/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:32
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_mesh.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_mesh
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl mesh
*********************************************************************/
#ifndef __OGL_MESH_H__
#define __OGL_MESH_H__

#include "DpVector2.h"
#include "DpVector3.h"

#include <GL/glew.h>
#include <string>
#include <vector>

using namespace dopixel::math;

struct aiScene;
struct aiMesh;

namespace ogl
{
	class Texture;
	class Mesh
	{
	public:
		struct Vertex
		{
			Vector3f pos;
			Vector2f tex;
			Vector3f normal;

			Vertex() {}
			Vertex(const Vector3f& _pos, const Vector2f& _tex, const Vector3f& _normal)
				: pos(_pos)
				, tex(_tex)
				, normal(_normal)
			{}
		};

		Mesh();
		~Mesh();

		bool LoadMesh(const std::string& fileName);

		void Render();
	private:
		bool InitFromScene(const aiScene* scene, const std::string& fileName);
		void InitMesh(unsigned int index, const aiMesh* mesh);
		bool InitMaterials(const aiScene* scene, const std::string& fileName);
		void Clear();

		struct MeshEntry
		{
			MeshEntry();
			~MeshEntry();

			void Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices);
			GLuint VB_;
			GLuint IB_;
			unsigned int numIndices_;
			unsigned int materialIndex_;
		};

		std::vector<MeshEntry> entries_;
		std::vector<Texture*> textures_;
	};
}

#endif
