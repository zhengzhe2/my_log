#ifndef MY_LOG_MANAGER_H
#define MY_LOG_MANAGER_H
#include "MY_Log_Proxy_Impl.h"

#include <string>
#include <map>
#include <iostream>

namespace Proxy
{

class MY_Log_Manager
{
public:
    static MY_Log_Manager *GetInstance();
    void Create(int LogType, std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize);
    void SaveLog(int LogType, const char* Buffer, int Level);
    void Destroy();
protected:
    MY_Log_Manager();
    virtual ~MY_Log_Manager();

private:
    static MY_Log_Manager *Instance;
    std::map<int, MY_Log_Proxy_Impl*> mLogMaps;
};

}  // namespace Proxy
#endif
