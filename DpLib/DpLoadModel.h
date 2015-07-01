/********************************************************************
created:	2014/12/08
created:	8:12:2014   22:59
filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpLoadModel.cpp
file path:	F:\SkyDrive\3D\DpLib\DpLib
file base:	DpLoadModel
file ext:	cpp
author:		mi2think@gmail.com

purpose:	Load 3D model file
*********************************************************************/

#ifndef __DP_LOAD_MODEL__
#define __DP_LOAD_MODEL__

#include "DpObject.h"
#include "DpVector4.h"

namespace DoPixel
{
	namespace Core
	{
		//////////////////////////////////////////////////////////////////////////
		// .PLG file

		#define PLX_RGB_MASK		0x8000
		#define PLX_SHADE_MODE_MASK	0x6000
		#define PLX_2SIDE_MASK		0x1000
		#define PLX_COLOR_MASK		0x0fff

		#define PLX_COLOR_MODE_RGB_FLAG			0x8000
		#define PLX_COLOR_MODE_INDEXED_FLAG		0x0000

		#define PLX_2SIDE_FLAG					0x1000
		#define PLX_1SIDE_FLAG					0x0000

		#define PLX_SHADE_MODE_PURE_FLAG		0x0000	//alias: EMISSIVE, CONSTANT
		#define PLX_SHADE_MODE_FLAT_FLAG		0x2000
		#define PLX_SHADE_MODE_GUARD_FLAG		0x4000
		#define PLX_SHADE_MODE_PHONG_FLAG		0x6000

		// Load Object from .PLG file
		// Success return true, else false
		// scale: initial scale
		// pos: initial pos
		bool LoadObjectFromPLG(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos);

		//////////////////////////////////////////////////////////////////////////
		// Vertex flags

		#define VERTEX_FLAGS_INVERT_X				0x1
		#define VERTEX_FLAGS_INVERT_Y				0x2
		#define VERTEX_FLAGS_INVERT_Z				0x4
		#define VERTEX_FLAGS_SWAP_YZ				0x8
		#define VERTEX_FLAGS_SWAP_XZ				0x10
		#define VERTEX_FLAGS_SWAP_XY				0x20
		#define VERTEX_FLAGS_INVERT_WINDING_ORDER	0x40

		#define VERTEX_FLAGS_TRANSFORM_LOCAL         512  // if file format has local transform then do it!
		#define VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD  1024  // if file format has local to world then do it!

		#define VERTEX_FLAGS_OVERRIDE_MASK          0xf000 // this masks these bits to extract them
		#define VERTEX_FLAGS_OVERRIDE_PURE          0x1000
		#define VERTEX_FLAGS_OVERRIDE_FLAT          0x2000
		#define VERTEX_FLAGS_OVERRIDE_GOURAUD       0x4000
		#define VERTEX_FLAGS_OVERRIDE_TEXTURE       0x8000

		//////////////////////////////////////////////////////////////////////////
		// .ASC file

		// Load object from .ASC file
		// Success return true, else false
		// scale: initial scale
		// pos: initial pos
		// rot: initial rotate
		// vertexFlag: vertex flag
		bool LoadObjectFrom3DSASC(Object& obj, const char* fileName, const Vector4f& scale, const Vector4f& pos, const Vector4f& rot, int vertexFlag);

		//////////////////////////////////////////////////////////////////////////
		// .COB file

		// Load object from .COB file
		// Success return true, else false
		// scale: initial scale
		// pos: initial pos
		// rot: initial rotate
		// vertexFlag: vertex flag
		bool LoadObjectFromCOB(Object& object, const char* fileName, const Vector4f& scale, const Vector4f& pos, const Vector4f& rot, int vertexFlag);
	}
}

#endif