/*
 *  ObfuscatedZip.h
 */

#ifndef obfuscated_zip_h
#define obfuscated_zip_h

#include "OgreArchive.h"
#include "OgreArchiveFactory.h"

// Forward declaration for zziplib to avoid header file dependency.
typedef struct zzip_dir ZZIP_DIR;
typedef struct zzip_file ZZIP_FILE;

/** Specialization of the Archive class to allow reading of files from an
    obfuscated zip format source archive.
@remarks
    This archive format supports obfuscated zip archives.
*/
class ObfuscatedZip : public Ogre::Archive
{
protected:
    /// Handle to root zip file
    ZZIP_DIR* mZzipDir;
    /// Handle any errors from zzip
    void checkZzipError(int zzipError, const Ogre::String& operation) const;
    /// File list (since zziplib seems to only allow scanning of dir tree once)
    Ogre::FileInfoList mFileList;

public:
    ObfuscatedZip(const Ogre::String& name, const Ogre::String& archType );
    ~ObfuscatedZip();
    /// @copydoc Archive::isCaseSensitive
    bool isCaseSensitive(void) const { return false; }

    /// @copydoc Archive::load
    void load();
    /// @copydoc Archive::unload
    void unload();

    /// @copydoc Archive::open
    Ogre::DataStreamPtr open(const Ogre::String& filename, bool readOnly = true) const;

    /// @copydoc Archive::list
    Ogre::StringVectorPtr list(bool recursive = true, bool dirs = false);

    /// @copydoc Archive::listFileInfo
    Ogre::FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);

    /// @copydoc Archive::find
    Ogre::StringVectorPtr find(const Ogre::String& pattern, bool recursive = true,
        bool dirs = false);

    /// @copydoc Archive::findFileInfo
    Ogre::FileInfoListPtr findFileInfo(const Ogre::String& pattern, bool recursive = true,
        bool dirs = false) const;

    /// @copydoc Archive::exists
    bool exists(const Ogre::String& filename);

    /// @copydoc Archive::getModifiedTime
    time_t getModifiedTime(const Ogre::String& filename);
};

/** Specialization of ArchiveFactory for Obfuscated Zip files. */
class ObfuscatedZipFactory : public Ogre::ArchiveFactory
{
public:
    virtual ~ObfuscatedZipFactory() {}
    /// @copydoc FactoryObj::getType
    const Ogre::String& getType(void) const;
    /// @copydoc FactoryObj::createInstance
    Ogre::Archive *createInstance( const Ogre::String& name, bool readOnly )
    {
        return new ObfuscatedZip(name, "Obf");
    }
    /// @copydoc FactoryObj::destroyInstance
    void destroyInstance( Ogre::Archive* arch) { OGRE_DELETE arch; }
};

/** Specialization of DataStream to handle streaming data from zip archives. */
class ObfuscatedZipDataStream : public Ogre::DataStream
{
protected:
    ZZIP_FILE* mZzipFile;
public:
    /// Unnamed constructor
    ObfuscatedZipDataStream(ZZIP_FILE* zzipFile, size_t uncompressedSize);
    /// Constructor for creating named streams
    ObfuscatedZipDataStream(const Ogre::String& name, ZZIP_FILE* zzipFile, size_t uncompressedSize);
    ~ObfuscatedZipDataStream();
    /// @copydoc DataStream::read
    size_t read(void* buf, size_t count);
    /// @copydoc DataStream::skip
    void skip(long count);
    /// @copydoc DataStream::seek
    void seek( size_t pos );
    /// @copydoc DataStream::seek
    size_t tell(void) const;
    /// @copydoc DataStream::eof
    bool eof(void) const;
    /// @copydoc DataStream::close
    void close(void);
};

#endif  // obfuscated_zip_h
