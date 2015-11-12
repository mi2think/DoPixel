/********************************************************************
	created:	2015/09/20
	created:	20:9:2015   16:41
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DoPixel.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DoPixel
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	DoPixel Interface
*********************************************************************/
#ifndef __DOPIXEL__
#define __DOPIXEL__

#include "DpRef.h"

#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace dopixel
{
	using string = std::string;
	using wstring = std::wstring;
	using std::vector;
	using std::map;

	class Camera;
	class Event;

	typedef Ref<Camera> CameraRef;
	typedef Ref<Event> EventRef;

	// can not copy
	class NoCopyable
	{
	public:
		NoCopyable() {}
		NoCopyable(const NoCopyable&) = delete;
		NoCopyable& operator=(const NoCopyable&) = delete;
	};

	// scope guard
	class ScopeGuard : public NoCopyable
	{
	public:
		ScopeGuard(std::function<void()> func) : func_(func) {}
		~ScopeGuard() { func_(); }
	private:
		std::function<void()> func_;
	};
	#define SCOPEGUARD_NAMELINE_CAT(name, line) name##line
	#define SCOPEGUARD_NAMELINE(name, line) SCOPEGUARD_NAMELINE_CAT(name, line)
	#define ON_SCOPE_EXIT(func) ScopeGuard SCOPEGUARD_NAMELINE(EXIT, __LINE__)(func)

	// macros
	#define SAFE_DELETE(p)		do { if (p != nullptr) { delete p; p = nullptr; } } while(0)
	#define SAFE_DELETEARRAY(p) do { if (p != nullptr) { delete[] p; p = nullptr; } } while(0)
	#define SAFE_RELEASE(p)		do { if (p != nullptr) { p->Release(); p = nullptr; } } while(0)

	#define ASSERT	assert
	#define BIT(x)      (1<<(x))

	// swap
	template<typename T>
	inline void swap_t(T& a, T& b) { T t = a; a = b; b = t; }

	// string utils

	// string copy
	void strncpy_s(char* pDest, const char* pSrc, int destSize);
	// string format
	// use internal static space hold format result, length limits to 1024
	const char* str_format(const char* format, ...);
	// string trim
	enum { TRIM_LEFT = 1, TRIM_RIGHT };
	void str_trim(string& str, int flag = TRIM_LEFT | TRIM_RIGHT);
	// string strip
	void str_strip(char* strDest, const char* strSrc, const char* strPattern);
	void str_strip(string& strDest, const string& strSrc, const string& strPattern);
	// string strip any char
	// common used in parse file, e.g. "<1,2> <2,3>" -> "1,2 2,3", where any char is:"<>" 
	void str_strip_anychar(char* strDest, const char* strSrc, const char* anychar);
	void str_strip_anychar(string& strDest, const string& strSrc, const string& anychar);
	// string split
	void str_split(vector<string>& vec, const string& str, char ch);
	// string replace
	void str_replace(string& strDest, const string& strSrc, const string& strPattern, const string& strReplace);
	// string replace by regex pattern
	void str_regex_replace(string& strDest, const string& strSrc, const string& strRegexPattern, const string& strReplace);
	// string replace any char
	void str_replace_anychar(string& strDest, const string& strSrc, const string& anychar, const string& strReplace);
	// string is valid float format
	bool str_is_valid_float(const string& str);
}

#endif // !__DOPIXEL__
