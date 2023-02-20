#include "my_log_proxy_wrapper.h"
#include "MY_Log_Proxy_IF.h"

#ifdef __cplusplus
extern "C" {
#endif

void my_log_init()
{
    Proxy::MY_Log_Proxy_IF::Init();
}

void my_log_create(int LogType, const char* FileName, const char* FilePath, const char* FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize)
{
    Proxy::MY_Log_Proxy_IF::Create(LogType, FileName, FilePath, FileFormat, FileSize, TotalFileCount, PartitionSize, SpecificCacheSize);
}

void my_log_deinit()
{
    Proxy::MY_Log_Proxy_IF::Destroy();
}

void my_log_write_buffer(int LogType, const char* Buffer, int Level)
{
    Proxy::MY_Log_Proxy_IF::SaveLog(LogType, Buffer, Level);
}

#ifdef __cplusplus
};
#endif

