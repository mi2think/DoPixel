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

			FileStream();
			FileStream(const char* fileName, Mode _mode);
			~FileStream();

			bool Open(const char* fileName, Mode _mode);

			void Close();

			fpos_t Size() const;

			fpos_t Position() const;

			void Seek(fpos_t size);

			unsigned int Read(void* buffer, unsigned int size);

			unsigned int Write(void* buffer, unsigned int size);

			FILE* GetFile() const { return file; }
		private:
			FILE* file;
			int mode;
		};
	}
}

#endif