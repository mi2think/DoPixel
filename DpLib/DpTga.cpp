/********************************************************************
	created:	2015/07/24
	created:	24:7:2015   0:34
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpTga.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpTga
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Load tga file
*********************************************************************/

#include "DpTga.h"
#include "DpCore.h"
#include "DpFileStream.h"
#include "stb/stb_image.h"

namespace dopixel
{
	namespace core
	{
		void Tga::Clear()
		{
			SAFE_DELETEARRAY(m_image);
		}

		bool Tga::Load(const char* fileName)
		{
			Clear();

			FileStream fs(fileName, FileStream::BinaryRead);
			size_t size = (size_t)fs.Size();
			unsigned char* fileData = new unsigned char[size];
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

				//LoadImage(fileData);
				LoadImageBySTBI(fileData, size);
			}
			return true;
		}

		void Tga::LoadImage(unsigned char* fileData)
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

			unsigned char* pData = fileData + sizeof(m_tgaHeader);
			pData += m_tgaHeader.IDLength;

			unsigned char bitsPerPixel = m_tgaHeader.PixelDepth;
			unsigned char bytesPerPixel = bitsPerPixel / 8;

			//Image Data:
			//This field contains (Width)x(Height) pixels. Each pixel specifies image data in one of the following
			//formats: a single color-map index for Pseudo-Color; Attribute, Red, Green and Blue ordered data for True-
			// Color; and independent color-map indices for Direct-Color

			// For convenience, use 4 bytes
			m_image = new unsigned char[m_tgaHeader.ImageWidth * m_tgaHeader.ImageHeight * 4];

			unsigned char* p = m_image;
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
						unsigned char packetType = (*pData) & 0x80;
						unsigned char pixelCount = ((*pData) & 0x7f) + 1;

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

		void Tga::LoadImageBySTBI(unsigned char* fileData, int len)
		{
			int width = 0;
			int height = 0;
			int comp = 0;
			unsigned char* image = stbi_load_from_memory(fileData, len, &width, &height, &comp, 4);
			
			int size = width * height * 4;
			m_image = new unsigned char[size];
			memcpy(m_image, image, size);
			// Because stbi load it by [r g b a], we need convert it. :(
			// r g b a -> a r g b
			unsigned char* end = m_image + size;
			for (unsigned char* p = m_image; p < end; p += 4)
			{
				unsigned char r = *p;
				*p = *(p + 2);
				*(p + 2) = r;
			}

			free(image);
		}

		Color Tga::GetColor(const math::Point& pos) const
		{
			assert(pos.x < m_tgaHeader.ImageWidth && pos.y < m_tgaHeader.ImageHeight);
			assert(m_color != NULL);

			return *(m_color + (int)pos.y * m_tgaHeader.ImageWidth + (int)pos.x);
		}
	}
}
