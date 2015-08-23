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

namespace dopixel
{
	namespace core
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

				// Because stbi load it by [r g b a], we need convert it. :(
				// r g b a -> a r g b
				unsigned char* end = data + size;
				for (unsigned char* p = data; p < end; p += 4)
				{
					unsigned char r = *p;
					*p = *(p + 2);
					*(p + 2) = r;
				}
			}
		}

		void Texture::Release()
		{
			height = 0;
			width = 0;
			free(data);
		}

		void Texture::Sample(Color& color, float u, float v) const
		{
			assert(u >= 0.0f && u <= 1.0f);
			assert(v >= 0.0f && v <= 1.0f);

			int x = int(u * (width - 1));
			int y = int(v * (height - 1));

			Color* p = (Color*)data;
			color = *(p + y * width + x);
		}
	}
}