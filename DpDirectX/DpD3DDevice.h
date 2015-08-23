/********************************************************************
created:	2014/09/06
created:	6:9:2014   21:45
filename: 	F:\SkyDrive\3D\DpLib\DpDirectX\DpD3DDevice.h
file path:	F:\SkyDrive\3D\DpLib\DpDirectX
file base:	DpD3DDevice
file ext:	h
author:		mi2think@gmail.com

purpose:	Direct3D device
*********************************************************************/

#ifndef __DP_D3DDEVICE__
#define __DP_D3DDEVICE__

#include "DpGeometry.h"
#include "DpNoCopy.h"
#include "DpColor.h"
#include "DpCore.h"

#include <d3d9.h>
#include <d3dx9core.h>
#include <map>
#include <vector>

using namespace dopixel::core;
using namespace dopixel::math;

namespace dopixel
{
	namespace d3d
	{
		void DebugDXTrace(HRESULT hr, const char* file, int line);
		#define DEBUG_DXTRACE(hr) DebugDXTrace(hr, __FILE__, __LINE__)

		class D3DDevice : public NoCopyable
		{
		public:
			D3DDevice();
			~D3DDevice();

			struct stVertex
			{
				float x, y, z, w;
				DWORD color;
				static const DWORD FVF;
			};

			struct stVertexTex
			{
				float x, y, z, w;
				DWORD color;
				float u, v;
				static const DWORD FVF;
			};

			struct stQuadTex
			{
				stVertexTex vt[4];
				IDirect3DTexture9* tex;
			};

			// Device info
			void PrintDisplayMode(D3DDISPLAYMODE& display_mode);
			void PrintCurDisplayMode() { PrintDisplayMode(displayMode); }
			void PrintAdapterIdentifier();
			void PrintCaps();

			void Init(HWND hwnd, int wndWidth, int wndHeight, bool bWindow = true);

			void Release();

			void BeginScene(Color color);

			void EndScene();

			// Draw line
			void DrawLine(const Point& pt0, const Point& pt1, Color color);

			void DrawLineList(const Point* p, unsigned int num, Color color);

			void DrawLineStrip(const Point* p, unsigned int num, Color color);

			void DrawRect(const RectF& rect, Color color);

			void FillRect(const RectF& rect, Color color);

			void FillRectList(const RectF* rect, unsigned int num, Color color);

			// Draw a bitmap, with a quad
			void DrawQuadTex(const stQuadTex& qt);

			// Flush Quad buffer
			void FlushQuadTex();

			// Draw Text
			void DrawText(const char* text, const Point& pos, Color color);

			// Clip rect
			math::RectF SetClipRect(const math::RectF& rect) { math::RectF rc = m_clipRect; m_clipRect = rect; return rc; }

			math::RectF GetClipRect() const { return m_clipRect; }

			// Get a texture, the size of it is equal with the size of client
			IDirect3DTexture9* GetClientTexture() const { return m_pTexture; }

			// Create texture
			IDirect3DTexture9* CreateTexture(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool = D3DPOOL_DEFAULT, unsigned int usage = 0);

			HRESULT UpdateTexture(IDirect3DTexture9* pSrc, IDirect3DTexture9* pDest);

			IDirect3DDevice9* GetD3DD() const { return m_pD3DD; }

			D3DFORMAT GetDisplayFormat() const { return displayMode.Format; }
		private:
			D3DDISPLAYMODE			displayMode;
			D3DADAPTER_IDENTIFIER9	adapterIdentifer;
			D3DCAPS9 caps;

			IDirect3D9*				m_pD3D;
			IDirect3DDevice9*		m_pD3DD;
			IDirect3DTexture9*		m_pTexture;
			IDirect3DTexture9*		m_pRenderTarget;

			IDirect3DVertexBuffer9*	m_pVB;
			IDirect3DIndexBuffer9*	m_pIB;

			DWORD					m_dwVBOffset;
			DWORD					m_dwIBOffset;

			ID3DXFont*				m_pFont;

			std::map<void*, std::vector<stVertexTex> >	m_mapQuadTex;

			RectF					m_clipRect;
		};

		inline D3DDevice* GetD3DDevice() { static D3DDevice s_ins; return &s_ins; }
	}
}

#endif
