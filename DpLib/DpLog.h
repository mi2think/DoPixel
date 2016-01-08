/********************************************************************
	created:	2016/01/07
	created:	7:1:2016   23:21
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpLog.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpLog
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Log
*********************************************************************/
#ifndef __DP_LOG_H__
#define __DP_LOG_H__

namespace dopixel
{
	class Log
	{
	public:
		struct ScopeNewline
		{
			ScopeNewline(bool newline);
			~ScopeNewline();
			bool newline_;
		};

		enum LogLevel
		{
			Info,
			Warn,
			Error,
			Fatal,
		};
		Log();

		void WriteBuf(int level, const char* fmt, ...);
		unsigned short GetTextColor(int level);
		void SetTextColor(unsigned short color);
		void SetNewline(bool newline) { newline_ = newline; }
		bool GetNewline() const { return newline_; }
	private:
		void* hwnd_;
		bool newline_;
	};

	extern Log g_Log;

	#define LOG_TO(level, fmt, ...) g_Log.WriteBuf(level, fmt, __VA_ARGS__)
	#define LOG_INFO(fmt, ...)	LOG_TO(Log::Info, fmt, __VA_ARGS__)
	#define LOG_WARN(fmt, ...)	LOG_TO(Log::Warn, fmt, __VA_ARGS__)
	#define LOG_ERROR(fmt, ...)	LOG_TO(Log::Error, fmt, __VA_ARGS__)
	#define LOG_FATAL(fmt, ...)	LOG_TO(Log::Fatal, fmt, __VA_ARGS__)

	#define LOGSCOPE_NAMELINE_CAT(name, line) name##line
	#define LOGSCOPE_NAMELINE(name, line) LOGSCOPE_NAMELINE_CAT(name, line)
	#define LOGSCOPE_NEWLINE(newline) Log::ScopeNewline LOGSCOPE_NAMELINE(LogScope, __LINE__)(newline)
}

#endif