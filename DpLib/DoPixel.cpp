/********************************************************************
	created:	2015/09/26
	created:	26:9:2015   0:31
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DoPixel.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DoPixel
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	DoPixel
*********************************************************************/
#include "DoPixel.h"
#include <cstdarg>
#include <regex>

namespace dopixel
{
	void strncpy_s(char* pDest, const char* pSrc, int destSize)
	{
		if (!pDest || !pSrc)
			return;
		int len = strlen(pSrc) + 1;
		if (len > destSize)
			len = destSize;
		memcpy(pDest, pSrc, len);
		*(pDest + len - 1) = 0;
	}

	const char* str_format(const char* format, ...)
	{
		static char buffer[1024];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, format, ap);
		va_end(ap);
		buffer[1023] = 0;
		return buffer;
	}

	void str_strip(char* strDest, const char* strSrc, const char* strPattern)
	{
		ASSERT(strDest != nullptr && strSrc != nullptr && strPattern != nullptr);

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

	void str_strip(string& strDest, const string& strSrc, const string& strPattern)
	{
		strDest.clear();
		string::size_type length = strPattern.size();

		string::size_type pos = 0;
		string::size_type begin = pos;
		while ((pos = strSrc.find(strPattern, pos)) != string::npos)
		{
			if (pos != begin)
			{
				strDest += strSrc.substr(begin, pos - begin);
			}
			pos += length;
			begin = pos;
		}

		if (begin != strSrc.size())
		{
			strDest += strSrc.substr(begin, strSrc.size() - begin);
		}
	}

	void str_strip_anychar(char* strDest, const char* strSrc, const char* anychar)
	{
		ASSERT(strDest != nullptr && strSrc != nullptr && anychar != nullptr);

		size_t srcLen = strlen(strSrc);
		const char* p = strSrc;
		const char* q = p;

		char ch;
		while ((ch = *q) != 0)
		{
			if (strchr(anychar, ch) == nullptr)
			{
				++q;
			}
			else
			{
				if (p != q)
				{
					ptrdiff_t len = q - p;
					memcpy(strDest, p, len);
					strDest += len;
				}

				++q;
				p = q;
			}
		}

		// last should exist
		const char* last = strSrc + srcLen;
		ptrdiff_t lastLen = last - p + 1;
		memcpy(strDest, p, lastLen);
	}

	void str_strip_anychar(string& strDest, const string& strSrc, const string& anychar)
	{
		strDest.clear();

		std::string::size_type pos = 0;
		std::string::size_type begin = pos;

		char ch;
		while (pos < strSrc.size())
		{
			ch = strSrc[pos];
			if (anychar.find(ch) == std::string::npos)
			{
				++pos;
			}
			else
			{
				if (pos != begin)
				{
					strDest += strSrc.substr(begin, pos - begin);
				}
				++pos;
				begin = pos;
			}
		}

		if (begin != strSrc.size())
		{
			strDest += strSrc.substr(begin, strSrc.size() - begin);
		}
	}

	void str_split(vector<string>& vec, const string& str, char ch)
	{
		vec.clear();
		if (str.empty())
			return;

		string::size_type pos = 0;
		string::size_type begin = pos;
		while ((pos = str.find(ch, pos)) != string::npos)
		{
			string strTemp = str.substr(begin, pos - begin);
			if (!strTemp.empty())
			{
				vec.push_back(strTemp);
			}
			pos += sizeof(char);
			begin = pos;
		}

		if (begin != str.size())
		{
			string strTemp = str.substr(begin, str.size() - begin);
			if (!strTemp.empty())
			{
				vec.push_back(strTemp);
			}
		}
	}

	void str_replace(string& strDest, const string& strSrc, const string& strPattern, const string& strReplace)
	{
		strDest.clear();
		string::size_type length = strPattern.size();

		string::size_type pos = 0;
		string::size_type begin = pos;
		while ((pos = strSrc.find(strPattern, pos)) != string::npos)
		{
			if (pos != begin)
			{
				strDest += strSrc.substr(begin, pos - begin);
			}
			strDest += strReplace;
			pos += length;
			begin = pos;
		}

		if (begin != strSrc.size())
		{
			strDest += strSrc.substr(begin, strSrc.size() - begin);
		}
	}

	void str_regex_replace(string& strDest, const string& strSrc, const string& strRegexPattern, const string& strReplace)
	{
		strDest.clear();

		std::regex r(strRegexPattern);
		strDest = std::regex_replace(strSrc, r, strReplace);
	}

	void str_replace_anychar(string& strDest, const string& strSrc, const string& anychar, const string& strReplace)
	{
		strDest.clear();

		std::string::size_type pos = 0;
		std::string::size_type begin = pos;

		char ch;
		while (pos < strSrc.size())
		{
			ch = strSrc[pos];
			if (anychar.find(ch) == std::string::npos)
			{
				++pos;
			}
			else
			{
				if (pos != begin)
				{
					strDest += strSrc.substr(begin, pos - begin);
				}
				strDest += strReplace;
				++pos;
				begin = pos;
			}
		}

		if (begin != strSrc.size())
		{
			strDest += strSrc.substr(begin, strSrc.size() - begin);
		}
	}

	void str_trim(string& str, int flag /* = TRIM_LEFT | TRIM_RIGHT */)
	{
		if (str.empty())
			return;

		string::size_type length = str.size();
		if ((flag & TRIM_LEFT) != 0)
		{
			string::size_type pos = 0;
			while (pos < length)
			{
				if (!isspace(str[pos]))
					break;
				++pos;
			}

			if (pos < length)
			{
				str = str.substr(pos, length - pos);
				length = str.size();
			}
		}

		if ((flag & TRIM_RIGHT) != 0)
		{
			int pos = int(length) - 1;
			while (pos >= 0)
			{
				if (!isspace(str[pos]))
					break;
				--pos;
			}

			if (pos >= 0)
			{
				str = str.substr(0, pos + 1);
			}
		}
	}

	bool str_is_valid_float(const string& str)
	{
		// float: [white space][sign][digits][.digits][{d | D | e | E}[digits]]
		const char* begin = str.c_str();
		const char* p = begin;

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

		return p == begin + str.size();
	}
}

