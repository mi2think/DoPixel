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

#include "DpCore.h"
#include "DpColor.h"
#include "DpVector2.h"
#include "DpFileStream.h"

namespace DoPixel
{
	namespace Core
	{
#pragma pack(1)
		struct TgaHeader
		{
			BYTE IDLength;
			BYTE ColormapType;
			BYTE ImageType;
			BYTE ColormapSpecification[5];
			WORD XOrigin;
			WORD YOrigin;
			WORD ImageWidth;
			WORD ImageHeight;
			BYTE PixelDepth;
			BYTE ImageDescriptor;
		};
#pragma pack()

		class Tga
		{
		public:
			Tga() : m_image(nullptr) {}
			Tga(const std::string& fileName) { Load(fileName); }
			~Tga() { Clear(); }

			inline void Clear();

			inline bool Load(const std::string& fileName);

			inline Color GetColor(const Math::Point& pos) const;

			unsigned int GetWidth() const { return m_tgaHeader.ImageWidth; }

			unsigned int GetHeight() const { return m_tgaHeader.ImageHeight; }

			const BYTE* GetImage() const { return m_image; }
		private:
			inline void LoadImage(BYTE* fileData);

			TgaHeader	m_tgaHeader;

			// Image data
			union
			{
				BYTE*	m_image;
				Color*	m_color;
			};
		};

		inline void Tga::Clear()
		{
			SAFE_DELETEARRAY(m_image);
		}

		inline bool Tga::Load(const std::string& fileName)
		{
			Clear();

			FileStream fs(fileName.c_str(), FileStream::BinaryRead);
			size_t size = (size_t)fs.Size();
			BYTE* fileData = new BYTE[size];
			ON_SCOPE_EXIT([&fileData](){ SAFE_DELETEARRAY(fileData); });

			if (fileData)
			{
				size_t readSize = fs.Read(fileData, size);
				if (readSize != size)
				{
					DEBUG_TRACE("Load %s error!\n", fileName);
					return false;
				}

				memcpy(&m_tgaHeader, fileData, sizeof(m_tgaHeader));

				LoadImage(fileData);
			}
			return true;
		}

		inline void Tga::LoadImage(BYTE* fileData)
		{
			/*
			Tga file format:

			-------------------------
			Tga Header
			-------------------------
			Image ID
			-------------------------
			Color Map Data
			-------------------------
			Image Data
			-------------------------

			ImageType:
			0  -  文件中没有图像数据
			1  -  未压缩的，颜色表图像
			2  -  未压缩的，rgb 图像
			3  -  未压缩的，黑白图像
			9  -  runlength 编码的颜色表图像
			10 -  runlength 编码的 rgb 图像
			11 -  压缩的，黑白图像
			32 -  使用 huffman,delta 和 runlength 编码的颜色表图像
			33 -  使用 huffman,delta 和 runlength 编码的颜色映射图像，4 趟四叉树类型处理。
			*/

			if (m_tgaHeader.ImageType != 2 && m_tgaHeader.ImageType != 10)
				return;

			BYTE* pData = fileData + sizeof(m_tgaHeader);
			pData += m_tgaHeader.IDLength;

			BYTE bitsPerPixel = m_tgaHeader.PixelDepth;
			BYTE bytesPerPixel = bitsPerPixel / 8;

			//Image Data:
			//This field contains (Width)x(Height) pixels. Each pixel specifies image data in one of the following
			//formats: a single color-map index for Pseudo-Color; Attribute, Red, Green and Blue ordered data for True-
			// Color; and independent color-map indices for Direct-Color

			// For convenience, use 4 bytes
			m_image = new BYTE[m_tgaHeader.ImageWidth * m_tgaHeader.ImageHeight * 4];

			BYTE* p = m_image;
			bool topLeft = (m_tgaHeader.ImageDescriptor & 0xf0) == 0x20;

			for (int i = 0; i < m_tgaHeader.ImageHeight; ++i)
			{
				if (!topLeft)
				{
					// bottom left
					p = m_image + (m_tgaHeader.ImageHeight - i - 1) * m_tgaHeader.ImageWidth * 4;
				}

				for (int j = 0; j < m_tgaHeader.ImageWidth;)
				{
					if (m_tgaHeader.ImageType == 2)
					{
						switch (bytesPerPixel)
						{
						case 4:
							memcpy(p, pData, 4);
							break;
						case 3:
							memcpy(p, pData, 3);
							*(p + 3) = 0xff;
							break;
						case 2:
							break;
						}

						pData += bytesPerPixel;
						p += 4;
						++j;
					}
					else if (m_tgaHeader.ImageType == 10)
					{
						BYTE packetType = (*pData) & 0x80;
						BYTE pixelCount = ((*pData) & 0x7f) + 1;

						++pData;
						j += pixelCount;
						if (packetType == 0)
						{
							while (pixelCount != 0)
							{
								--pixelCount;
								switch (bytesPerPixel)
								{
								case 4:
									memcpy(p, pData, 4);
									break;
								case 3:
									memcpy(p, pData, 3);
									*(p + 3) = 0xff;
									break;
								case 2:
									break;
								}
								p += 4;
								pData += bytesPerPixel;
							}
						}
						else
						{
							while (pixelCount != 0)
							{
								--pixelCount;
								switch (bytesPerPixel)
								{
								case 4:
									memcpy(p, pData, 4);
									break;
								case 3:
									memcpy(p, pData, 3);
									*(p + 3) = 0xff;
									break;
								case 2:
									break;
								}
								p += 4;
							}
							pData += bytesPerPixel;
						}
					}
				}
			}
		}

		inline Color Tga::GetColor(const Math::Point& pos) const
		{
			assert(pos.x < m_tgaHeader.ImageWidth && pos.y < m_tgaHeader.ImageHeight);
			assert(m_color != NULL);

			return *(m_color + (int)pos.y * m_tgaHeader.ImageWidth + (int)pos.x);
		}
	}
}


#endif