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
		Image(int width, int height, ImageFormat::Type format);
		Image(const void* data, int length, ImageFormat::Type format);
		~Image();

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		ImageFormat::Type GetFormat() const { return format_; }

		int GetBytesPerPixel() const { return bytesPerPixel_; }
		int GetImageDataSize() const { return dataSize_; }

		void* GetData() { return data_; }
		const void* GetData() const { return data_; }

		static int GetBytesPerPixel(ImageFormat::Type format)
		static ImageRef FromFile(const string& path);
	private:
		int width_;
		int height_;
		ImageFormat::Type format_;
		int bytesPerPixel_;
		int dataSize_;
		void* data_;
	};
}

#endif
