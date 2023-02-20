#include "MY_Log_Proxy_Impl.h"
#include <iostream>
#include <fstream>

#include <errno.h>              /* errno                */
#include <sys/types.h>          /* mkdir, stat          */
#include <math.h>               /* pow                  */
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>
#include <algorithm>
#include <string.h>
#include <functional>
#include <sstream>

#include "MY_Dirent.h"         /* readdir              */
#include "MY_Stat.h"           /* mkdir, stat          */
#include "MY_Fcntl.h"          /* open                 */

#include "zlib.h"

namespace Proxy
{
static const int COMPRESS_BUF_SIZE      = 8 KB;
static const int COMPRESS_INPUT_MAX_SIZE      = 40 MB;
static const int GZIP_WINDOWBITS        = MAX_WBITS + 16;
static const int DEF_MEM_LEVEL          = 8;

static const char* LEVEL_DEBUG_STRING = "[DEBUG]";
static const char* LEVEL_INFO_STRING = "[INFO]";
static const char* LEVEL_WARN_STRING = "[WARN]";
static const char* LEVEL_ERROR_STRING = "[ERROR]";

enum MY_LOG_LEVEL
{
    MY_LOG_LEVEL_DEBUG = 0,
    MY_LOG_LEVEL_INFO,
    MY_LOG_LEVEL_WARN,
    MY_LOG_LEVEL_ERROR
};

MY_Log_Proxy_Impl::MY_Log_Proxy_Impl(std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize):
    mFileName(FileName)
    , mFilePath(FilePath)
    , mFileFormat(FileFormat)
    , mFileMaxSize(FileSize)
    , mTotalFileCount(TotalFileCount)
    , mPartitionSize(PartitionSize)
    , mSpecificCacheSize(SpecificCacheSize)
    , mCompressBuf(NULL)
    , mCachedSize(0)
    , mFileSize(0)
    , mIsFlushCacheThreadExit(false)
{
    std::cout << "XT_Log_Proxy_Impl::MY_Log_Proxy_Impl" << std::endl;
    POSTFIX_FILE = ".gz";
    SEPARATOR_FILE = "/";
    mCompressBuf = new char[COMPRESS_BUF_SIZE];
    cfmFilePathAndOpen();
    mOfs = std::ofstream(mFilePath + mFileName + mFileFormat, std::ios::out| std::ios::app);
    mCache = new char[mSpecificCacheSize];
    mFlushCacheThread = std::thread(std::bind(&MY_Log_Proxy_Impl::flushCache, this));
}

MY_Log_Proxy_Impl::~MY_Log_Proxy_Impl()
{
    std::cout << "XT_Log_Proxy_Impl::~MY_Log_Proxy_Impl" << std::endl;
    if (nullptr != mCompressBuf) {
        delete[] mCompressBuf;
        mCompressBuf = nullptr;
    }
    mCacheMutex.lock();
    if (mCachedSize > 0) {
        mOfs.write(mCache, mCachedSize);
        mOfs.flush();
        mCachedSize = 0;
    }
    mOfs.close();
    mCacheMutex.unlock();
    mIsFlushCacheThreadExit = true;
    mFlushCacheThread.join();
    if (nullptr != mCache) {
        delete[] mCache;
        mCache = nullptr;
    }
}


void MY_Log_Proxy_Impl::cfmFilePathAndOpen()
{
    std::cout << "XT_Log_Proxy_Impl::cfmFilePathAndOpen" << std::endl;
    // 1. check dir, if dir not exist, create them 
    DIR *dirFirst = opendir(mFilePath.c_str());
    if (!dirFirst) {
        int err = errno;
        if ((ENOENT == err) || (ENOTDIR == err)) {
            if (ENOTDIR == err) { // Not a directory, maybe a file.
                remove(mFilePath.c_str()); // remove the file, which should be a dir.
            }
            // create the dir.
            int size = mFilePath.size();
            for (int i = 0; i < size; ++i) {
                std::size_t pos = mFilePath.find(SEPARATOR_FILE, i);
                if (std::string::npos != pos) {
                    std::string substr = mFilePath.substr(0, pos);
                    mkdir(substr.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
                    i = pos;
                }
                else {
                    break;
                }
            }
            // open again.
            dirFirst = opendir(mFilePath.c_str());
            if (!dirFirst) {
                std::cout<<"Failed to open log dir again \""<< mFilePath <<"\", errno = "<< errno <<" "<< strerror(errno) <<"\n";
                return;
            }
        }
        else {
            std::cout<<"Failed to open log dir \""<< mFilePath <<"\", errno = "<< errno <<" "<< strerror(errno) <<"\n";
            return;
        }
    }

    // 2. read dir, get all log file info, delete old files if necessary
    std::map<time_t, std::string> tempfileNameList;
    std::string tempfilePath = mFilePath;
    int tempfileCount = 0;
    off_t tempfileSize = 0;
    for (struct dirent *entry = readdir(dirFirst); NULL != entry; entry = readdir(dirFirst)) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        std::string temp = entry->d_name;
        std::string tempPathName = tempfilePath + temp;
        // add file name to list
        struct stat statBuf;
        if (0 == lstat(tempPathName.c_str(), &statBuf)) {
            tempfileCount++;
            tempfileSize += statBuf.st_size;
            // sort logfile by index
            tempfileNameList.insert(std::pair<time_t, std::string>(statBuf.st_mtime, tempPathName));
            // if the number of files exceeds the maximum, remove old file
            if ((tempfileCount >= mTotalFileCount) || ((mPartitionSize - tempfileSize) < mFileMaxSize))
            {
                // remove old file
                std::map<time_t, std::string>::iterator it = tempfileNameList.begin();
                struct stat tempstatBuf;
                if (0 == lstat(it->second.c_str(), &tempstatBuf)) {
                    tempfileSize -= tempstatBuf.st_size;
                    tempfileCount--;
                    remove(it->second.c_str());
                    tempfileNameList.erase(it);
                }
                else {
                    std::cout<<"check file status error, file name:"<<it->second<<std::endl;
                }
            }
            
        }
    }

    if (tempfileNameList.size() != 0) {
        std::map<time_t, std::string>::reverse_iterator lastIt = tempfileNameList.rbegin();
        std::string lastFileName = lastIt->second;
        struct stat lastFileStatBuf;
        if (0 == lstat(lastIt->second.c_str(), &lastFileStatBuf)) {
            std::stringstream fileModifyTime;
            fileModifyTime << lastFileStatBuf.st_mtime;
            std::string tempFileModifyTime;
            fileModifyTime >> tempFileModifyTime;
            if (lastFileStatBuf.st_size >= mFileMaxSize && lastFileName.find(POSTFIX_FILE) == lastFileName.npos) {
                std::string tempFileName = lastFileName;
                std::string outputFileName = tempFileName.substr(0, tempFileName.find(mFileFormat));
                outputFileName.append(".").append(tempFileModifyTime).append(mFileFormat).append(".gz");
                compress2gz(lastFileName, outputFileName);
            }
        }
    }
    closedir(dirFirst);
}

void MY_Log_Proxy_Impl::compress2gz(std::string fileName, std::string outputFileName) {

    clock_t before = clock();
    std::string input = fileName;
    std::string output = outputFileName;

	std::ifstream fin;
	std::ofstream fwr;
 
	int errComp;
	unsigned long file_len_src;
    unsigned long len_tmp;
 
	fin.open(input.c_str(), std::ios_base::in | std::ios_base::binary); 

    fwr.open(output.c_str(), std::ios_base::out | std::ios_base::binary);
 
	fin.seekg(0, std::ios::end);
 
	file_len_src = fin.tellg();//获取原文件大小
//    std::cout<<"\n source file size:"<<file_len_src<<"\n";
 
    fin.seekg(0,std::ios::beg);

    z_stream c_stream;
    memset(&c_stream, 0, sizeof(c_stream));

    int ret = -1;
    int totalSize = 0;

    c_stream.zalloc = NULL;
    c_stream.zfree = NULL;
    c_stream.opaque = NULL;

    // Initialisation, GZIP will be created
    if (deflateInit2(&c_stream, Z_BEST_SPEED, Z_DEFLATED, GZIP_WINDOWBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY) == Z_OK) {
        int needRead = std::min(file_len_src, (unsigned long) COMPRESS_INPUT_MAX_SIZE);
        char* source = new char[needRead];
        unsigned long alreadyRead = 0;
        while (needRead > 0) {
            fin.read((char *)source, needRead);//读取压缩文件内容
            // on some platform (ex.: OSX), the builtin zlib implementation does not have const constraint declared.
            c_stream.next_in  = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(source));
            c_stream.avail_in = needRead;

            // Finish the stream
            do {
                c_stream.avail_out = COMPRESS_BUF_SIZE;
                c_stream.next_out = reinterpret_cast<Bytef*>(mCompressBuf);
                ret = deflate(&c_stream, Z_FINISH);
                if ((Z_STREAM_END != ret) && (Z_OK != ret)) {
                    std::cout<<"\n gzip compress failed"<<ret<<"\n";
                    break;
                }
                int have = COMPRESS_BUF_SIZE - c_stream.avail_out;
                fwr.write(mCompressBuf, have);

                // if (-1 == written) {
                //     std::cout<<"\n Failed to write data to file.\n";
                //     break;
                // }
                // else {
                //     totalSize += written;
                // }
            } while (Z_OK == ret);
            alreadyRead += needRead;
            needRead = std::min(file_len_src - alreadyRead, (unsigned long) COMPRESS_INPUT_MAX_SIZE);
        }
        delete[] source;
        deflateEnd(&c_stream);
    }
	fin.close();
	fwr.close();

    clock_t after = clock();
    remove(fileName.c_str());
    std::cout<< "[performance] compress file["<<output<<"] source size["<<(double)file_len_src/1024/1024<<"M] takes "<<(double)(after - before) / 1000<<" ms.\n";
    return ;
}

void MY_Log_Proxy_Impl::saveLog(const char* buffer, int level)
{
    std::cout << "XT_Log_Proxy_Impl::saveLog" << std::endl;
    mCacheMutex.lock();
    std::string content = buffer;
    content.append("\n");
    updateLogByLevel(content, level);
    if (mCachedSize + content.size() >= mSpecificCacheSize) {
        std::cout << "XT_Log_Proxy_Impl::saveLog flush cacheSize:" << mCachedSize << " bufferSize:" << content.size() << " mSpecificCacheSize:" << mSpecificCacheSize << std::endl;
        mOfs.write(mCache, mCachedSize);
        mOfs << content;
        mOfs.flush();
        mFileSize += mCachedSize + content.size();
        mCachedSize = 0;
    } else {
        std::cout << "XT_Log_Proxy_Impl::saveLog add cache" << std::endl;
        strncpy(mCache + mCachedSize, content.c_str(), content.size());
        mCachedSize += content.size();
    }
    if (mFileSize >= mFileMaxSize) {
        std::cout << "XT_Log_Proxy_Impl::saveLog save file" << std::endl;
        mOfs.close();
        cfmFilePathAndOpen();
        mFileSize = 0;
        mOfs = std::ofstream(mFilePath + mFileName + mFileFormat, std::ios::out| std::ios::app);
    }
    mCacheMutex.unlock();
}

void MY_Log_Proxy_Impl::flushCache()
{
    while (!mIsFlushCacheThreadExit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "XT_Log_Proxy_Impl::flushCache " << std::endl;
        if (mCacheMutex.try_lock()) {
            if (mCachedSize > 0) {
                mOfs.write(mCache, mCachedSize);
                mOfs.flush();
                mFileSize += mCachedSize;
                mCachedSize = 0;
            }
            mCacheMutex.unlock();
        }
    }
}

void MY_Log_Proxy_Impl::updateLogByLevel(std::string& log, int level)
{
    switch (level)
    {
    case Proxy::MY_LOG_LEVEL::MY_LOG_LEVEL_DEBUG:
        log.insert(0, LEVEL_DEBUG_STRING);
        break;
    case Proxy::MY_LOG_LEVEL::MY_LOG_LEVEL_INFO:
        log.insert(0, LEVEL_INFO_STRING);
        break;
    case Proxy::MY_LOG_LEVEL::MY_LOG_LEVEL_WARN:
        log.insert(0, LEVEL_WARN_STRING);
        break;
    case Proxy::MY_LOG_LEVEL::MY_LOG_LEVEL_ERROR:
        log.insert(0, LEVEL_ERROR_STRING);
        break;
    default:
        log.insert(0, LEVEL_DEBUG_STRING);
        break;
    }
}

}
/* EOF */
