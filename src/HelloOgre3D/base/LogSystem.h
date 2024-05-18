#ifndef __LOG_SYSTEM_H__
#define __LOG_SYSTEM_H__

#include "OgreLog.h"
#include <string.h>

#define LOG_WITH_TIME	0

namespace Fancy 
{
	extern void LogMessage(const char* format, ...);
	extern void LogSetParam(const char* filename, const char* funcname, int line, Ogre::LogMessageLevel level);

	extern std::string getLogMessage(const char* pfilename, const char* pfunction, int line, const char* pstr);
}

#define __FILENAME__ __FILE__
// 相关的几个日志打印接口
#define	CCLUA_INFO		Fancy::LogSetParam(nullptr, nullptr, __LINE__, Ogre::LML_NORMAL),	Fancy::LogMessage
#define CCLUA_ERROR		Fancy::LogSetParam(nullptr, nullptr, __LINE__, Ogre::LML_CRITICAL),	Fancy::LogMessage

#define	CCLOG_INFO		Fancy::LogSetParam((__FILENAME__), nullptr, __LINE__, Ogre::LML_NORMAL),	Fancy::LogMessage
#define CCLOG_ERROR		Fancy::LogSetParam((__FILENAME__), nullptr, __LINE__, Ogre::LML_CRITICAL),	Fancy::LogMessage
//#define	CCLOG_DEBUG		Fancy::LogSetParam((__FILENAME__), __FUNCTION__, __LINE__, Ogre::LML_NORMAL),	Fancy::LogMessage

#endif //__LOG_SYSTEM_H__
