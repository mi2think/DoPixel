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
#include "DpFileStream.h"
#include "stb/stb_image.h"

namespace DoPixel
{
	namespace Core
	{
		Texture::Texture()
			: width(0)
			, height(0)
			, data(nullptr)
		{

		}

		Texture::~Texture()
		{
			Release();
		}

		void Texture::Load(const char* fileName)
		{
			Release();

			FileStream fs(fileName, FileStream::BinaryRead);
			size_t size = (size_t)fs.Size();
			unsigned char* fileData = new unsigned char[size];
			ON_SCOPE_EXIT([&fileData](){ SAFE_DELETEARRAY(fileData); });

			if (fileData)
			{
				size_t readSize = fs.Read(fileData, size);
				if (readSize != size)
				{
					DEBUG_TRACE("Load %s error!\n", fileName);
					return;
				}

				int comp = 0;
				data = stbi_load_from_memory(fileData, size, &width, &height, &comp, 4);
			}
		}

		void Texture::Release()
		{
			height = 0;
			width = 0;
			free(data);
		}
	}
}