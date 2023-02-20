/**
 * @file MY_Fcntl.h
 * @brief wrapper for the C standard header file "fcntl.h"
 *
 * Handle compatibility issues over compilers, especially the brain-dead MSVC.
 */
#ifndef MY_FCNTL_H
#define MY_FCNTL_H

#include <fcntl.h>
#ifdef _MSC_VER
#   include <io.h>      /* open     */
#endif

#ifdef _MSC_VER

#define O_APPEND    _O_APPEND
#define O_CREAT     _O_CREAT
#define O_EXCL      _O_EXCL
#define O_TRUNC     _O_TRUNC

#define O_RDONLY    _O_RDONLY
#define O_WRONLY    _O_WRONLY
#define O_RDWR      _O_RDWR

#define S_IRUSR     _S_IREAD
#define S_IWUSR     _S_IWRITE
#define S_IXUSR     0

#endif  // _MSC_VER

#endif  // MY_FCNTL_H
/* EOF */
