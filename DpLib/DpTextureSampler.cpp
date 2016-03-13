/********************************************************************
	created:	2016/02/17
	created:	17:2:2016   20:29
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTextureSampler.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTextureSampler
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Texture Sampler
*********************************************************************/
#include "DpTextureSampler.h"
#include "DpTexture.h"

namespace dopixel
{
	struct WrapFunc_Reapeat
	{
		bool operator()(int& a, int r) const
		{
			if (a < 0)
			{
				// e.g.  
				// a = -1, r = 10, then a % r = -1, so a % r + r - 1 = 9
				a = a % r + r - 1;
			}
			else
			{
				if (a > r)
					a %= r;
			}
			return true;
		}
	};

	struct WrapFunc_Mirror
	{
		bool operator()(int& a, int r) const
		{
			if (a < 0)
			{
				int b = a % r;
				int c = a / r;
				// odd
				if (c & 0x1)
					a = -b;
				else
					a = r + b;
			}
			else
			{
				if (a > r)
				{
					int b = a % r;
					int c = a / r;
					// odd
					if (c & 0x1)
						a = r - b;
					else
						a = b;
				}
			}
			return true;
		}
	};

	struct WrapFunc_ClampToEdge
	{
		bool operator()(int& a, int r) const
		{
			if (a < 0)
				a = 0;
			if (a > r)
				a = r - 1;
			return true;
		}
	};

	struct WrapFunc_ClampToBorder
	{
		bool operator()(int& a, int r) const
		{
			if (a < 0 || a > r)
				return false;

			return true;
		}
	};

	std::function<bool(int&, int)> GetWrapFuncByType(TextureWrap::Type wrapType)
	{
		switch (wrapType)
		{
		case TextureWrap::Reapeat:
			return WrapFunc_Reapeat();
			break;
		case TextureWrap::Mirror:
			return WrapFunc_Mirror();
			break;
		case TextureWrap::ClampToEdge:
			return WrapFunc_ClampToEdge();
			break;
		case TextureWrap::ClampToBorder:
			return WrapFunc_ClampToBorder();
			break;
		default:
			ASSERT(false && "Invalid Wrap Type!");
			break;
		}
		return WrapFunc_Reapeat();
	}

	//////////////////////////////////////////////////////////////////////////

	class TextureSampler::ITextureFilter
	{
	public:
		virtual ~ITextureFilter() = 0 {};
		virtual math::Vector3f Sample(const math::Vector2f& uv) const = 0;
	};

	class NearestPointFilter : public TextureSampler::ITextureFilter
	{
	public:
		NearestPointFilter(const ImageRef& image, const math::Vector3f& borderColor,TextureWrap::Type wrapS, TextureWrap::Type wrapT)
			: image_(image)
			, borderColor_(borderColor)
			, width_(image->GetWidth())
			, height_(image_->GetHeight())
			, data_((const float*)image->GetData())
			, wrapSFunc_(GetWrapFuncByType(wrapS))
			, wrapTFunc_(GetWrapFuncByType(wrapT))
		{
			ASSERT(image->GetFormat() == ImageFormat::FLOAT4);
			ASSERT(data_ != nullptr);
		}

		virtual math::Vector3f Sample(const math::Vector2f& uv) const override
		{
			int u = (int)uv.x;
			int v = (int)uv.y;
			bool s_border = ! wrapSFunc_(u, width_);
			bool t_border = ! wrapTFunc_(v, height_);
			if (s_border || t_border)
			{
				return borderColor_;
			}
			else
			{
				ASSERT(u >= 0 && u < width_);
				ASSERT(v >= 0 && v < height_);
				const float* p = data_ + (v * width_ + u) * 4;
				++p; // TODO: skip alpha temporary
				return math::Vector3f(p[0], p[1], p[2]);
			}
		}
	private:
		const ImageRef image_;
		const float* data_;
		math::Vector3f borderColor_;
		int width_;
		int height_;
		std::function<bool(int&, int)> wrapSFunc_;
		std::function<bool(int&, int)> wrapTFunc_;
	};

	//////////////////////////////////////////////////////////////////////////

	TextureSampler::TextureSampler()
		: filterMin_(TextureFilter::NearestPoint)
		, filterMag_(TextureFilter::NearestPoint)
		, wrapS_(TextureWrap::Reapeat)
		, wrapT_(TextureWrap::Reapeat)
		, borderColor_(0.0f, 0.0f, 0.0f)
		, textureFilter_(nullptr)
		, buf_(new char[128], &default_array_destory<char>)
	{
	}

	TextureSampler::~TextureSampler()
	{
	}

	void TextureSampler::SetTexture(const TextureRef& texture)
	{
		texture_ = texture;
		if (texture_)
		{
			SetWrapS(texture_->GetWrapS());
			SetWrapT(texture_->GetWrapT());
			SetFilterMin(texture_->GetFilterMin());
			SetFilterMag(texture_->GetFilterMag());
		}
	}

	math::Vector2f TextureSampler::GetTextureSize() const
	{
		return math::Vector2f(texture_->GetWidth(), texture_->GetHeight());
	}

	void TextureSampler::BeginSample(
		const math::Vector2f& p0, 
		const math::Vector2f& uv0, 
		const math::Vector2f& p1, 
		const math::Vector2f& uv1, 
		const math::Vector2f& p2, 
		const math::Vector2f& uv2)
	{
		ASSERT(texture_ != nullptr);

		// Note: simple process on choose minifying & magnification filter:
		// if pixel area > texel area: choose magnification
		// else: choose minifying
		math::Vector2f v0 = p1 - p0;
		math::Vector2f v1 = p2 - p0;
		float pixelArea = math::Abs(v0.x * v1.y - v0.y * v1.x);

		v0 = uv1 - uv0;
		v1 = uv2 - uv0;
		float texelArea = math::Abs(v0.x * v1.y - v0.y * v1.x);

		ImageRef image = texture_->GetMipmap(0);
		TextureFilter::Type filterType = (pixelArea > texelArea ? filterMag_ : filterMin_);
		switch (filterType)
		{
		case TextureFilter::NearestPoint:
			textureFilter_ = new(buf_.Get()) NearestPointFilter(image, borderColor_, wrapS_, wrapT_);
			break;
		default:
			ASSERT(false && "Not Supported Filter Type!");
			break;
		}
		ASSERT(textureFilter_ != nullptr);
	}

	math::Vector3f TextureSampler::Sample(const math::Vector2f& uv) const
	{
		return textureFilter_->Sample(uv);
	}

	void TextureSampler::EndSample()
	{
		textureFilter_->~ITextureFilter();
		textureFilter_ = nullptr;
	}
}