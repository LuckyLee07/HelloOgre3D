#ifndef __CCLOG_SYSTEM_H__
#define __CCLOG_SYSTEM_H__

#include "OgreLog.h"
#include <string.h>

namespace Fancy 
{
	extern void CCLogMessage(const char* format, ...);
	extern void CCLogSetParam(const char* filename, const char* funcname, int line, Ogre::LogMessageLevel level);

	extern std::string getLogMessage(const char* pfilename, const char* pfunction, int line, const char* pstr);
}

#define __FILENAME__ __FILE__
// 相关的几个日志打印接口
#define	CCLUA_INFO		Fancy::CCLogSetParam(nullptr, nullptr, __LINE__, Ogre::LML_NORMAL),	Fancy::CCLogMessage
#define CCLUA_ERROR		Fancy::CCLogSetParam(nullptr, nullptr, __LINE__, Ogre::LML_CRITICAL),	Fancy::CCLogMessage

#define	CCLOG_INFO		Fancy::CCLogSetParam((__FILENAME__), __FUNCTION__, __LINE__, Ogre::LML_NORMAL),	Fancy::CCLogMessage
//#define	CCLOG_DEBUG		Fancy::CCLogSetParam((__FILENAME__), __FUNCTION__, __LINE__, Ogre::LML_NORMAL),	Fancy::CCLogMessage
#define CCLOG_ERROR		Fancy::CCLogSetParam((__FILENAME__), __FUNCTION__, __LINE__, Ogre::LML_CRITICAL),	Fancy::CCLogMessage

#endif //__CCLOG_SYSTEM_H__
