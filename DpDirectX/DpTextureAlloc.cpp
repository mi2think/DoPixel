/********************************************************************
	created:	2014/09/07
	created:	7:9:2014   22:46
	filename: 	F:\SkyDrive\3D\DpLib\DpDirectX\DpTextureAlloc.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpDirectX
	file base:	DpTextureAlloc
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Texture allocation
*********************************************************************/

#include "DpTextureAlloc.h"
#include "DpD3DDevice.h"
#include "DpCore.h"

#include <d3dx9.h>
#include <algorithm>

using namespace DoPixel::Core;

namespace DoPixel
{
	namespace D3D
	{
		IDirect3DTexture9* TextureAlloc::CreateTexture(const std::string& fileName)
		{
			auto it = m_mapFileTex.find(fileName);
			if (it == m_mapFileTex.end() || it->second == nullptr)
			{
				IDirect3DTexture9* tex = nullptr;
				HRESULT hr = D3DXCreateTextureFromFile(GetD3DDevice()->GetD3DD(), fileName.c_str(), &tex);
				if (FAILED(hr))
				{
					DEBUG_DXTRACE(hr);
					return nullptr;
				}
				m_mapFileTex[fileName] = tex;
				return tex;
			}

			return nullptr;
		}

		IDirect3DTexture9* TextureAlloc::CreateTexture(const std::string& fileName, const void* buffer, DWORD size)
		{
			auto it = m_mapFileTex.find(fileName);
			if (it == m_mapFileTex.end() || it->second == nullptr)
			{
				IDirect3DTexture9* tex = NULL;
				HRESULT hr = D3DXCreateTextureFromFileInMemory(GetD3DDevice()->GetD3DD(), buffer, size, &tex);
				if (FAILED(hr))
				{
					DEBUG_DXTRACE(hr);
					return nullptr;
				}
				m_mapFileTex[fileName] = tex;
				return tex;
			}

			return it->second;
		}

		IDirect3DTexture9* TextureAlloc::CreateTexture(const std::string& fileName, unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool, unsigned long usage)
		{
			auto it = m_mapFileTex.find(fileName);
			if (it == m_mapFileTex.end() || it->second == nullptr)
			{
				IDirect3DTexture9* tex = GetD3DDevice()->CreateTexture(width, height, format, pool, usage);
				m_mapFileTex[fileName] = tex;
				return tex;
			}

			return it->second;
		}


		IDirect3DTexture9* TextureAlloc::GetTextureByFileName(const std::string& fileName)
		{
			auto it = m_mapFileTex.find(fileName);
			if (it != m_mapFileTex.end())
				return it->second;
			return nullptr;
		}

		IDirect3DTexture9* TextureAlloc::CreateTexture(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool, unsigned long usage)
		{
			return GetD3DDevice()->CreateTexture(width, height, format, pool, usage);
		}

		void TextureAlloc::Release()
		{
			std::for_each(m_mapFileTex.begin(), m_mapFileTex.end(), [](decltype(*m_mapFileTex.begin())& e) { SAFE_RELEASE(e.second); });
			m_mapFileTex.clear();
		}

	}
}