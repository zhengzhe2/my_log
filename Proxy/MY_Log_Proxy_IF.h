#ifndef MY_Log_Proxy_IF_H
#define MY_Log_Proxy_IF_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>
#include <functional>


namespace Proxy
{

class MY_Log_Proxy_IF
{
public:
    static void Init();
    static void Create(int LogType, std::string FileName, std::string FilePath, std::string FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize);
    static void Destroy();
    static void SaveLog(int LogType, const char* Buffer, int Level);
};

}  // namespace Proxy
#endif
