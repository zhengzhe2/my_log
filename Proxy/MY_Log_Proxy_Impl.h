
/**
 * @file XT_Log_Impl.h
 * @brief Definition of class XT_Log_Impl.
 *
 * Implementation of interface XT_Log_Impl which output log to file(s).
 */
#ifndef MY_LOG_PROXY_IMPL_H
#define MY_LOG_PROXY_IMPL_H
#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <atomic>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include <fstream>

#define KB  * 1024
#define MB  * 1024 * 1024

namespace Proxy
{

class MY_Log_Proxy_Impl
{
public:
    MY_Log_Proxy_Impl(std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize);
    virtual ~MY_Log_Proxy_Impl();
    void cfmFilePathAndOpen();
    void compress2gz(std::string fileName, std::string outputFileName);
    void saveLog(const char* buffer, int level);
    void flushCache();
    void updateLogByLevel(std::string& log, int level);

private:
    std::string mFileName;
    std::string mFilePath;
    std::string mFileFormat;
    int mFileMaxSize;
    int mTotalFileCount;
    int mPartitionSize;
    int mSpecificCacheSize;
    bool mIsFlushCacheThreadExit;
    std::string POSTFIX_FILE;
    std::string SEPARATOR_FILE;
    std::mutex mCacheMutex;
    int mCachedSize;
    int mFileSize;
    char* mCache = nullptr;
    char* mCompressBuf;
    std::ofstream mOfs;
    std::thread mFlushCacheThread;
};

}

#endif  // MY_LOG_PROXY_IMPL_H
/* EOF */
