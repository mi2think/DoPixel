/********************************************************************
	created:	2016/02/17
	created:	17:2:2016   20:28
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTextureSampler.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTextureSampler
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Texture Sampler
*********************************************************************/
#ifndef __TEXTURE_SAMPLER_H__
#define __TEXTURE_SAMPLER_H__

#include "DoPixel.h"
#include "DpVector2.h"
#include "DpVector3.h"

namespace dopixel
{
	class TextureSampler
	{
	public:
		TextureSampler();
		~TextureSampler();

		void SetTexture(const TextureRef& texture);
		
		void SetFilterMin(TextureFilter::Type filter) { filterMin_ = filter; }
		void SetFilterMag(TextureFilter::Type filter) { filterMag_ = filter; }

		void SetWrapS(TextureWrap::Type wrap) { wrapS_ = wrap; }
		void SetWrapT(TextureWrap::Type wrap) { wrapT_ = wrap; }

		void SetBorderColor(const math::Vector3f& borderColor) { borderColor_ = borderColor; }
		math::Vector2f GetTextureSize() const;

		void BeginSample(
			const math::Vector2f& p0, 
			const math::Vector2f& uv0,
			const math::Vector2f& p1,
			const math::Vector2f& uv1,
			const math::Vector2f& p2,
			const math::Vector2f& uv2);
		math::Vector3f Sample(const math::Vector2f& uv) const;
		void EndSample();

		class ITextureFilter;
	private:
		TextureRef texture_;
		// filter type
		TextureFilter::Type filterMin_;
		TextureFilter::Type filterMag_;
		// wrap type
		TextureWrap::Type wrapS_;
		TextureWrap::Type wrapT_;
		// texture border color
		math::Vector3f borderColor_;

		Ref<ITextureFilter> textureFilter_;
		Ref<char> buf_;
	};
}

#endif