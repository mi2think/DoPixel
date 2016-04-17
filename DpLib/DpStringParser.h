/********************************************************************
	created:	2016/04/17
	created:	17:4:2016   10:51
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpStringParser.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpStringParser
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	String Parser
*********************************************************************/
#ifndef __DP_STRING_PARSER_H__
#define __DP_STRING_PARSER_H__

#include "DoPixel.h"

namespace dopixel
{
	class StringParser
	{
	public:
		StringParser();
		~StringParser();
		// format:
		//
		// [i] : integer
		// [f] : float
		// [s]	 : match string s
		// [s=d] : match string which length equals d
		// [s<d] : match string which length less than d
		// [s>d] : match string which logger than d
		// ['s..s'] : match the given string


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
		// Store for pattern match
		std::vector<int>			matchInts;
		std::vector<float>			matchFloats;
		std::vector<std::string>	matchStrings;
	};
}

#endif
