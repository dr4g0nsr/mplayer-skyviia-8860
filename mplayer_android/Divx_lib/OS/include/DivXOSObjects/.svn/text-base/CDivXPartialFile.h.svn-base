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

#ifndef PARTIAL_FILE_H
#define PARTIAL_FILE_H

#include "DivXInt.h"
#include "CDivXError.h"
#include "DivXWchar.h"
#include "CDivXEvent.h"
#include "CDivXMutex.h"
#include "DivXMem.h"

namespace DivXOS
{

    #define PF_CREATE       0x01
    #define PF_EXISTING     0x02

    typedef struct _PFParams
    {    
        /*! create new file or not */
        uint8_t mode;

        /*! path of status file */
        DivXString *pStatusFilePath;

        /*! path of data file */
        DivXString *pDataFilePath;

        /*! context buffer */
        void *pContext;

        /*! context size */
        uint32_t contextSize;

        /*! granularity file can be written */
        uint32_t blockSize;
    }
    PFParams;

    typedef struct _PFInfo
    {
        /*! set if eof is determined */
        bool eofSet;

        /*! granularity file can be written */
        uint32_t blockSize;

        /*! size of file */
        uint64_t currentSize;       /*!< @note this size may grow */

        /*! number of valid bytes */
        uint64_t validBytes;

        /*! context data in file */
        void *pContext;             /*!< @note mem gets dealloced on close */

        /*! context size */
        uint32_t contexSize;
    }
    PFInfo;

    typedef struct _PFStatus
    {
        /*! if file is opened */
        bool opened;

        /*! set when end of file is determined */
        bool eofSet;

        /*! set when all reads should abort */
        bool abort;

        /*! current file size */
        uint64_t currentSize;

        /*! number of valid bytes */
        uint64_t validBytes;

        /*! block size file can be written */
        uint32_t blockSize;

        /*! context data */
        void *pContext;

        /*! context size */
        uint32_t contextSize;
    }
    PFStatus;


    /*! re-entrent partial file class */
    /*! data should only be written on block bondaries accept for eof */
    class CDivXPartialFile
    {
    public:    
        
        /*! constructor */
        CDivXPartialFile(DivXMem hMem);

        /*! deconstructor */
        ~CDivXPartialFile();

        /*! open partial file */
        int OpenFile(PFParams *pParams);

        /*! close partial file */
        int CloseFile();

        /*! get current file info */
        /*! @note info will reference internal memory */
        int GetInfo(PFInfo *pInfo);

        /*! read byte range from file */
        DCDM_ERR_e Read(uint64_t pos, uint32_t len, uint8_t *pBuf, bool block);
        
        /*! abort reads, all reads will fail until resume called */
        int CancelRead();  

        /*! resume reads */
        int ResumeRead();

        /*! write byte range to partial file */
        int Write(uint64_t pos, uint32_t len, uint8_t *pBuf);     

        /*! return closes missing byte range */
        int CheckRange(uint64_t *pPos, uint64_t *pLen);   
        
        /*! set file size */
        int SetFileSize(uint64_t fileSize);

        // Moved for testing purposes
        /*! event set when data written */
        CDivXEvent writeEvent;
    private:
        
        /*! create status file */
        bool CreateStatusFile(DivXString *pFilename, PFParams *pParams);

        /*! create data file */
        bool CreateDataFile(DivXString *pFilename);

        /*! open status file */
        bool OpenStatusFile(DivXString *pFilename);

        /*! close status file */
        bool CloseStatusFile();

        /*! open data file */
        bool OpenDataFile(DivXString *pFilename);

        /*! close data file */
        bool CloseDataFile();

        /*! adjust the file size */
        bool AdjustFileSize(uint64_t fileSize);

        /*! count the number of valid bytes in mask range */
        bool ValidBytes(uint64_t startByte, uint64_t endByte, uint64_t *pBytes);

        /*! status of current file */
        struct _PFStatus status;

#ifdef _MSC_VER
        /*! handle to data file */
        void* hDataFile;

        /*! handle to status file */
        void* hStatusFile;
#else
        /*! handle to data file */
        int hDataFile;

        /*! handle to status file */
        int hStatusFile;
#endif

        /*! protect file handles */
        CDivXMutex fileMutex;

        /*! protect blocking read count */
        CDivXMutex blockMutex;

        /*! count number of blocking reads */
        uint32_t blockCount;

        /*! event set when all activated */
        CDivXEvent blockEvent;

        /*! mutex protecting info cache */
        CDivXMutex cacheMutex;

        /*! cached info */
        PFInfo cachedInfo;

        /*! opened cache */
        bool cachedOpened;

        /*! DivXMem memory handler, use NULL for maliged memory */
        DivXMem hMem;

    };
}

#endif
