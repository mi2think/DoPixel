/********************************************************************
	created:	2014/11/10
	created:	10:11:2014   22:38
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpStrUtility.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpStrUtility
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	String utility
*********************************************************************/

#ifndef __DP_STR_UTILITY__
#define __DP_STR_UTILITY__

#include <string>
#include <vector>

namespace DoPixel
{
	namespace Core
	{
		namespace StrUtility
		{
			enum { CaseSensitive, NoCaseSensitive, };

			void StrStrip(char* strDest, const char* strSrc, const char* strPattern);

			void StrStrip(std::string& strDest, const std::string& strSrc, const std::string& strPattern);

			void StrReplace(std::string& strDest, const std::string& strSrc, const std::string& strPattern, const std::string& strReplace);

			void StrRegexReplace(std::string& strDest, const std::string& strSrc, const std::string& strRegexPattern, const std::string& strReplace);

			void StrSplit(std::vector<std::string>& vec, const std::string& str, char ch);

			void StrTrimLeft(std::string& str);

			void StrTrimRight(std::string& str);

			bool ToFloat(float& f, const char* strNum);
		}
	}
}

#endif
