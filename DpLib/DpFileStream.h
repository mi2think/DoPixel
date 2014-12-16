/********************************************************************
	created:	2014/06/28
	created:	28:6:2014   21:27
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpFileStream.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpFileStream
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	File Stream
*********************************************************************/

#ifndef __DP_FILE_STREAM__
#define __DP_FILE_STREAM__

#include "DpNoCopy.h"
#include <cstdio>
#include <cassert>

namespace DoPixel
{
	namespace Core
	{
		class FileStream : public NoCopyable
		{
		public:
			enum Mode
			{
				TextRead,
				TextWrite,
				TextReadWrite,
				
				BinaryRead,
				BinaryWrite,
				BinaryReadWrite,
			};

			FileStream() : file(nullptr) {}
			FileStream(const char* fileName, Mode _mode)
			{
				Open(fileName, _mode);
			}
			~FileStream() { Close(); }

			bool Open(const char* fileName, Mode _mode)
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
				assert(ret == 0);
				return ret == 0;
			}

			void Close()
			{
				if (file)
				{
					fclose(file);
					file = nullptr;
				}
			}

			fpos_t Size() const
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

			fpos_t Position() const
			{
				assert(file != nullptr);

				fpos_t pos = 0;
				if (fgetpos(file, &pos) == 0)
				{
					return pos;
				}
				return -1;
			}

			void Seek(fpos_t size)
			{
				assert(file != nullptr);

				fpos_t pos = Position();
				if (pos + size > Size())
					fseek(file, 0, SEEK_END);
				else if (pos + size < 0)
					fseek(file, 0 , SEEK_SET);
				else
					fseek(file, (long)size, SEEK_CUR);
			}

			unsigned int Read(void* buffer, unsigned int size)
			{
				assert(file != nullptr && buffer != nullptr);
				assert (mode == TextRead || mode == BinaryRead || mode == TextReadWrite || mode == BinaryReadWrite);

				return fread(buffer, 1, size, file);
			}

			unsigned int Write(void* buffer, unsigned int size)
			{
				assert(file != nullptr && buffer != nullptr);
				assert (mode == TextWrite || mode == BinaryWrite || mode == TextReadWrite || mode == BinaryReadWrite);

				return fwrite(buffer, 1, size, file);
			}

			FILE* GetFile() const { return file; }
		private:
			FILE* file;
			int mode;
		};
	}
}

#endif