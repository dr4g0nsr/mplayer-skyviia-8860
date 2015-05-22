/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#ifndef SPARSEFILE_H
#define SPARSEFILE_H

#include "DivXInt.h"
#include "DivXWchar.h"
#include "DivXDbg.h"
#include "MemoryManager.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! opaque handle defined internally */
typedef struct _SparseFile *SparseFile;

/*! define for handle size, update manually if changes */
#define SPARSE_FILE_HANDLE_SIZE(MAXURI) (128 + MAXURI * sizeof(wchar_t))

typedef enum _SFError
{
    SFOK,
    SFFail,
    SFBadParam,
    SFEndFile,
}
SFError;

/*! seek modes */
typedef enum _SFSeekMode
{
    SFRelativeForward,      //! offset from current position forward
    SFRelativeBackward,     //! offset from current position in reverse
    SFFromStart,            //! offset from start
    SFFromEnd,              //! offset in reverse from end
}
SFSeekMode;

/*! mask values for open file */
#define SF_SPARSE_FILE  0x80
#define SF_CREATE_FILE  0x40    //if not specified, file will not be created
#define SF_READ_FILE    0x10    //open file for reading
#define SF_WRITE_FILE   0x08    //open file for writing

typedef struct _SFInfo
{
    uint32_t fileSizeHigh;      //! high 32bits of file size
    uint32_t fileSizeLow;       //! low 32bits of file size
    uint32_t lastAccess;        //! last access in seconds
}
SFInfo;

/*! open binary file with sparse option */
SparseFile SFOpenFile(const wchar_t *uri, uint8_t modeMask, MemoryManager memory);

/*! close binary file */
void SFCloseFile(SparseFile file);

/*! seek in file */
SFError SFSetPosition(SparseFile file, uint32_t highPos, uint32_t lowPos, SFSeekMode mode);

/*! get current file position */
SFError SFGetPosition(SparseFile file, uint32_t *highPos, uint32_t *lowPos);

/*! read data from file */
SFError SFReadData(SparseFile file, void *buf, uint32_t *size);

/*! read data from file */
SFError SFWriteData(SparseFile file, const void *buf, uint32_t *size);

/*! get current file info */
SFError SFGetInfo(SparseFile file, SFInfo *info);

/*! get specified file info */
SFError SFGetFileInfo(const wchar_t *uri, SFInfo *info);

/*! delete specified file */
SFError SFDeleteFile(const wchar_t *uri);

/*! move file from src path to dst path */
SFError SFMoveFile(const wchar_t *src, const wchar_t *dst);

#ifdef __cplusplus
}
#endif

#endif
