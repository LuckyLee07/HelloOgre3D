#ifndef __FILE_MANANGER_H__
#define __FILE_MANANGER_H__

#include <string>
#include <vector>

#include "Singleton.h"

// 文件操作管理

class FileAutoClose
{
public:
	FileAutoClose(const std::string &path, int flags);
	FileAutoClose(const char *path, int flags);
	~FileAutoClose();

	bool flush();
	void close();
	bool isNull();
	bool read(void *buf, int nbytes);
	bool write(const void *buf, int nbytes);
	int fileSize();
	int tell();
	bool seek(int offset, int pos);
	bool syncFile(int fp);

private:
	int m_FP;
};

class FileManager : public Singleton<FileManager>
{
public:
	FileManager(void);
	~FileManager() { }
	
	const char* getFullPath(const char *filePath);
	bool isFileExists(const char *filepath);

	bool initRootPath(const char *rootpath = NULL);
	bool renameFile(const char *oldname, const char *newname);
	
	void* ReadWholeFile(const char *path, int &datalen);
	bool WriteWholeFile(const char *path, const void *data, int datalen, bool safely=false);

private:
	std::string m_fullPath;
	std::vector<std::string> m_rootPaths;
};

FileManager* GetFileManager();

#endif //__FILE_MANANGER_H__
