/********************************************************************
	created:	2014/09/07
	created:	7:9:2014   21:24
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpTga.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpTga
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Load tga file
*********************************************************************/

#ifndef __DP_TGA__
#define __DP_TGA__

#include "DpColor.h"
#include "DpVector2.h"

namespace dopixel
{
#pragma pack(1)
	struct TgaHeader
	{
		unsigned char IDLength;
		unsigned char ColormapType;
		unsigned char ImageType;
		unsigned char ColormapSpecification[5];
		unsigned short XOrigin;
		unsigned short YOrigin;
		unsigned short ImageWidth;
		unsigned short ImageHeight;
		unsigned char PixelDepth;
		unsigned char ImageDescriptor;
	};
#pragma pack()

	class Tga
	{
	public:
		Tga() : m_image(nullptr) {}
		Tga(const char* fileName) { Load(fileName); }
		~Tga() { Clear(); }

		void Clear();

		bool Load(const char* fileName);

		Color GetColor(const math::Point& pos) const;

		int GetWidth() const { return m_tgaHeader.ImageWidth; }

		int GetHeight() const { return m_tgaHeader.ImageHeight; }

		const unsigned char* GetImage() const { return m_image; }
	private:
		void LoadImage(unsigned char* fileData);

		void LoadImageBySTBI(unsigned char* fileData, int len);

		TgaHeader	m_tgaHeader;

		// Image data
		union
		{
			unsigned char*	m_image;
			Color*	m_color;
		};
	};
}


#endif