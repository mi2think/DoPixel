/********************************************************************
	created:	2016/03/23
	created:	23:3:2016   11:40
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_cubemap_texture.cpp
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_cubemap_texture
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	ogl cubemap texture
*********************************************************************/
#include "ogl_cubemap_texture.h"

#include "DpFileStream.h"
#include "stb/stb_image.h"

namespace ogl
{
	static const GLenum types[CubemapTexture::CUBEMAP_TEXTURE_NUM] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	CubemapTexture::CubemapTexture(const std::string& directory,
		const std::string& posXFileName,
		const std::string& negXFileName,
		const std::string& posYFileName,
		const std::string& negYFileName,
		const std::string& posZFileName,
		const std::string& negZFileName)
	{
		std::string baseDir = directory;
		auto it = directory.cend();
		--it;
		if (*it != '/')
			baseDir += "/";

		fileNames_[0] = baseDir + posXFileName;
		fileNames_[1] = baseDir + negXFileName;
		fileNames_[2] = baseDir + posYFileName;
		fileNames_[3] = baseDir + negYFileName;
		fileNames_[4] = baseDir + posZFileName;
		fileNames_[5] = baseDir + negZFileName;

		textureObj_ = 0;
	}

	CubemapTexture::~CubemapTexture()
	{
		if (textureObj_ != 0)
			glDeleteTextures(1, &textureObj_);
	}

	bool CubemapTexture::Load()
	{
		glGenTextures(1, &textureObj_);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj_);

		dopixel::FileStream fs;
		for (int i = 0; i < CUBEMAP_TEXTURE_NUM; ++i)
		{
			if (! fs.Open(fileNames_[i], dopixel::FileStream::BinaryRead))
			{
				fprintf(stderr, "error: failed to open %s", fileNames_[i].c_str());
				return false;
			}

			// load file to memory
			size_t fileSize = (size_t)fs.Size();
			unsigned char* data = new unsigned char[fileSize];
			size_t readSize = fs.Read(data, fileSize);
			ASSERT(readSize == fileSize);

			// get image data
			int comp = 0;
			int w = 0;
			int h = 0;
			unsigned char* imageData = stbi_load_from_memory(data, fileSize, &w, &h, &comp, 4);
			if (imageData)
			{
				glTexImage2D(types[i], 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				stbi_image_free(imageData);
			}
		}

		return true;
	}

	void CubemapTexture::Bind(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj_);
	}
}