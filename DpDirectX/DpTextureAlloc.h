/********************************************************************
	created:	2014/09/07
	created:	7:9:2014   22:40
	filename: 	F:\SkyDrive\3D\DpLib\DpDirectX\DpTextureAlloc.h
	file path:	F:\SkyDrive\3D\DpLib\DpDirectX
	file base:	DpTextureAlloc
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Texture allocation
*********************************************************************/
#ifndef __DP_TEXTURE_ALLOC__
#define __DP_TEXTURE_ALLOC__

#include <d3d9.h>

#include <string>
#include <map>

namespace dopixel
{
	namespace d3d
	{
		class TextureAlloc : public core::NoCopyable
		{
		public:
			// Create Tex by file name
			IDirect3DTexture9* CreateTexture(const std::string& fileName);

			// Create Tex by file memory
			IDirect3DTexture9* CreateTexture(const std::string& fileName, const void* buffer, DWORD size);

			// Create Tex empty
			IDirect3DTexture9* CreateTexture(const std::string& fileName, unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT, unsigned long usage = 0);

			// Get Tex By file name
			IDirect3DTexture9* GetTextureByFileName(const std::string& fileName);

			// Create a temp Tex
			IDirect3DTexture9* CreateTexture(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool = D3DPOOL_SYSTEMMEM, unsigned long usage = 0);

			void Release();

		private:
			std::map<std::string, IDirect3DTexture9*> m_mapFileTex;
		};

		inline TextureAlloc* GetTextureAlloc() { static TextureAlloc texAlloc; return &texAlloc; }
	}
}

#endif

