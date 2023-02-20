
/**
 * @file MY_Dirent.h
 * @brief wrapper for the C standard header file "dirent.h"
 *
 * Handle compatibility issues over compilers, especially the brain-dead MSVC.
 */
#ifndef MY_DIRENT_H
#define MY_DIRENT_H

// Do not use the partial implementation from MinGW
#if !defined(_MSC_VER) && !defined(__MINGW32__)
#   include <dirent.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)

#include <windows.h>

#define DT_REG 1
#define DT_DIR 2

struct dirent
{
    int    d_type;
    char * d_name;
};

typedef struct _DIR
{
    HANDLE              hFind;
    WIN32_FIND_DATA     findData;
    dirent              ent;
} DIR;

inline DIR *opendir(const char *name)
{
    DIR* dir = new(MEM_Base) DIR();
    if (NULL == dir) {
        return NULL;
    }

    int length = strlen(name);
    char* pattern = new(MEM_Base) char[length + 3];
    if (NULL == pattern) {
        return NULL;
    }

    memcpy(pattern, name, length);
    if ('/' == pattern[length - 1] || '\\' == pattern[length - 1]) {
        pattern[length] = '*';
        pattern[length + 1] = '\0';
    }
    else {
        pattern[length] = '/';
        pattern[length + 1] = '*';
        pattern[length + 2] = '\0';
    }

    dir->hFind = FindFirstFile(pattern, &(dir->findData));
    if (INVALID_HANDLE_VALUE == dir->hFind) {
        delete dir;
        dir = NULL;
    }

    delete[] pattern;
    pattern = NULL;

    return dir;
}

inline struct dirent *readdir(DIR *dirp)
{
    if (NULL == dirp) {
        return NULL;
    }

    if (FindNextFile(dirp->hFind, &(dirp->findData))) {
        dirp->ent.d_name = dirp->findData.cFileName;
        if (dirp->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            dirp->ent.d_type = DT_DIR;
        }
        else {
            dirp->ent.d_type = DT_REG;
        }
        return &(dirp->ent);
    }

    return NULL;
}

int closedir(DIR *dirp)
{
    if (dirp) {
        FindClose(dirp->hFind);
        delete dirp;
        return 0;
    }

    return -1;
}

#endif  // _MSC_VER

#endif  // MY_DIRENT_H
/* EOF */
