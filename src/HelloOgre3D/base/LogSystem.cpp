#include "LogSystem.h"
#include "OgreLogManager.h"
#include <chrono>

using namespace std::chrono;

namespace Fancy
{
	static int s_iLine;
	static const char *s_pFileName;
	static const char *s_pFuncName;
	static Ogre::LogMessageLevel s_LogLevel;

	void LogSetParam(const char *filename, const char *funcname, int line, Ogre::LogMessageLevel level)
	{
		s_iLine = line;
		s_LogLevel = level;
		s_pFuncName = funcname;
		s_pFileName = filename;
		
#ifdef _WIN32
		if (filename != nullptr) 
		{
			const char* newfname = strrchr(filename, '\\');// Windows_反斜杠 
			if (newfname != NULL) s_pFileName = newfname + 1;//去掉斜杠
		}
#endif
		/*
		static const char* brefix = "client";
		static int bresize = strlen(brefix)+1;//去掉斜杠

		const char* newfname = strstr(fullpath, brefix);
		if (newfname != NULL) s_pFileName = newfname + bresize;
		*/
	}

	std::string getLogMessage(const char* pfilename, const char* pfunction, int line, const char* pstr)
	{
#ifdef _WIN32
		char buffer[1024 * 10];
		// 获取当前时间点
		auto now = std::chrono::system_clock::now();

		// 转换为 time_t 用于格式化年月日时分秒
		auto now_time_t = std::chrono::system_clock::to_time_t(now);
		
		// 使用 localtime 格式化时间（注意线程安全问题）
		std::tm local_tm;
#if defined(_WIN32) || defined(_WIN64)
		localtime_s(&local_tm, &now_time_t);  // Windows 安全版本
#else
		localtime_r(&now_time_t, &local_tm);  // Linux/macOS 安全版本
#endif
		// 取出毫秒部分
		auto time_epoch = now.time_since_epoch();
		auto now_ms = duration_cast<milliseconds>(time_epoch) % 1000;
		
		// 拼接时间字符串
		std::ostringstream oss;
		oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << '.'
			<< std::setfill('0') << std::setw(3) << now_ms.count();
		
		if (pfilename != NULL && pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s][%s(%d)]%s(): %s", oss.str().c_str(), pfilename, line, pfunction, pstr);
		}
		else if (pfilename != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s][%s(%d)]: %s", oss.str().c_str(), pfilename, line, pstr);
		}
		else if (pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s]%s(): %s", oss.str().c_str(), pfunction, pstr);
		}
		else
		{
			snprintf(buffer, sizeof(buffer), "[%s] %s", oss.str().c_str(), pstr);
		}
		return buffer;
#else
		return "--";
#endif
	}

	std::string getLogMessageWithoutTime(const char* pfilename, const char* pfunction, int line, const char* pstr)
	{
#ifdef _WIN32
		char buffer[1024 * 10];

		if (pfilename != NULL && pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s(%d)]%s(): %s", pfilename, line, pfunction, pstr);
		}
		else if (pfilename != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s(%d)]: %s", pfilename, line, pstr);
		}
		else if (pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "%s(): %s", pfunction, pstr);
		}
		else
		{
			snprintf(buffer, sizeof(buffer), "%s", pstr);
		}
		return buffer;
#else
		return "--";
#endif
	}

	void LogMessage(const char *format, ...)
	{
		if (format == NULL) format = "";

		va_list		argptr;
		char message[1024 * 10];

		va_start(argptr, format);
		vsnprintf(message, sizeof(message), format, argptr);
		va_end(argptr);
		
#if LOG_WITH_TIME
		const std::string& buffer = getLogMessage(s_pFileName, s_pFuncName, s_iLine, message);
		Ogre::LogManager::getSingletonPtr()->logMessage(s_LogLevel, buffer.c_str());
#else
		const std::string& buffer = getLogMessageWithoutTime(s_pFileName, s_pFuncName, s_iLine, message);
		Ogre::LogManager::getSingletonPtr()->logMessage(s_LogLevel, buffer.c_str());
#endif // LOG_WITH_TIME

		
	}
}