#include "MY_Log_Proxy_IF.h"
#include "MY_Log_Manager.h"

namespace Proxy
{

void MY_Log_Proxy_IF::Init()
{
    Proxy::MY_Log_Manager::GetInstance();
}

void MY_Log_Proxy_IF::Create(int LogType, std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize)
{
    Proxy::MY_Log_Manager::GetInstance()->Create(LogType, FileName, FilePath, FileFormat, FileSize, TotalFileCount, PartitionSize, SpecificCacheSize);
}

void MY_Log_Proxy_IF::Destroy()
{
    Proxy::MY_Log_Manager::GetInstance()->Destroy();
}

void MY_Log_Proxy_IF::SaveLog(int LogType, const char* Buffer, int Level)
{
    Proxy::MY_Log_Manager::GetInstance()->SaveLog(LogType, Buffer, Level);    
}

}
