#ifndef MY_LOG_PROXY_WRAPPER_H
#define MY_LOG_PROXY_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

extern void my_log_init();
extern void my_log_create(int LogType, const char* FileName, const char* FilePath, const char* FileFormat, int FileSize, int TotalFileCount, int PartitionSize, int SpecificCacheSize);
extern void my_log_deinit();
extern void my_log_write_buffer(int LogType, const char* Buffer, int Level);


#ifdef __cplusplus
};
#endif


#endif