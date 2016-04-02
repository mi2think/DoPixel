/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   21:48
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpImage.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpImage
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Image
*********************************************************************/
#ifndef __DP_IMAGE_H__
#define __DP_IMAGE_H__

#include "DoPixel.h"

namespace dopixel
{
	class Image : public NoCopyable
	{
	public:
		Image();
		Image(int width, int height, PixelFormat::Type format);
		Image(const void* data, int length, PixelFormat::Type format);
		~Image();

		bool Valid() const { return data_ != nullptr; }
		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		PixelFormat::Type GetFormat() const { return format_; }

		int GetBytesPerPixel() const { return bytesPerPixel_; }
		int GetImageDataSize() const { return dataSize_; }

		void* GetData() { return data_; }
		const void* GetData() const { return data_; }

		void SaveTGA(const string& path);
		static int GetBytesPerPixel(PixelFormat::Type format);
		static ImageRef FromFile(const string& path);
	private:
		int width_;
		int height_;
		PixelFormat::Type format_;
		int bytesPerPixel_;
		int dataSize_;
		void* data_;
	};
}

#endif
