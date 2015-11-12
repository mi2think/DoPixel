/********************************************************************
created:	2014/09/06
created:	6:9:2014   21:54
filename: 	F:\SkyDrive\3D\DpLib\DpDirectX\DpD3DDevice.cpp
file path:	F:\SkyDrive\3D\DpLib\DpDirectX
file base:	DpD3DDevice
file ext:	cpp
author:		mi2think@gmail.com

purpose:	Direct3D device
*********************************************************************/

#include "DpD3DDevice.h"
#include "DpTextureAlloc.h"

#include <DxErr.h>
#include <algorithm>

namespace dopixel
{
	namespace d3d
	{
		const DWORD D3DDevice::stVertex::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
		const DWORD D3DDevice::stVertexTex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
		const DWORD vertexBufferSize = 100 * sizeof(D3DDevice::stVertexTex) * 4;
		const DWORD indexBufferSize = 100 * sizeof(WORD) * 6;

#define DEBUG_DXTRACE(hr)	DebugDXTrace(hr, __FILE__, __LINE__)

		void DebugDXTrace(HRESULT hr, const char* file, int line)
		{
			//const char* error = DXGetErrorDescription(hr);
			DEBUG_TRACE("Dx Error: %s:%d %d\n", file, line, (int)hr);
		}

		D3DDevice::D3DDevice()
			: m_pD3D(nullptr)
			, m_pD3DD(nullptr)
			, m_pTexture(nullptr)
			, m_pRenderTarget(nullptr)
			, m_pVB(nullptr)
			, m_pIB(nullptr)
			, m_dwVBOffset(0)
			, m_dwIBOffset(0)
			, m_pFont(nullptr)
		{

		}

		D3DDevice::~D3DDevice()
		{

		};

		void D3DDevice::PrintDisplayMode(D3DDISPLAYMODE& display_mode)
		{
			std::string fmt;
			switch (display_mode.Format)
			{
			case D3DFMT_R8G8B8:			fmt = "D3DFMT_R8G8B8";			break;
			case D3DFMT_A8R8G8B8:		fmt = "D3DFMT_A8R8G8B8";		break;
			case D3DFMT_X8R8G8B8:		fmt = "D3DFMT_X8R8G8B8";		break;
			case D3DFMT_R5G6B5:			fmt = "D3DFMT_R5G6B5";			break;
			case D3DFMT_X1R5G5B5:		fmt = "D3DFMT_X1R5G5B5";		break;
			case D3DFMT_A1R5G5B5:		fmt = "D3DFMT_A1R5G5B5";		break;
			case D3DFMT_A4R4G4B4:		fmt = "D3DFMT_A4R4G4B4";		break;
			case D3DFMT_R3G3B2:			fmt = "D3DFMT_R3G3B2";			break;
			case D3DFMT_A8:				fmt = "D3DFMT_A8";				break;
			case D3DFMT_A8R3G3B2:		fmt = "D3DFMT_A8R3G3B2";		break;
			case D3DFMT_X4R4G4B4:		fmt = "D3DFMT_X4R4G4B4";		break;
			case D3DFMT_A2B10G10R10:	fmt = "D3DFMT_A2B10G10R10";		break;
			case D3DFMT_A8B8G8R8:		fmt = "D3DFMT_A8B8G8R8";		break;
			case D3DFMT_X8B8G8R8:		fmt = "D3DFMT_X8B8G8R8";		break;
			case D3DFMT_G16R16:			fmt = "D3DFMT_G16R16";			break;
			case D3DFMT_A2R10G10B10:	fmt = "D3DFMT_A2R10G10B10";		break;
			case D3DFMT_A16B16G16R16:	fmt = "D3DFMT_A16B16G16R16";	break;
			}
			DEBUG_TRACE("Width:%d\tHeight:%d\tRefreshRate:%d\tFormat:%s\n", display_mode.Width, display_mode.Height, display_mode.RefreshRate, fmt.c_str());
		}

		void D3DDevice::PrintAdapterIdentifier()
		{
			DEBUG_TRACE("Adapter info:\nDirver:%s\nDescription:%s", adapterIdentifer.Driver, adapterIdentifer.Description);
			DWORD Product = HIWORD(adapterIdentifer.DriverVersion.HighPart);
			DWORD Version = LOWORD(adapterIdentifer.DriverVersion.HighPart);
			DWORD SubVersion = HIWORD(adapterIdentifer.DriverVersion.LowPart);
			DWORD Build = LOWORD(adapterIdentifer.DriverVersion.LowPart);
			DEBUG_TRACE("\t%d.%d.%d.%d\n", Product, Version, SubVersion, Build);
		}

		void D3DDevice::PrintCaps()
		{
			DEBUG_TRACE("dev caps:\n");
			if (caps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)
				DEBUG_TRACE("Device supports stream offsets \n");
		}

		void D3DDevice::Init(HWND hwnd, int wndWidth, int wndHeight, bool bWindow)
		{
			m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if (!m_pD3D)
			{
				DEBUG_TRACE("Create IDirect3D9 object failed!\n");
				return;
			}

			// adapter count
			int adapter_count = m_pD3D->GetAdapterCount();
			DEBUG_TRACE("adapter_count:%d\n", adapter_count);

			// adapter info
			if (FAILED(m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifer)))
			{
				DEBUG_TRACE("Get adapter identifier failed!\n");
				return;
			}
			PrintAdapterIdentifier();

			// current display mode
			if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
			{
				DEBUG_TRACE("Get adapter display mode failed!\n");
				return;
			}
			PrintCurDisplayMode();

			// adapter mode count
			DEBUG_TRACE("Other display mode:\n");
			int adapter_mode_count = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
			for (int i = 0; i < adapter_mode_count; ++i)
			{
				D3DDISPLAYMODE display_mode;
				if (SUCCEEDED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &display_mode)))
				{
					PrintDisplayMode(display_mode);
				}
			}

			// device caps
			if (FAILED(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
			{
				DEBUG_TRACE("Get device caps failed!\n");
				return;
			}
			PrintCaps();

			int vp = 0;
			if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
				vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			else
				vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

			// Create device
			D3DPRESENT_PARAMETERS d3dpp;
			ZeroMemory(&d3dpp, sizeof(d3dpp));
			d3dpp.Windowed = bWindow ? TRUE : FALSE;
			d3dpp.hDeviceWindow = hwnd;
			//d3dpp.BackBufferFormat = displayMode.Format;
			d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
			d3dpp.EnableAutoDepthStencil = TRUE;
			d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
			d3dpp.MultiSampleQuality = 0;
			if (bWindow)
			{
				d3dpp.BackBufferCount = 2;
				d3dpp.BackBufferWidth = 0;
				d3dpp.BackBufferHeight = 0;
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
				d3dpp.FullScreen_RefreshRateInHz = 0;
				d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
			}
			else
			{
				d3dpp.BackBufferCount = 2;
				d3dpp.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
				d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
				d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
			}
			HRESULT hr = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, displayMode.Format, d3dpp.BackBufferFormat, d3dpp.Windowed);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, vp, &d3dpp, &m_pD3DD);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			DEBUG_TRACE("Create device succeed!\n");


			// Create vertex & index buffer
			hr = m_pD3DD->CreateVertexBuffer(vertexBufferSize, D3DUSAGE_WRITEONLY, stVertexTex::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}
			hr = m_pD3DD->CreateIndexBuffer(indexBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}
			DEBUG_TRACE("Create vertex & index buffer succeed!\n");

			hr = m_pD3DD->CreateTexture(wndWidth, wndHeight, 1, D3DUSAGE_RENDERTARGET, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &m_pRenderTarget, NULL);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			hr = m_pD3DD->CreateTexture(wndWidth, wndHeight, 1, D3DUSAGE_DYNAMIC, d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &m_pTexture, NULL);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			// Init font
			D3DXFONT_DESC desc;
			memset(&desc, 0, sizeof(desc));
			desc.Height = 12;
			desc.Width = 0;
			desc.Weight = 400;
			desc.MipLevels = D3DX_DEFAULT;
			desc.Italic = false;
			desc.CharSet = DEFAULT_CHARSET;
			desc.OutputPrecision = 0;
			desc.Quality = 0;
			desc.PitchAndFamily = 0;
			strncpy_s(desc.FaceName, "ËÎÌו", sizeof(desc.FaceName));

			hr = D3DXCreateFontIndirect(m_pD3DD, &desc, &m_pFont);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			m_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			m_pD3DD->SetRenderState(D3DRS_ZENABLE, TRUE);
			m_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_pD3DD->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			m_pD3DD->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			m_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

			m_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}

		void D3DDevice::Release()
		{
			GetTextureAlloc()->Release();

			SAFE_RELEASE(m_pFont);
			SAFE_RELEASE(m_pIB);
			SAFE_RELEASE(m_pVB);
			SAFE_RELEASE(m_pRenderTarget);
			SAFE_RELEASE(m_pTexture);
			SAFE_RELEASE(m_pD3DD);
			SAFE_RELEASE(m_pD3D);
		}

		void D3DDevice::BeginScene(Color color)
		{
			m_pD3DD->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color.value, 1.0f, 0);
			m_pD3DD->BeginScene();
			m_dwIBOffset = 0;
			m_dwVBOffset = 0;
		}

		void D3DDevice::EndScene()
		{
			FlushQuadTex();
			m_pD3DD->EndScene();
			m_pD3DD->Present(NULL, NULL, NULL, NULL);
		}

		void D3DDevice::DrawLine(const Point& pt0, const Point& pt1, Color color)
		{
			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int vbSize = sizeof(stVertex) * 2;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			v[0].x = pt0.x;		v[0].y = pt0.y;		v[0].z = 0.0f;	v[0].w = 1.0f;	v[0].color = color.value;
			v[1].x = pt1.x;		v[1].y = pt1.y;		v[1].z = 0.0f;	v[1].w = 1.0f;	v[1].color = color.value;
			m_pVB->Unlock();

			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawPrimitive(D3DPT_LINELIST, 0, 1);

			m_dwVBOffset += vbSize;
		}

		void D3DDevice::DrawLineList(const Point* p, unsigned int num, Color color)
		{
			if (!p || num < 2)
				return;
			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int lineNum = num / 2;
			int vbSize = sizeof(stVertex) * lineNum * 2;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			for (int i = 0; i < lineNum; ++i)
			{
				v->x = p->x;	v->y = p->y;	v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;
				++p;
				v->x = p->x;	v->y = p->y;	v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;
				++p;
			}
			m_pVB->Unlock();

			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawPrimitive(D3DPT_LINELIST, 0, lineNum);

			m_dwVBOffset += vbSize;
		}

		void D3DDevice::DrawLineStrip(const Point* p, unsigned int num, Color color)
		{
			if (!p || num < 2)
				return;
			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int vbSize = sizeof(stVertex) * num;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			for (unsigned int i = 0; i < num; ++i, ++v, ++p)
			{
				v->x = p->x;
				v->y = p->y;
				v->z = 0.0f;
				v->w = 1.0f;
				v->color = color.value;
			}
			m_pVB->Unlock();

			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawPrimitive(D3DPT_LINESTRIP, 0, num - 1);

			m_dwVBOffset += vbSize;
		}

		void D3DDevice::DrawRect(const RectF& rect, Color color)
		{
			if (rect.IsEmpty())
				return;
			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int vbSize = sizeof(stVertex) * 5;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			v[0].x = rect.left;		v[0].y = rect.top;		v[0].z = 0.0f;	v[0].w = 1.0f;	v[0].color = color.value;
			v[1].x = rect.right;	v[1].y = rect.top;		v[1].z = 0.0f;	v[1].w = 1.0f;	v[1].color = color.value;
			v[2].x = rect.right;	v[2].y = rect.bottom;	v[2].z = 0.0f;	v[2].w = 1.0f;	v[2].color = color.value;
			v[3].x = rect.left;		v[3].y = rect.bottom;	v[3].z = 0.0f;	v[3].w = 1.0f;	v[3].color = color.value;
			v[4].x = rect.left;		v[4].y = rect.top;		v[4].z = 0.0f;	v[4].w = 1.0f;	v[4].color = color.value;
			m_pVB->Unlock();

			m_pD3DD->SetTexture(0, nullptr);
			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);

			m_dwVBOffset += vbSize;
		}

		void D3DDevice::FillRect(const RectF& rect, Color color)
		{
			if (rect.IsEmpty())
				return;
			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int vbSize = sizeof(stVertex) * 4;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			v[0].x = rect.left;		v[0].y = rect.bottom;	v[0].z = 0.0f;	v[0].w = 1.0f;	v[0].color = color.value;
			v[1].x = rect.left;		v[1].y = rect.top;		v[1].z = 0.0f;	v[1].w = 1.0f;	v[1].color = color.value;
			v[2].x = rect.right;	v[2].y = rect.bottom;	v[2].z = 0.0f;	v[2].w = 1.0f;	v[2].color = color.value;
			v[3].x = rect.right;	v[3].y = rect.top;		v[3].z = 0.0f;	v[3].w = 1.0f;	v[3].color = color.value;
			m_pVB->Unlock();

			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

			m_dwVBOffset += vbSize;
		}

		void D3DDevice::FillRectList(const RectF* rect, unsigned int num, Color color)
		{
			assert(rect != NULL);

			HRESULT hr = S_OK;
			stVertex* v = NULL;
			int vbSize = sizeof(stVertex) * 4 * num;
			hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			for (unsigned int i = 0; i < num; ++i)
			{
				v->x = rect->left;	v->y = rect->top;		v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;
				v->x = rect->right;	v->y = rect->top;		v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;
				v->x = rect->right;	v->y = rect->bottom;	v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;
				v->x = rect->left;	v->y = rect->bottom;	v->z = 0.0f;	v->w = 1.0f;	v->color = color.value;
				++v;	++rect;
			}
			m_pVB->Unlock();

			WORD* w = NULL;
			int ibSize = sizeof(WORD) * 6 * num;
			hr = m_pIB->Lock(m_dwIBOffset, ibSize, (void**)&w, D3DLOCK_NOOVERWRITE);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return;
			}

			int ibNum = 6 * num;
			for (int i = 0, j = 0; i < ibNum; i += 6, j += 4)
			{
				w[i] = WORD(j + 3);
				w[i + 1] = WORD(j);
				w[i + 2] = WORD(j + 1);
				w[i + 3] = WORD(j + 3);
				w[i + 4] = WORD(j + 1);
				w[i + 5] = WORD(j + 2);
			}
			m_pIB->Unlock();

			m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
			m_pD3DD->SetIndices(m_pIB);
			m_pD3DD->SetFVF(stVertex::FVF);
			m_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4 * num, m_dwIBOffset / sizeof(WORD), 2 * num);

			m_dwVBOffset += vbSize;
			m_dwIBOffset += ibSize;
		}

		void D3DDevice::DrawQuadTex(const stQuadTex& qt)
		{
			auto it = m_mapQuadTex.find(qt.tex);
			if (it != m_mapQuadTex.end())
			{
				int vSize = it->second.size();
				it->second.resize(vSize + 4);
				memcpy(&it->second[vSize], (void*)&qt.vt, sizeof(qt.vt));
			}
			else
			{
				m_mapQuadTex[qt.tex] = std::vector<stVertexTex>();
				std::vector<stVertexTex>& v = m_mapQuadTex[qt.tex];
				v.resize(4);
				memcpy(&v[0], (void*)&qt.vt, sizeof(qt.vt));
			}

		}

		void D3DDevice::FlushQuadTex()
		{
			std::for_each(m_mapQuadTex.begin(), m_mapQuadTex.end(), [this](decltype(*m_mapQuadTex.begin())& e)
			{
				IDirect3DTexture9* tex = (IDirect3DTexture9*)e.first;

				HRESULT hr = S_OK;
				stVertexTex* vt = NULL;
				int vbNum = e.second.size();
				int vbSize = sizeof(stVertexTex) * vbNum;
				hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&vt, D3DLOCK_NOOVERWRITE);
				if (FAILED(hr))
				{
					DEBUG_DXTRACE(hr);
					return;
				}

				memcpy(vt, (void*)&e.second[0], vbSize);
				m_pVB->Unlock();

				WORD* w = NULL;
				int ibSize = sizeof(WORD) * 6 * vbNum / 4;
				hr = m_pIB->Lock(m_dwIBOffset, ibSize, (void**)&w, D3DLOCK_NOOVERWRITE);
				if (FAILED(hr))
				{
					DEBUG_DXTRACE(hr);
					return;
				}

				int ibNum = 6 * vbNum / 4;
				for (int i = 0, j = 0; i < ibNum; i += 6, j += 4)
				{
					w[i] = WORD(j + 3);
					w[i + 1] = WORD(j);
					w[i + 2] = WORD(j + 1);
					w[i + 3] = WORD(j + 3);
					w[i + 4] = WORD(j + 1);
					w[i + 5] = WORD(j + 2);
				}
				m_pIB->Unlock();

				m_pD3DD->SetTexture(0, tex);

				m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertexTex));
				m_pD3DD->SetIndices(m_pIB);
				m_pD3DD->SetFVF(stVertexTex::FVF);
				m_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vbNum, m_dwIBOffset / sizeof(WORD), vbNum / 2);

				m_dwVBOffset += vbSize;
				m_dwIBOffset += ibSize;
			});

			m_mapQuadTex.clear();
		}

		void D3DDevice::DrawText(const char* text, const Point& pos, Color color)
		{
			RECT rect;
			rect.left = int(pos.x);
			rect.top = int(pos.y);
			rect.right = int(pos.x + 500);
			rect.bottom = int(pos.y + 30);

			DWORD f = DT_LEFT | DT_TOP;

			m_pFont->DrawText(NULL, text, -1, &rect, f, color.value);
		}

		IDirect3DTexture9* D3DDevice::CreateTexture(unsigned int width, unsigned int height, D3DFORMAT format, D3DPOOL pool /* = D3DPOOL_DEFAULT */, unsigned int usage /* = 0 */)
		{
			IDirect3DTexture9* tex = NULL;
			HRESULT hr = m_pD3DD->CreateTexture(width, height, 1, usage, format, pool, &tex, NULL);
			if (FAILED(hr))
			{
				DEBUG_DXTRACE(hr);
				return NULL;
			}

			return tex;
		}

		HRESULT D3DDevice::UpdateTexture(IDirect3DTexture9* pSrc, IDirect3DTexture9* pDest)
		{
			return m_pD3DD->UpdateTexture(pSrc, pDest);
		}
	}
}
