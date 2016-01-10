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

#include <sstream>

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

		static Log& GetInstance() { static Log s_ins; return s_ins; }
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

	#define GLOG Log::GetInstance()
	#define LOG_TO(level, fmt, ...) GLOG.WriteBuf(level, fmt, __VA_ARGS__)
	#define LOG_INFO(fmt, ...)	LOG_TO(Log::Info, fmt, __VA_ARGS__)
	#define LOG_WARN(fmt, ...)	LOG_TO(Log::Warn, fmt, __VA_ARGS__)
	#define LOG_ERROR(fmt, ...)	LOG_TO(Log::Error, fmt, __VA_ARGS__)
	#define LOG_FATAL(fmt, ...)	LOG_TO(Log::Fatal, fmt, __VA_ARGS__)

	#define LOGSCOPE_NAMELINE_CAT(name, line) name##line
	#define LOGSCOPE_NAMELINE(name, line) LOGSCOPE_NAMELINE_CAT(name, line)
	#define LOGSCOPE_NEWLINE(newline) Log::ScopeNewline LOGSCOPE_NAMELINE(LogScope, __LINE__)(newline)

	// log stream
	class LogStream
	{
	public:
		static LogStream& GetInstance() { static LogStream s_ins; return s_ins; }
		LogStream() : level_(Log::Info) {}

		void SetLevel(int level) { level_ = level; }

		template<typename T>
		LogStream& operator<<(const T& t)
		{
			LOGSCOPE_NEWLINE(false);
			std::ostringstream oss;
			oss << t;
			GLOG.WriteBuf(level_, "%s", oss.str().c_str());
			return *this;
		}
	private:
		int level_;
	};

	#define GLOGS LogStream::GetInstance()
}

#endif