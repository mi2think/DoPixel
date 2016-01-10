/********************************************************************
	created:	2016/01/07
	created:	7:1:2016   22:31
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpDXUtils.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpDXUtils
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	DX Utils
*********************************************************************/
#ifndef __DP_DX_UTILS_H__
#define __DP_DX_UTILS_H__

#include "DoPixel.h"

#include <windows.h>
#include <d3dx9.h>
#include <dxerr.h>

namespace dopixel
{
	// error checker
	class DXErrorChecker
	{
	public:
		DXErrorChecker(const char* file, int line);
		~DXErrorChecker();

		DXErrorChecker& operator=(HRESULT hr);
	private:
		const char* file_;
		int line_;
		HRESULT hr_;
	};

	#define CHECK_HR DXErrorChecker(__FILE__, __LINE__)

	extern IDirect3D9* g_pD3D;
	extern IDirect3DDevice9* g_pD3DD;

	// utils

	// setup
	IDirect3D9* SetupD3D();
	IDirect3DDevice9* SetupD3DDevice(HWND hwnd, bool wndmode);
	// format
	const char* D3DFormat2String(D3DFORMAT format);
	// default display mode
	void ShowDisplayMode();
	void ShowAdapterCount();
	// default identifier
	void ShowAdapterIdentifier();
}

#endif