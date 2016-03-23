/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:28
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_texture.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_texture
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	ogl texture
*********************************************************************/
#include "ogl_texture.h"

#include "DpFileStream.h"
#include "stb/stb_image.h"

namespace ogl
{
	Texture::Texture(GLenum textureTarget, const std::string& fileName)
		: textureTarget_(textureTarget)
		, fileName_(fileName)
		, width_(0)
		, height_(0)
		, data_(nullptr)
	{
	}

	Texture::~Texture()
	{
		if (data_)
		{
			free(data_);
			stbi_image_free(data_);
			data_ = nullptr;
		}
	}

	bool Texture::Load()
	{
		dopixel::FileStream fs(fileName_.c_str(), dopixel::FileStream::BinaryRead);
		size_t size = (size_t)fs.Size();
		unsigned char* fileData = new unsigned char[size];
		dopixel::ON_SCOPE_EXIT([&fileData]() { SAFE_DELETEARRAY(fileData); });

		if (fileData)
		{
			size_t readSize = fs.Read(fileData, size);
			ASSERT(readSize == size);

			int comp = 0;
			data_ = stbi_load_from_memory(fileData, size, &width_, &height_, &comp, 4);
			if (data_ != nullptr)
			{
				glGenTextures(1, &textureObj_);
				glBindTexture(textureTarget_, textureObj_);
				glTexImage2D(textureTarget_, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
				glTexParameterf(textureTarget_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(textureTarget_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(textureTarget_, 0);
				return true;
			}
		}
		return false;
	}

	void Texture::Bind(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(textureTarget_, textureObj_);
	}
}