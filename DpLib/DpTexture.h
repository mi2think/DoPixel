/********************************************************************
	created:	2015/11/15
	created:	15:11:2015   20:49
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTexture.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTexture
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Texture
*********************************************************************/
#ifndef __DP_TEXTURE_H__
#define __DP_TEXTURE_H__

#include "DoPixel.h"
#include "DpImage.h"

namespace dopixel
{
	class Texture
	{
	public:
		Texture();
		Texture(int width, int height, PixelFormat::Type format);
		Texture(const string& path, bool mipmaps);
		Texture(const ImageRef& image);
		~Texture();

		const string& GetName() const { return name_; }
		void SetImage(const ImageRef& image);
		void Load(const string& path, bool mipmaps);
		void SaveTGA(const string& path, int level = 0);
		void Clear();

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		PixelFormat::Type GetFormat() const { return format_; }

		int GetMipmapCount() const { return mipmapCount_; }
		bool HasMips() const { return mipmapCount_ > 1; }
		ImageRef GetMipmap(int level) const;

		TextureFilter::Type GetFilterMin() const { return filterMin_; }
		TextureFilter::Type GetFilterMag() const { return filterMag_; }
		void SetFilterMin(TextureFilter::Type filter) { filterMin_ = filter; }
		void SetFilterMag(TextureFilter::Type filter) { filterMag_ = filter; }

		TextureWrap::Type GetWrapS() const { return wrapS_; }
		TextureWrap::Type GetWrapT() const { return wrapT_; }
		void SetWrapS(TextureWrap::Type wrap) { wrapS_ = wrap; }
		void SetWrapT(TextureWrap::Type wrap) { wrapT_ = wrap; }

		void SetUsage(TextureUsage::Type usage) { usage_ = usage; }
		TextureUsage::Type GetUsage() const { return usage_; }
	private:
		void GenMipmaps(ImageRef image);

		// file name if load from file
		string name_;
		int width_;
		int height_;
		PixelFormat::Type format_;
		TextureUsage::Type usage_;
		// mipmap
		int mipmapCount_;
		vector<ImageRef> mipmaps_;
		// filter type
		TextureFilter::Type filterMin_;
		TextureFilter::Type filterMag_;
		// wrap type
		TextureWrap::Type wrapS_;
		TextureWrap::Type wrapT_;
	};

	// texture cache
	class TextureCache
	{
	public:
		static TextureCache& Instance() { static TextureCache cache; return cache; }
		TextureCache();
		~TextureCache();

		void Clear();
		TextureRef GetTexture(const string& path);
	private:
		map<string, TextureRef> textures_;
	};
}

#endif
