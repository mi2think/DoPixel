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
#include <cassert>

namespace DoPixel
{
	namespace Core
	{
		struct Texture
		{
			std::string fileName;
			unsigned char* data;

			void SetFileName(const char* fileName) { this->fileName = fileName; }

			void Load();

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

				ATTR_SHADE_PURE = 0x20,	// constant
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

			Material(int _id = 0) 
			: id(_id)
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

			void SetAttr(int attr) { this->attr = attr; }

			int GetAttr() const { return attr; }

			void SetTextureFileName(const char* fileName)
			{
				assert((this->attr & ATTR_SHADE_TEXTURE));

				if (texture == nullptr)
				{
					texture = new Texture();
					texture->SetFileName(fileName);
				}
			}

			Texture* GetTexture() const { return texture; }
		};

		class MaterialManager
		{
		public:
			static MaterialManager& GetInstance() { static MaterialManager ins; return ins; }

			MaterialManager() : materialId(0){}
			~MaterialManager()
			{
				for (unsigned int i = 0; i < materials.size(); ++i)
				{
					if (materials[i].texture)
					{
						materials[i].texture->Release();
						materials[i].texture = NULL;
					}
				}
			}

			Material& GenMaterial(int id)
			{
				for (const auto& m : materials)
				{
					if (m.id == id)
					{
						assert(false && "id has exists!");
					}
				}
				materials.push_back(Material(id));
				return materials.back();
			}

			Material& GenMaterial()
			{
				materials.push_back(Material(GenMaterialId()));
				return materials.back();
			}

			const Material* GetMaterial(int id) const
			{
				for (const auto& m : materials)
				{
					if (m.id == id)
						return &m;
				}
				return nullptr;
			}

		private:
			int GenMaterialId() const
			{
				int genId = 0;
				for (const auto& m : materials)
				{
					genId = MAX(m.id, genId);
				}
				return ++genId;
			}
			std::vector<Material>  materials;
			unsigned int materialId;
		};
	}
}



#endif