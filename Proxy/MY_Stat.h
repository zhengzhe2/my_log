/**
 * @file MY_Stat.h
 * @brief wrapper for the C standard header file "sys/stat.h"
 *
 * Handle compatibility issues over compilers, especially the brain-dead MSVC.
 */
#ifndef MY_STAT_H
#define MY_STAT_H

#include <sys/stat.h>

#ifdef _MSC_VER
#   include <direct.h>          /* mkdir            */
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)

#define stat _stat

#ifndef __MINGW32__

inline int mkdir(const char *dirname)
{
    return _mkdir(dirname);
}

#endif

inline int mkdir(const char *dirname, int mode)
{
#ifdef _MSC_VER
    return _mkdir(dirname);
#else
    return mkdir(dirname);
#endif
}

inline int lstat(const char *path, struct _stat *buffer)
{
    return _stat(path, buffer);
}

inline int fstat(int fd, struct _stat *buffer)
{
    return _fstat(fd, buffer);
}

#endif  // _MSC_VER

#endif  // MY_STAT_H
/* EOF */
