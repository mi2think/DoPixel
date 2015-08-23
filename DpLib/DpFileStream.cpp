/********************************************************************
	created:	2015/08/02
	created:	2:8:2015   20:09
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpFileStream.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpFileStream
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	File Stream
*********************************************************************/
#include "DpFileParser.h"
#include <string>
#include <cassert>

namespace dopixel
{
	namespace core
	{
		FileStream::FileStream()
			: file(nullptr)
		{
		}

		FileStream::FileStream(const char* fileName, Mode _mode)
		{
			Open(fileName, _mode);
		}

		FileStream::~FileStream()
		{
			Close();
		}

		bool FileStream::Open(const char* fileName, Mode _mode)
		{
			mode = _mode;
			const char* fmt = nullptr;
			switch (mode)
			{
			case TextRead:
				fmt = "r";
				break;
			case TextWrite:
				fmt = "w";
				break;
			case TextReadWrite:
				fmt = "r+";
				break;
			case BinaryRead:
				fmt = "rb";
				break;
			case BinaryWrite:
				fmt = "wb";
				break;
			case BinaryReadWrite:
				fmt = "r+b";
				break;
			}
			int ret = fopen_s(&file, fileName, fmt);
			if (ret != 0)
			{
				// try ../Resource/ + fileName
				std::string path("../Resource/");
				path += fileName;

				ret = fopen_s(&file, path.c_str(), fmt);
			}
			assert(ret == 0);
			return ret == 0;
		}

		void FileStream::Close()
		{
			if (file)
			{
				fclose(file);
				file = nullptr;
			}
		}

		fpos_t FileStream::Size() const
		{
			assert(file != nullptr);

			fpos_t pos = Position();
			if (pos != -1)
			{
				fpos_t size = 0;
				if (fseek(file, 0, SEEK_END) == 0)
				{
					size = Position();
					if (fsetpos(file, &pos) == 0)
						return size;
				}
			}
			return -1;
		}

		fpos_t FileStream::Position() const
		{
			assert(file != nullptr);

			fpos_t pos = 0;
			if (fgetpos(file, &pos) == 0)
			{
				return pos;
			}
			return -1;
		}

		void FileStream::Seek(fpos_t size)
		{
			assert(file != nullptr);

			fpos_t pos = Position();
			if (pos + size > Size())
				fseek(file, 0, SEEK_END);
			else if (pos + size < 0)
				fseek(file, 0, SEEK_SET);
			else
				fseek(file, (long)size, SEEK_CUR);
		}

		unsigned int FileStream::Read(void* buffer, unsigned int size)
		{
			assert(file != nullptr && buffer != nullptr);
			assert(mode == TextRead || mode == BinaryRead || mode == TextReadWrite || mode == BinaryReadWrite);

			return fread(buffer, 1, size, file);
		}

		unsigned int FileStream::Write(void* buffer, unsigned int size)
		{
			assert(file != nullptr && buffer != nullptr);
			assert(mode == TextWrite || mode == BinaryWrite || mode == TextReadWrite || mode == BinaryReadWrite);

			return fwrite(buffer, 1, size, file);
		}
	}
}