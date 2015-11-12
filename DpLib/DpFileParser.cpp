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
#include <regex>
#include <cassert>

#define PRINT_REGEX_MATCH_INFO
#ifdef PRINT_REGEX_MATCH_INFO
#include "DpConsole.h"
using namespace dopixel::Console;
#endif

namespace dopixel
{
	FileParser::RegexInfo::RegexInfo(const std::string& fmt)
	{
		GenRegexInfo(fmt);
	}

	void FileParser::RegexInfo::GenRegexInfo(const std::string& fmt)
	{
		// string literal                        = ['string']
		// floating point number                 = [f]
		// integer number                        = [i]
		// match a string						 = [s]
		// match a string exactly ddd chars      = [s=ddd] 
		// match a string less than ddd chars    = [s<ddd] 
		// match a string greater than ddd chars = [s>ddd]

		// for example to match "vertex: 34.234 56.34 12.4"
		// ['vertex'] [f] [f] [f]

		vecSubExprType.clear();
		regexString.clear();

		std::string pattern = fmt;
		str_trim(pattern);

		// func
		auto fnStringLiteral = [](std::string::size_type& i, const std::string& str) -> std::string
		{
			std::string::size_type pos = ++i; // skip '\''
			while (str.at(i) != '\'')
				++i;
			std::string s = str.substr(pos, i - pos);
			return s;
		};
		auto fnStringFormat = [](std::string::size_type& i, const std::string& str)->std::string
		{
			std::string s = "[0-9a-zA-Z \\\\:._-]";

			char op = str[++i];
			if (op == ']')		//[s]
			{
				--i; // for next char is ']'
				return s + "+";
			}

			// For s op num
			// skip '=' or '<' or '>'
			std::string::size_type pos = ++i;
			while (str.at(i) != ']')
				++i;

			std::string str_num = str.substr(pos, i - pos);
			if (op == '=')
				s += "{" + str_num + "}";	// {n}
			else if (op == '<')
				s += "{0," + str_num + "}";	// {n,m}
			else if (op == '>')
				s += "{" + str_num + ",}";	// {n,}
			--i; // for next char is ']'
			return s;
		};

		enum STATE { MatchWait, MatchBein, MatchEnd, };
		unsigned int state = MatchWait;

		std::string::size_type i = 0;
		while (i < pattern.size())
		{
			char ch = pattern[i];
			switch (ch)
			{
			case '[':
				assert(state == MatchWait);
				state = MatchBein;
				break;
			case ']':
				assert(state == MatchBein);
				state = MatchEnd;
				break;
			case '\'':
				regexString += "(" + fnStringLiteral(i, pattern) + ")";
				vecSubExprType.push_back(TypeStr);
				break;
			case 'f':
				regexString += "([-+]?[0-9]+[.]?[0-9]*([eE][-+]?[0-9]+)?)";
				vecSubExprType.push_back(TypeFloat);
				break;
			case 'i':
				regexString += "([+-]?[0-9]+)";
				vecSubExprType.push_back(TypeInt);
				break;
			case 's':
				regexString += "(" + fnStringFormat(i, pattern) + ")";
				vecSubExprType.push_back(TypeStr);
				break;
			default:
				break;
			}
			++i;

			// group by space
			if (state == MatchEnd)
			{
				regexString += "([ ]*)";
				vecSubExprType.push_back(TypeNone);
				state = MatchWait;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////

	bool FileParser::Open(const std::string& fileName)
	{
		assert(buffer == nullptr);

		bool b = fileStream.Open(fileName.c_str(), FileStream::TextRead);
		if (b)
		{
			bufferSize = (unsigned int)fileStream.Size();
			buffer = new char[bufferSize + 1];
			bufferSize = fileStream.Read(buffer, bufferSize);
			*(buffer + bufferSize) = 0;
		}
		return b;
	}

	void FileParser::Close()
	{
		fileStream.Close();
		if (buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}
		bufferPos = 0;
		bufferSize = 0;
	}

	bool FileParser::GetLine(std::string& strLine, int lineOp)
	{
		assert(buffer != nullptr);

		strLine.clear();
		// If there is nothing to read, except '\0', return false
		if (bufferPos >= bufferSize)
			return false;

		auto fnGetLine = [this, &strLine]() -> void
		{
			const char* pBegin = buffer + bufferPos;
			if (*pBegin == 0)
				return;

			const char* p = strchr(pBegin, '\n');
			if (!p)
			{
				// There must be single line
				unsigned int len = buffer + bufferSize - pBegin;
				if (len > 0)
					strLine = std::string(pBegin, len);
				bufferPos += len;
			}
			else
			{
				// There are multi lines
				unsigned int len = p - pBegin;
				if (len > 0)
				{
					strLine = std::string(pBegin, len);
					bufferPos += len;
				}
				// Skip '\n'
				++bufferPos;
			}
		};

		if ((lineOp & StripEmptyLine) != 0)
		{
			// Get a no empty line
			while (bufferPos < bufferSize)
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
				if ((lineOp & StripComments) != 0 && !comment.empty())
				{
					std::string::size_type commentsIndex = strLine.find(comment);
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
		// When read the last char, bufferPos add 1 and then equal to bufferSize
		return bufferPos <= bufferSize;
	}

	bool FileParser::PatternMatch(const std::string& str, const std::string& fmt)
	{
		return RegexPatternMatch(str, RegexInfo(fmt));
	}

	bool FileParser::RegexPatternMatch(const std::string& str, const RegexInfo& regexInfo)
	{
		matchInts.clear();
		matchStrings.clear();
		matchFloats.clear();

		// Parse
		std::regex r(regexInfo.regexString);
		std::smatch results;
		if (std::regex_search(str, results, r))
		{
#ifdef PRINT_REGEX_MATCH_INFO
			os_cout << results.str() << "\n";
#endif
			// Save sub expression
			for (std::smatch::size_type i = 1, index = 1; i < results.size(); ++i)
			{
				auto& subExpr = results[i];
				if (!subExpr.matched)
					continue;

				if (index > regexInfo.vecSubExprType.size())
					break;

				auto& subExprType = regexInfo.vecSubExprType[index - 1];

				switch (subExprType)
				{
				case RegexInfo::TypeStr:
					matchStrings.push_back(subExpr.str());
					break;
				case RegexInfo::TypeInt:
					matchInts.push_back(atoi(subExpr.str().c_str()));
					break;
				case RegexInfo::TypeFloat:
					matchFloats.push_back((float)atof(subExpr.str().c_str()));
					break;
				default:
					break;
				}
				++index;
#ifdef PRINT_REGEX_MATCH_INFO
				os_cout << subExpr.str() << "\n";
#endif
			}
			return true;
		}
		return false;
	}
}