#include "DpSprite.h"
#include "DpD3DDevice.h"
#include "DpTextureAlloc.h"
#include "DpTga.h"
#include "DpCore.h"

#include <d3dx9.h>

namespace DoPixel
{
	namespace D3D
	{
		static const float s_half = 0.5f;

		Sprite::Sprite()
			: m_width(0)
			, m_height(0)
			, m_texture(NULL)
		{
		}

		Sprite::~Sprite()
		{

		}

		void Sprite::Load(const std::string& fileName)
		{
			const char* ext = strrchr(fileName.c_str(), '.');
			if (ext != NULL)
				++ext;

			assert(_stricmp(ext, "tga") == 0);

			Core::Tga tga(fileName);
			m_width = tga.GetWidth();
			m_height = tga.GetHeight();

			if (fileName == m_fileName)
				return;

			m_texture = GetTextureAlloc()->CreateTexture(fileName, m_width, m_height, D3DFMT_A8R8G8B8);

			assert(m_texture != nullptr);

			UpdateTexture(tga.GetImage());
		}

		void Sprite::UpdateTexture(const unsigned char* imageData)
		{
			// Get tex surface desc
			D3DSURFACE_DESC desc;
			HRESULT hr = m_texture->GetLevelDesc(0, &desc);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			// Get a temp system texture
			IDirect3DTexture9* tempTex = GetTextureAlloc()->CreateTexture(desc.Width, desc.Height, desc.Format);
			if (!tempTex)
				return;

			ON_SCOPE_EXIT([&tempTex]{ SAFE_RELEASE(tempTex); });

			// Lock temp system texture
			D3DLOCKED_RECT lockRect;
			hr = tempTex->LockRect(0, &lockRect, NULL, 0);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			const unsigned char* p = imageData;

			unsigned char* pBytes = (unsigned char*)lockRect.pBits;
			unsigned int bytes = m_width * sizeof(Color);
			for (unsigned int i = 0; i < m_height; ++i)
			{
				memcpy(pBytes, p, bytes);
				pBytes += lockRect.Pitch;
				p += bytes;
			}
	
			hr = tempTex->UnlockRect(0);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			hr = GetD3DDevice()->UpdateTexture(tempTex, m_texture);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}
		}

		void Sprite::Render(const Math::Point& pt)
		{
			if (m_texture == NULL)
				return;

			Math::Point pos = m_pos;
			pos += pt;
			RectF spriteRect(pos.x, pos.y, pos.x + m_width, pos.y + m_height);

			RectF clipRect = GetD3DDevice()->GetClipRect();
			RectF rc = clipRect & spriteRect;
			if (rc.IsEmpty())
				return;

			D3DDevice::stQuadTex qt;
			qt.vt[0].x = rc.left - s_half;			qt.vt[0].y = rc.top - s_half;			qt.vt[0].z = 0.0f;		qt.vt[0].w = 1.0f;
			qt.vt[0].u = (rc.left - spriteRect.left) / spriteRect.Width();
			qt.vt[0].v = (rc.top - spriteRect.top) / spriteRect.Height();

			qt.vt[1].x = rc.right - s_half;			qt.vt[1].y = rc.top - s_half;			qt.vt[1].z = 0.0f;		qt.vt[1].w = 1.0f;
			qt.vt[1].u = (rc.right - spriteRect.left) / spriteRect.Width();
			qt.vt[1].v = (rc.top - spriteRect.top) / spriteRect.Height();

			qt.vt[2].x = rc.right - s_half;			qt.vt[2].y = rc.bottom - s_half;		qt.vt[2].z = 0.0f;		qt.vt[2].w = 1.0f;
			qt.vt[2].u = (rc.right - spriteRect.left) / spriteRect.Width();
			qt.vt[2].v = (rc.bottom - spriteRect.top) / spriteRect.Height();

			qt.vt[3].x = rc.left - s_half;			qt.vt[3].y = rc.bottom - s_half;		qt.vt[3].z = 0.0f;		qt.vt[3].w = 1.0f;
			qt.vt[3].u = (rc.left - spriteRect.left) / spriteRect.Width();
			qt.vt[3].v = (rc.bottom - spriteRect.top) / spriteRect.Height();

			qt.tex = m_texture;

			GetD3DDevice()->DrawQuadTex(qt);
		}
	}
}