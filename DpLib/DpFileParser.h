/********************************************************************
	created:	2014/11/10
	created:	10:11:2014   21:52
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpFileParser.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpFileParser
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Text file parser
*********************************************************************/

#ifndef __DP_FILE_PARSER__
#define __DP_FILE_PARSER__

#include "DoPixel.h"
#include "DpFileStream.h"

namespace dopixel
{
	class FileParser : public NoCopyable
	{
	public:
		FileParser();
		~FileParser();

		// If open success return true, otherwise false
		bool Open(const std::string& fileName);

		void Close();

		// Read a line
		enum LineOP
		{
			StripEmptyLine = 0x1,	// Strip empty line
			StripLineSpace = 0x2,	// Strip end space of line
			StripComments = 0x4,	// Strip comments
		};
		bool GetLine(std::string& strLine, int lineOp = StripEmptyLine | StripLineSpace | StripComments);

		void SetComment(const std::string& comment);
	private:
		FileStream fileStream_;
		std::string comment_;
		char* buffer_;
		unsigned int bufferSize_;
		unsigned int bufferPos_;
	};
}

#endif
