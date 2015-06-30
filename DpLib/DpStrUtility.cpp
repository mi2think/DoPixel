/********************************************************************
	created:	2014/11/10
	created:	10:11:2014   23:06
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpStrUtility.cpp
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpStrUtility
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	
*********************************************************************/
#include "DpStrUtility.h"
#include <cstring>
#include <cassert>
#include <regex>

namespace DoPixel
{
	namespace Core
	{
		namespace StrUtility
		{
			void StrStrip(char* strDest, const char* strSrc, const char* strPattern)
			{
				assert(strDest != nullptr && strSrc != nullptr && strPattern != nullptr);

				size_t srcLen = strlen(strSrc);
				const char* p = strSrc;
				const char* q = strstr(p, strPattern);
				if (q != nullptr)
				{
					size_t patternLen = strlen(strPattern);

					while (q != nullptr)
					{
						if (p != q)
						{
							ptrdiff_t len = q - p;
							memcpy(strDest, p, len);
							q += patternLen;
							p = q;
							strDest += len;
						}
						else
						{
							q += patternLen;
							p = q;
						}

						if (p != nullptr)
						{
							q = strstr(p, strPattern);
						}
					}
				}

				// last should exist
				const char* last = strSrc + srcLen;
				ptrdiff_t lastLen = last - p + 1;
				memcpy(strDest, p, lastLen);
			}

			void StrStrip(std::string& strDest, const std::string& strSrc, const std::string& strPattern)
			{
				strDest.clear();
				std::string::size_type length = strPattern.size();

				std::string::size_type pos = 0;
				std::string::size_type begin = pos;
				while ((pos = strSrc.find(strPattern, pos)) != std::string::npos)
				{
					strDest += strSrc.substr(begin, pos - begin);
					pos += length;
					begin = pos;
				}

				if (begin != strSrc.size())
				{
					strDest += strSrc.substr(begin, strSrc.size() - begin);
				}
			}

			void StrRegexReplace(std::string& strDest, const std::string& strSrc, const std::string& strRegexPattern, const std::string& strReplace)
			{
				strDest.clear();

				std::regex r(strRegexPattern);
				strDest = std::regex_replace(strSrc, r, strReplace);
			}

			void StrReplace(std::string& strDest, const std::string& strSrc, const std::string& strPattern, const std::string& strReplace)
			{
				strDest.clear();
				std::string::size_type length = strPattern.size();

				std::string::size_type pos = 0;
				std::string::size_type begin = pos;
				while ((pos = strSrc.find(strPattern, pos)) != std::string::npos)
				{
					strDest += strSrc.substr(begin, pos - begin);
					strDest += strReplace;
					pos += length;
					begin = pos;
				}

				if (begin != strSrc.size())
				{
					strDest += strSrc.substr(begin, strSrc.size() - begin);
				}
			}

			void StrSplit(std::vector<std::string>& vec, const std::string& str, char ch)
			{
				vec.clear();
				if (str.empty())
					return;

				std::string::size_type pos = 0;
				std::string::size_type begin = pos;
				while ((pos = str.find(ch, pos)) != std::string::npos)
				{
					std::string strTemp = str.substr(begin, pos - begin);
					if (!strTemp.empty())
					{
						vec.push_back(strTemp);
					}
					pos += sizeof(char);
					begin = pos;
				}

				if (begin != str.size())
				{
					std::string strTemp = str.substr(begin, str.size() - begin);
					if (!strTemp.empty())
					{
						vec.push_back(strTemp);
					}
				}
			}

			void StrTrimLeft(std::string& str)
			{
				if (str.empty())
					return;

				std::string::size_type length = str.size();
				std::string::size_type pos = 0;
				while (pos < length)
				{
					char ch = str[pos];
					if (!isspace(ch))
						break;
					++pos;
				}

				if (pos < length)
				{
					str = str.substr(pos, length - pos);
				}
			}

			void StrTrimRight(std::string& str)
			{
				if (str.empty())
					return;

				std::string::size_type length = str.size();
				int pos = int(length - 1);
				while (pos >= 0)
				{
					char ch = str[pos];
					if (!isspace(ch))
						break;
					--pos;
				}

				if (pos >= 0)
				{
					str = str.substr(0, pos + 1);
				}
			}

			bool ToFloat(float& f, const char* strNum)
			{
				// float: [white space][sign][digits][.digits][{d | D | e | E}[digits]]
				assert(strNum != nullptr);

				const char* p = strNum;
				while (isspace(*p))
					++p;

				if (*p == '-' || *p == '+')
					++p;

				while (isdigit(*p))
					++p;

				if (*p == '.')
				{
					++p;
					while (isdigit(*p))
						++p;
				}

				if (*p == 'e' || *p == 'E')
				{
					++p;
					if (*p == '-' || *p == '+')
						++p;
					while (isdigit(*p))
						++p;
				}

				if (*p == 'f')
					++p;

				if (p == strNum + strlen(strNum))
				{
					f = (float)atof(strNum);
					return true;
				}

				return false;
			}
		}
	}
}