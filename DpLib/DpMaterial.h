/********************************************************************
	created:	2014/08/06
	created:	6:8:2014   21:10
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMaterial.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMaterial
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Def material
*********************************************************************/

#ifndef __DP_MATERIAL__
#define __DP_MATERIAL__

#include "DpColor.h"

namespace DoPixel
{
	namespace Core
	{
		struct Texture
		{
			char file[80];
			unsigned char* data;

			void Release() {}
		};


		struct Material
		{
			enum 
			{
				ATTR_2SIDE = 0x1,
				ATTR_TRANSPARENT = 0x2,
				ATTR_8BITCOLOR = 0x4,
				ATTR_RGB16 = 0x8,
				ATTR_RGB24 = 0x10,

				ATTR_SHADE_PURE = 0x20,
				ATTR_SHADE_FLAT = 0x40,
				ATTR_SHADE_GOURAUD = 0x80,
				ATTR_SHADE_PHONG = 0x100,
				ATTR_SHADE_TEXTURE = 0x200,

				STATE_ACTIVE = 0x1,
			};

			char name[64];
			int id;
			int state;
			int attr;
			Color color;

			float kambient;
			float kdiffuse;
			float kspecular;
			float power;

			Color rambient;
			Color rdiffuse;
			Color rspecular;

			Texture* texture;

			Material() 
			: id(0)
			, state(0)
			, attr(0)
			, kambient(0)
			, kdiffuse(0)
			, kspecular(0)
			, power(0)
			, texture(NULL)
			{
				memset(name, 0, sizeof(name));
			}
		};

		class MaterialManager
		{
		public:
			const int MAX_MATERIALS = 128;
			Material materials[MAX_MATERIALS];

			void MaterialManager()
			{
				for (int i = 0; i < MAX_MATERIALS; ++i)
				{
					if (materials[i].texture)
					{
						materials[i].texture->Release();
						materials[i].texture = NULL;
					}
				}
			}
		};
	}
}



#endif