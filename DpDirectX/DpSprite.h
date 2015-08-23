/********************************************************************
	created:	2014/09/07
	created:	7:9:2014   19:47
	filename: 	F:\SkyDrive\3D\DpLib\DpDirectX\DpSprite.h
	file path:	F:\SkyDrive\3D\DpLib\DpDirectX
	file base:	DpSprite
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	sprite
*********************************************************************/

#ifndef __DP_SPRITE__
#define __DP_SPRITE__

#include "DpVector2.h"

#include <d3d9.h>
#include <string>

namespace dopixel
{
	namespace d3d
	{
		class Sprite
		{
		public:
			Sprite();
			virtual ~Sprite();

			void Load(const std::string& fileName);

			void SetPos(const math::Point& pt) { m_pos = pt; }

			math::Point GetPos() const { return m_pos; }

			void Render(const math::Point& pt);

			unsigned int GetWidth() const { return m_width; }

			unsigned int GetHeight() const { return m_height; }
		protected:
			void UpdateTexture(const unsigned char* imageData);

			std::string		m_fileName;
			unsigned int	m_width;
			unsigned int	m_height;

			IDirect3DTexture9*	m_texture;

			math::Point		m_pos;
		};
	}
}



#endif
