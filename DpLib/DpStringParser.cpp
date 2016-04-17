/********************************************************************
	created:	2016/04/17
	created:	17:4:2016   11:42
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpStringParser.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpStringParser
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	String Parser
*********************************************************************/
#include "DpStringParser.h"
#include <regex>

#define PRINT_REGEX_MATCH_INFO

namespace dopixel
{
	StringParser::RegexInfo::RegexInfo(const std::string& fmt)
	{
		GenRegexInfo(fmt);
	}

	void StringParser::RegexInfo::GenRegexInfo(const std::string& fmt)
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
				ASSERT(state == MatchWait);
				state = MatchBein;
				break;
			case ']':
				ASSERT(state == MatchBein);
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

	StringParser::StringParser()
	{
	}

	StringParser::~StringParser()
	{
	}

	bool StringParser::PatternMatch(const std::string& str, const std::string& fmt)
	{
		return RegexPatternMatch(str, RegexInfo(fmt));
	}

	bool StringParser::RegexPatternMatch(const std::string& str, const RegexInfo& regexInfo)
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
			GLOGS << results.str() << "\n";
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
				GLOGS << subExpr.str() << "\n";
#endif
			}
			return true;
		}
		return false;
	}
}
