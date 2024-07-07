#include "FileManager.h"

#ifdef _WIN32
#include <io.h>
#include <corecrt_io.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <Windows.h>
const int CREATE_PMODE = S_IREAD | S_IWRITE;
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#define O_BINARY (0)
const int CREATE_PMODE = S_IRWXU | S_IRWXG | S_IRWXO;
#endif
#include "LogSystem.h"

//int g_openfiles = 0;
FileAutoClose::FileAutoClose(const std::string &path, int flags)
{
	m_FP = ::open(path.c_str(), flags, CREATE_PMODE);
	if(m_FP < 0)
	{
		int err = errno;
		if(err != 2)
		{
			err = err;
		}
	}
}
FileAutoClose::FileAutoClose(const char *path, int flags)
{
	m_FP = open(path, flags, CREATE_PMODE);
	if (m_FP < 0)
	{
		int err = errno;
		if (err != 2)
		{
			err = err;
		}
	}
}
FileAutoClose::~FileAutoClose()
{
	close();
}

bool FileAutoClose::flush()
{
	if(m_FP >= 0)
	{
		return syncFile(m_FP);
	}

	return false;
}

bool FileAutoClose::syncFile(int fp)
{
#ifdef _WIN32
	auto p = _get_osfhandle(fp);
	return FlushFileBuffers((HANDLE)p);
#else
	return fsync(m_FP);
#endif
}

void FileAutoClose::close()
{
	if(m_FP >= 0)
	{
		::close(m_FP);
		m_FP = -1;
	}
}

bool FileAutoClose::isNull()
{
	return m_FP<0;
}

bool FileAutoClose::read(void *buf, int nbytes)
{
	int n;
	if((n=::read(m_FP, buf, nbytes)) == nbytes) return true;
	else
	{
		int err = errno;
		if(err != 2)
		{
			err = err;
		}
		return false;
	}
}

bool FileAutoClose::write(const void *buf, int nbytes)
{
	int n;
	if((n=::write(m_FP, buf, nbytes)) == nbytes) 
		return true;
	else
	{
		//int err = errno;
		return false;
	}
}

int FileAutoClose::fileSize()
{
	int s = lseek(m_FP, 0, SEEK_END);
	if(s < 0) return 0;
	else return s;
}

int FileAutoClose::tell()
{
	return ::lseek(m_FP, 0, SEEK_CUR);
}

bool FileAutoClose::seek(int offset, int pos)
{
	if(lseek(m_FP, offset, pos) >= 0) 
		return true;
	else
	{
		//int err = errno;
		return false;
	}
}


FileManager::FileManager(void)
{
	this->initRootPath(NULL);
}

const char* FileManager::getFullPath(const char *filePath)
{
	m_fullPath = filePath;
	/*
	for (size_t i = 0; i < m_rootPaths.size(); i++)
	{
		std::string fullPath = m_rootPaths[i] + filePath;
		if (isFileExists(fullPath.c_str()))
		{
			m_fullPath = fullPath;
			break;
		}
	}
	
	if (m_fullPath.empty()) 
		m_fullPath = m_rootPaths[0] + filePath;
	*/
	return m_fullPath.c_str();
}

bool FileManager::isFileExists(const char *filepath)
{
	struct  stat buffer;
	return (stat(filepath, &buffer) == 0);
}

bool FileManager::initRootPath(const char *rootpath)
{
	if (rootpath != NULL)
		m_rootPaths.push_back(rootpath);

	std::string filePath = __FILE__;
	size_t findPos = filePath.find("client");
	std::string _rootPath = filePath.substr(0, findPos);

	//root write file path
	m_rootPaths.push_back(_rootPath);
	m_rootPaths.push_back(_rootPath + "bin/");
	m_rootPaths.push_back(_rootPath + "bin/res/");

	return true;
}

bool FileManager::renameFile(const char *oldname, const char *newname)
{
	for(int i=0; i<5; i++) //try 5 times
	{
		if(rename(oldname, newname) == 0) return true;

#ifdef _WIN32
		DeleteFileA(newname);
		if(rename(oldname, newname) == 0) return true;
		::Sleep(100);
#else
		::sleep(100);
#endif
	}
	return false;
}

void* FileManager::ReadWholeFile(const char *path, int &datalen)
{
	std::string fullpath = getFullPath(path);

	FileAutoClose fp(fullpath, O_RDONLY|O_BINARY);
	if (fp.isNull()) return NULL;

	datalen = fp.fileSize();
	if(datalen == 0) return NULL;

	void *buf = malloc(datalen);
	if(buf == NULL) return NULL;

	fp.seek(0, SEEK_SET);
	if(!fp.read(buf, datalen))
	{
		free(buf);
		return NULL;
	}
	return buf;
}

bool FileManager::WriteWholeFile(const char *path, const void *data, int datalen, bool safely/*=false*/)
{
	if(safely)
	{
		char tmppath[256];
		sprintf_s(tmppath, "%s.tmp", path);
		std::string fullpath = getFullPath(path);

		FileAutoClose fp(fullpath, O_CREAT|O_WRONLY|O_TRUNC|O_BINARY);
		if(fp.isNull()) return false;

		if(!fp.write(data, datalen)) return false;

		fp.flush();
		fp.close();

		this->renameFile(tmppath, path);

		return true;
	}
	else
	{
		std::string fullpath(path);

		FileAutoClose fp(fullpath, O_CREAT|O_WRONLY|O_TRUNC|O_BINARY);
		if(fp.isNull()) return false;

		return fp.write(data, datalen);
	}
}

static FileManager* s_FileManager = nullptr;
FileManager* GetFileManager()
{
	if (s_FileManager == nullptr)
	{
		s_FileManager = new FileManager();
	}
	return s_FileManager;
}
