/********************************************************************
	created:	2014/12/14
	created:	14:12:2014   19:59
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpFileParser.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpFileParser
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	Text file parser
*********************************************************************/
#include "DpFileParser.h"

namespace dopixel
{
	FileParser::FileParser()
		: buffer_(nullptr)
		, bufferSize_(0)
		, bufferPos_(0)
		, comment_("#")
	{
	}

	FileParser::~FileParser()
	{
		Close();
	}

	bool FileParser::Open(const std::string& fileName)
	{
		ASSERT(buffer_ == nullptr);

		bool b = fileStream_.Open(fileName.c_str(), FileStream::TextRead);
		if (b)
		{
			bufferSize_ = (unsigned int)fileStream_.Size();
			buffer_ = new char[bufferSize_ + 1];
			bufferSize_ = fileStream_.Read(buffer_, bufferSize_);
			*(buffer_ + bufferSize_) = 0;
		}
		return b;
	}

	void FileParser::Close()
	{
		fileStream_.Close();
		if (buffer_)
		{
			delete[] buffer_;
			buffer_ = nullptr;
		}
		bufferPos_ = 0;
		bufferSize_ = 0;
	}

	void FileParser::SetComment(const std::string& comment)
	{
		comment_ = comment;
	}

	bool FileParser::GetLine(std::string& strLine, int lineOp)
	{
		ASSERT(buffer_ != nullptr);

		strLine.clear();
		// If there is nothing to read, except '\0', return false
		if (bufferPos_ >= bufferSize_)
			return false;

		auto fnGetLine = [this, &strLine]() -> void
		{
			const char* pBegin = buffer_ + bufferPos_;
			if (*pBegin == 0)
				return;

			const char* p = strchr(pBegin, '\n');
			if (!p)
			{
				// There must be single line
				unsigned int len = buffer_ + bufferSize_ - pBegin;
				if (len > 0)
					strLine = std::string(pBegin, len);
				bufferPos_ += len;
			}
			else
			{
				// There are multi lines
				unsigned int len = p - pBegin;
				if (len > 0)
				{
					strLine = std::string(pBegin, len);
					bufferPos_ += len;
				}
				// Skip '\n'
				++bufferPos_;
			}
		};

		if ((lineOp & StripEmptyLine) != 0)
		{
			// Get a no empty line
			while (bufferPos_ < bufferSize_)
			{
				fnGetLine();

				std::string::size_type strIndex = 0;
				// Skip spaces
				for (; strIndex < strLine.size(); ++strIndex)
				{
					if (!isspace(strLine[strIndex]))
						break;
				}
				if (strIndex < strLine.size() && strIndex != 0)
				{
					strLine = strLine.substr(strIndex, strLine.size() - strIndex);
				}

				// May be this line just comments
				if ((lineOp & StripComments) != 0 && !comment_.empty())
				{
					std::string::size_type commentsIndex = strLine.find(comment_);
					if (commentsIndex != std::string::npos)
					{
						// Two case: 1 at the begin; 2 at the end
						if (commentsIndex == 0)
							strLine.clear();
						else
							strLine = strLine.substr(0, commentsIndex);
					}
				}

				if (!strLine.empty())
					break;
			}
		}
		else
		{
			fnGetLine();
		}

		if (!strLine.empty())
		{
			if ((lineOp & StripLineSpace) != 0)
			{
				str_trim(strLine);
			}
		}
		// When read the last char, bufferPos_ add 1 and then equal to bufferSize_
		return bufferPos_ <= bufferSize_;
	}
}