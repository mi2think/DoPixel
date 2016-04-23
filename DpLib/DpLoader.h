/********************************************************************
	created:	2016/04/17
	created:	17:4:2016   19:50
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLoader.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLoader
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Loader
*********************************************************************/
#ifndef __DP_LOADER_H__
#define __DP_LOADER_H__

#include "DoPixel.h"

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace dopixel
{
	class Loader
	{
	public:
		Loader();
		~Loader();

		bool Load(MeshRef& mesh, const string& path);
		bool Load(Mesh* mesh, const string& path);
	private:
		void LoadNode(aiNode* node, const aiScene* scene);
		SubMeshRef LoadMesh(aiMesh* mesh, const aiScene* scene);
		MaterialRef LoadMaterial(aiMaterial* mat);
		TextureRef LoadTexture(aiMaterial* mat, TextureUsage::Type usage);

		string dir_;
	};
}

#endif
