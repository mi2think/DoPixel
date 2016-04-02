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
		, format_(PixelFormat::Unknown)
		, bytesPerPixel_(0)
		, dataSize_(0)
		, data_(nullptr)
	{
	}

	Image::Image(int width, int height, PixelFormat::Type format)
		: width_(width)
		, height_(height)
		, format_(format)
	{
		bytesPerPixel_ = Image::GetBytesPerPixel(format_);
		dataSize_ = width_ * height_ * bytesPerPixel_;
		data_ = malloc(dataSize_);
	}

	Image::Image(const void* data, int length, PixelFormat::Type format)
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

	void Image::SaveTGA(const string& path)
	{
		// convert to RGBA
		ImageConverter converter(this);
		ImageRef destImage = converter.Convert(PixelFormat::RGBA);
		unsigned char* destData = (unsigned char*)destImage->GetData();

		// write tga
		FileStream fs(path.c_str(), FileStream::BinaryWrite);
		struct TGAHeader
		{
			char idlength;
			char colourmaptype;
			char datatypecode;
			short int colourmapOrigin;
			short int colourmapLength;
			char colourmapDepth;
			short int x_origin;
			short int y_origin;
			short int width;
			short int height;
			char bitsperpixel;
			char imagedescriptor;
		};
		TGAHeader header;

		memset(&header, 0, sizeof(TGAHeader));
		header.datatypecode = 2;	// uncompressed RGB
		header.width = (short int)width_;
		header.height = (short int)height_;
		header.bitsperpixel = 32;

		// write out the TGA header
		fs.Write((char*)&header.idlength, 1);
		fs.Write((char*)&header.colourmaptype, 1);
		fs.Write((char*)&header.datatypecode, 1);
		fs.Write((char*)&header.colourmapOrigin, 2);
		fs.Write((char*)&header.colourmapLength, 2);
		fs.Write((char*)&header.colourmapDepth, 1);
		fs.Write((char*)&header.x_origin, 2);
		fs.Write((char*)&header.y_origin, 2);
		fs.Write((char*)&header.width, 2);
		fs.Write((char*)&header.height, 2);
		fs.Write((char*)&header.bitsperpixel, 1);
		fs.Write((char*)&header.imagedescriptor, 1);

		const unsigned char* src = destData;
		for (int y = height_ - 1; y >= 0; --y)
		{
			const unsigned char* row = src + y * width_ * 4;
			for (int x = 0; x < width_ * 4; x += 4)
			{
				unsigned char r = row[x];
				unsigned char g = row[x + 1];
				unsigned char b = row[x + 2];
				unsigned char a = row[x + 3];
				fs.Write(&b, 1);
				fs.Write(&g, 1);
				fs.Write(&r, 1);
				fs.Write(&a, 1);
			}
		}

		fs.Close();
	}

	int Image::GetBytesPerPixel(PixelFormat::Type format)
	{
		switch (format)
		{
		case PixelFormat::RGB:
			return 3;
			break;
		case PixelFormat::RGBA:
		case PixelFormat::ARGB:
			return 4;
			break;
		case PixelFormat::FLOAT4:
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

		ImageRef image(new Image(width, height, PixelFormat::RGBA));
		void* data = image->GetData();
		memcpy(data, stbi_data, image->GetBytesPerPixel() * width * height);

		stbi_image_free(stbi_data);

		return image;
	}

	//////////////////////////////////////////////////////////////////////////
	ImageConverter::ImageConverter(const Image* srcImage)
		: srcImage_(srcImage)
	{
	}

	ImageRef ImageConverter::Convert(PixelFormat::Type destFormat)
	{
		ASSERT(srcImage_ != nullptr);

		PixelFormat::Type srcFormat = srcImage_->GetFormat();
		int width = srcImage_->GetWidth();
		int height = srcImage_->GetHeight();
		const void* data = srcImage_->GetData();

		// same format
		if (destFormat == srcFormat)
		{
			destImage_ = new Image(width, height, destFormat);
			memcpy(destImage_->GetData(), data, srcImage_->GetImageDataSize());
			return destImage_;
		}

		// need convert
		destImage_ = nullptr;
		switch (srcFormat)
		{
		case PixelFormat::FLOAT4:
			if (destFormat == PixelFormat::RGBA)
				ConvertFLOAT4ToRGBA(data, width, height);
			break;
		case PixelFormat::ARGB:
			if (destFormat == PixelFormat::RGBA)
				ConvertARGBToRGBA(data, width, height);
			break;
		case PixelFormat::RGBA:
			if (destFormat == PixelFormat::FLOAT4)
				ConvertRGBAToFLOAT4(data, width, height);
			break;
		}

		ASSERT(destImage_ != nullptr);
		return destImage_;
	}

	void ImageConverter::ConvertFLOAT4ToRGBA(const void* data, int width, int height)
	{
		destImage_ = new Image(width, height, PixelFormat::RGBA);

		const float* srcData = (const float*)data;
		unsigned char* destData = (unsigned char*)destImage_->GetData();
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				destData[0] = unsigned char(srcData[0] * 255.0f);
				destData[1] = unsigned char(srcData[1] * 255.0f);
				destData[2] = unsigned char(srcData[2] * 255.0f);
				destData[3] = unsigned char(srcData[3] * 255.0f);
				destData += 4;
				srcData += 4;
			}
		}
	}

	void ImageConverter::ConvertARGBToRGBA(const void* data, int width, int height)
	{
		destImage_ = new Image(width, height, PixelFormat::RGBA);

		const unsigned char* srcData = (const unsigned char*)data;
		unsigned char* destData = (unsigned char*)destImage_->GetData();
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				destData[0] = srcData[2];
				destData[1] = srcData[1];
				destData[2] = srcData[0];
				destData[3] = srcData[3];
				destData += 4;
				srcData += 4;
			}
		}
	}

	void ImageConverter::ConvertRGBAToFLOAT4(const void* data, int width, int height)
	{
		destImage_ = new Image(width, height, PixelFormat::FLOAT4);

		const unsigned char* srcData = (const unsigned char*)data;
		float* destData = (float*)destImage_->GetData();
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				destData[0] = srcData[0] / 255.0f;
				destData[1] = srcData[1] / 255.0f;
				destData[2] = srcData[2] / 255.0f;
				destData[3] = srcData[3] / 255.0f;
				destData += 4;
				srcData += 4;
			}
		}
	}
}