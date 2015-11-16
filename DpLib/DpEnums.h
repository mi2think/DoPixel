/********************************************************************
	created:	2015/09/14
	created:	14:9:2015   0:33
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpEnums.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpEnums
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Enums def
*********************************************************************/
#ifndef __DP_ENUMS__
#define __DP_ENUMS__

namespace dopixel
{
	enum FillMode
	{
		Fill_Wireframe,
		Fill_Solid,
	};

	enum CMPFunc
	{
		CMP_Never,
		CMP_Less,
		CMP_Equal,
		CMP_LessEqual,
		CMP_Greater,
		CMP_NotEqual,
		CMP_GreaterEqual,
		CMP_Always,
	};

	enum Boolen
	{
		False,
		True,
	};

	enum ZEnable
	{
		ZEnable_False,
		ZEnable_Z,
		ZEnable_INVZ,
	};

	enum RenderState
	{
		RS_FillMode,
		RS_ShadeMode,
		RS_ZEnable,
		RS_ZFUNC,
		RS_ZWriteEnable,
	};
}

#endif