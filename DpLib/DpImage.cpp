/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   21:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpImage.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpImage
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Image
*********************************************************************/
#include "DpImage.h"
#include "DpFileStream.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace dopixel
{
	Image::Image()
		: width_(0)
		, height_(0)
		, format_(ImageFormat::Unknown)
		, bytesPerPixel_(0)
		, dataSize_(0)
		, data_(nullptr)
	{
	}

	Image::Image(int width, int height, ImageFormat::Type format)
		: width_(width)
		, height_(height)
		, format_(format)
	{
		bytesPerPixel_ = Image::GetBytesPerPixel(format_);
		dataSize_ = width_ * height_ * bytesPerPixel_;
		data_ = malloc(dataSize_);
	}

	Image::Image(const void* data, int length, ImageFormat::Type format)
	{
		int comp = 0;
		int result = stbi_info_from_memory((const stbi_uc*)data, length, &width_, &height_, &comp);
		ASSERT(result && "error: stbi not suppot it");

		if (! result)
		{
			dataSize_ = length;
			format_ = format;
			data_ = malloc(dataSize_);
			memcpy(data_, data, dataSize_);
			bytesPerPixel_ = Image::GetBytesPerPixel(format_);
		}
	}

	Image::~Image()
	{
		if (data_ != nullptr)
			free(data_);
	}

	int Image::GetBytesPerPixel(ImageFormat::Type format)
	{
		switch (format)
		{
		case ImageFormat::RGB:
			return 3;
			break;
		case ImageFormat::RGBA:
		case ImageFormat::ARGB:
			return 4;
			break;
		case ImageFormat::FLOAT4:
			return 16;
			break;
		default:
			ASSERT(0 && "error: unsupported image format");
			break;
		}
		return 0;
	}

	ImageRef Image::FromFile(const string& path)
	{
		FileStream fs(path, FileStream::BinaryRead);
		size_t size = (size_t)fs.Size();
		unsigned char* fileData = new unsigned char[size];
		ON_SCOPE_EXIT([&fileData]() { SAFE_DELETEARRAY(fileData); });

		size_t readSize = fs.Read(fileData, size);
		if (readSize != size)
		{
			LOG_ERROR("error: load %s failed!\n", path.c_str());
			return ImageRef();
		}

		int width = 0;
		int height = 0;
		int comp = 0;
		stbi_uc* stbi_data = stbi_load_from_memory(fileData, size, &width, &height, &comp, 4);
		if (!stbi_data)
		{
			LOG_ERROR("error: stbi decode image %s filed!\n", path.c_str());
			return ImageRef();
		}

		ImageRef image(new Image(width, height, ImageFormat::RGBA));
		void* data = image->GetData();
		memcpy(data, stbi_data, image->GetBytesPerPixel() * width * height);

		stbi_image_free(stbi_data);

		return image;
	}
}