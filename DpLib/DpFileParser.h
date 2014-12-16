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

#include "DpNoCopy.h"
#include "DpFileStream.h"
#include "DpStrUtility.h"
#include <regex>

#ifdef _DEBUG
#include "DpConsole.h"
using namespace DoPixel::Console;
#endif

namespace DoPixel
{
	namespace Core
	{
		class FileParser : public NoCopyable
		{
		public:
			FileParser() : buffer(nullptr), bufferSize(0), bufferPos(0), comment("#") {}
			~FileParser() { Close(); }

			// If open success return true, otherwise false
			bool Open(const std::string& fileName);

			void Close();

			// Read a line
			enum LineOP
			{
				StripEmptyLine		= 0x1,	// Strip empty line
				StripLineSpace		= 0x2,	// Strip end space of line
				StripComments		= 0x4,	// Strip comments
			};
			bool GetLine(std::string& strLine, int lineOp = StripEmptyLine | StripLineSpace | StripComments);

			void SetComment(const std::string& comment) { this->comment = comment; }

			// Info for convert fmt string to regex string
			struct RegexInfo
			{
				// sub express index - Type
				enum SubExprType { TypeNone, TypeStr, TypeInt, TypeFloat, };
				std::vector<SubExprType> vecSubExprType;
				std::string regexString;

				RegexInfo() {}
				RegexInfo(const std::string& fmt);
				void GenRegexInfo(const std::string& fmt);
			};
			
			// [i] : integer
			// [f] : float
			// [s]	 : match string s
			// [s=d] : match string which length equals d
			// [s<d] : match string which length less than d
			// [s>d] : match string which logger than d
			// ['s..s'] : match the given string

			// Pattern match with raw fmt
			bool PatternMatch(const std::string& str, const std::string& fmt);

			// Pattern match with regex info
			bool RegexPatternMatch(const std::string& str, const RegexInfo& regexInfo);

			// For get pattern matched value
			template <typename T>
			inline T GetMatchedVal(int index) { return T(0); }
			template<> int			GetMatchedVal(int index) { return matchInts.at(index); }
			template<> float		GetMatchedVal(int index) { return matchFloats.at(index); }
			template<> std::string	GetMatchedVal(int index) { return matchStrings.at(index); }
		private:
			FileStream fileStream;
			std::string comment;
			char* buffer;
			unsigned int bufferSize;
			unsigned int bufferPos;

			// Store for pattern match
			std::vector<int>			matchInts;
			std::vector<float>			matchFloats;
			std::vector<std::string>	matchStrings;

			// For gen pattern regex, temp
		};

	
	}
}




#endif
