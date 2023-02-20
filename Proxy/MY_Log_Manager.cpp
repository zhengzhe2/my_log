#include "MY_Log_Manager.h"


namespace Proxy
{

MY_Log_Manager *MY_Log_Manager::Instance = NULL;
MY_Log_Manager *MY_Log_Manager::GetInstance()
{
    if (Instance == NULL) {
        Instance = new MY_Log_Manager();
    }
    return Instance;
}

void MY_Log_Manager::Create(int LogType, std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize)
{
    mLogMaps[LogType] = new MY_Log_Proxy_Impl(FileName, FilePath, FileFormat, FileSize, TotalFileCount, PartitionSize, SpecificCacheSize);
}

void MY_Log_Manager::SaveLog(int LogType, const char* Buffer, int Level)
{
    MY_Log_Proxy_Impl* log_proxy_impl = mLogMaps[LogType];
    if (log_proxy_impl != NULL) {
        log_proxy_impl->saveLog(Buffer, Level);
    }         
}

void MY_Log_Manager::Destroy()
{
    delete Instance;    
    Instance = NULL;
}

MY_Log_Manager::MY_Log_Manager()
{

}

MY_Log_Manager::~MY_Log_Manager()
{
    std::map<int, MY_Log_Proxy_Impl*>::iterator it;
    for (it = mLogMaps.begin(); it != mLogMaps.end(); ++it) {
        delete it->second;
    }
    mLogMaps.clear();
}

}
/* EOF */
