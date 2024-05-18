#include "LogSystem.h"
#include "OgreLogManager.h"

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

		time_t nowtime;
		nowtime = time(NULL);

		struct tm* timeinfo;
		timeinfo = localtime(&nowtime);

		char time_buffer[80];
		strftime(time_buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

		if (pfilename != NULL && pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s][%s(%d)]%s(): %s", time_buffer, pfilename, line, pfunction, pstr);
		}
		else if (pfilename != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s][%s(%d)]: %s", time_buffer, pfilename, line, pstr);
		}
		else if (pfunction != NULL)
		{
			snprintf(buffer, sizeof(buffer), "[%s]%s(): %s", time_buffer, pfunction, pstr);
		}
		else
		{
			snprintf(buffer, sizeof(buffer), "[%s] %s", time_buffer, pstr);
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