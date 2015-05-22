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

#ifndef DFILE_H
#define DFILE_H

#include "CDivXPartialFile.h"
#include "CDivXRemoteFile.h"
#include "CAutoMutexRW.h"

#include "CDivXHttpAuth.h"
#include "CDivXHttpProxy.h"

#include "DivXMem.h"
#include "CDivXError.h"

#include <queue>

namespace DivXOS
{

    #define DF_EXISTING             0x01
    #define DF_CREATE               0x02

    #define DF_MAX_CONTEXT_SIZE     1000000

    /*! \} */

    /*! \name Download Manager

      @brief Download Manager Initialization Options

      This structure is used in DownloadManagerCreate()

      */

    typedef struct _DCDMParams
    {
        /*! pointer to memory pool */
        void *pMemoryPool;
        /*! size of memory pool */
        uint32_t memoryPoolSize;
        /*! NULL terminated path to temp directory for downloading files */
        DivXString *pResourcePath;
        /*! number of allowable cached files with NULL output */
        uint32_t cachedNullCount;
    }
    DCDMParams;

    /*! 

      @brief Download Manager In Progress File List

      This structure lists out all unfinished downloads

      */

    typedef struct _DCDMFileList
    {
        /*! number of in progress downloads */
        uint32_t num;
        /*! point to file entries*/
        struct _DCDMFileListEntry *pEntry;
    }
    DCDMFileList;

    /*! 
      
      @brief Download Manager file entry 

      */

    typedef struct _DCDMFileListEntry
    {
        /*! title of download */
        DivXString *pTitle;
        /*! url of file */
        DivXString *pUrl;
        /*! output path */
        DivXString *pOutputPath;
        /*! thumbnail url */
        DivXString *pThumbUrl;
        /*! description of file */
        DivXString *pDescription;
        /*! set if is opened */
        DivXBool opened;
        /*! set if download is completed */
        DivXBool complete;
        /*! number of seconds since last accessed */
        uint32_t lastAccess;
        /*! download bitrate (kbps) */
        uint32_t bitrate;
        /*! file size */
        uint64_t fileSize;
        /*! number of bytes downloaded */
        uint64_t downloadSize;
    }
    DCDMFileListEntry;

    /*! 

      @brief Download Manager File Options Parameters

      This structure specifies downloading file options
      
      @note pOvsServer, slot, and TixBuffer are used only if pURL is NULL
      pLogin and pPassword will be used only if both are not NULL
      slot will be used only if all player slots are occupied

      @note If data field is not used, set equal to null
            Slot will likely not be implemented

      */

    typedef struct _DCDMFileParams
    {
        /*! NULL terminated file title */
        DivXString *pTitle;
        /*! NULL terminated URL for file */
        DivXString *pUrl;
        /*! NULL terminated path to output file */
        DivXString *pOutputPath;
        /*! NULL terminated URL for thumbnail */
        DivXString *pThumbUrl;
        /*! NULL terminated description */
        DivXString *pDescription;
        /*! NULL terminated DivX Login */
        DivXString *pLogin;
        /*! NULL terminated DivX Password */
        DivXString *pPassword;
        /*! pointer to tix buffer */
        /*! field ignored if pUrl not NULL */
        uint8_t *pTicket;
        /*! size of tix buffer */
        uint32_t ticketSize;
        /*! ovs server name */
        DivXString *pOvsServer;
    }
    DCDMFileParams;

    /*! 

      @brief Download Manager File Information 

      This structure specifies downloading file info

      */

    typedef struct _DCDMFileInfo
    {
        /*! downlod state of file */
        uint8_t state;
        /*! set if supporting request queue */
        uint8_t byteRanges;
        /*! current estimate of bandwidth, kbps */
        uint32_t bitrate;
        /*! if file size determined */
        uint8_t eofSet;
        /*! total file size */
        uint64_t currentSize;
        /*! bytes downloaded */
        uint64_t downloadSize;
        /*! set when requests are empty */
        DivXBool requestEmpty;
    }
    DCDMFileInfo;




    typedef struct _DFParams
    {
        /*! set to existing if already opened */
        uint8_t mode;

        /*! url of remote file */
        DivXString *pUrl;

        /*! divx login */
        DivXString *pLogin;

        /*! divx password */
        DivXString *pPassword;

        /*! HTTP Authorization object, will deprecate password and login */
        CDivXHttpAuth* pAuth;

        /*! HTTP Proxy Object, will allow for proxy usage */
        CDivXHttpProxy* pProxy;

        /*! drm header */
        uint8_t *pHeader;

        /*! drm header size */
        uint32_t headerSize;

        /*! output file path */
        DivXString *pOutputPath;

        /*! url of thumbnail */
        DivXString *pThumbUrl;

        /*! description of file */
        DivXString *pDescription;

        /*! title of file */
        DivXString *pTitle;

        /*! temp data path */
        DivXString *pDataPath;

        /*! temp status file path */
        DivXString *pStatusPath;
    }
    DFParams;

    typedef struct _DFInfo
    {
        /*! state of file */
        uint8_t state;

        /*! url of remote file */
        DivXString *pUrl;

        /*! divx login */
        DivXString *pLogin;

        /*! path for output file */
        DivXString *pOutputPath;

        /*! path to temp status file */
        DivXString *pStatusPath;

        /*! path to temp data file */
        DivXString *pDataPath;

        /*! url of remote thumbnail */
        DivXString *pThumbUrl;

        /*! description of file */
        DivXString *pDescription;

        /*! title of file */
        DivXString *pTitle;

        /*! set if eof has been set */
        bool eofSet;

        /*! if there are not pending requests */
        DivXBool requestEmpty;

        /*! set if supporting request queue */
        uint8_t byteRanges;

        /*! current size of file */
        uint64_t currentSize;

        /*! size downloaded */
        uint64_t validSize;

        /*! current download bitrate (kbps) */
        uint32_t bitrate;
    }
    DFInfo;

    typedef struct _DFContextData
    {
        /*! url of remote file */
        DivXString *pUrl;

        /*! url of thumbnail */
        DivXString *pThumbUrl;

        /*! description of file */
        DivXString *pDescription;

        /*! title of file */
        DivXString *pTitle;

        /*! modified timestamp of remote file */
        void *pTimestamp;

        /*! size of timestamp buffer */
        uint32_t timestampSize;

        /*! output path of remote file */
        DivXString *pOutputPath;

        /*! temp data file path */
        DivXString *pDataPath;
    }
    DFContextData;

    typedef struct _DFContextBuffer
    {
        /*! size of context data */
        uint32_t size;

        /*! buffer holding context data */
        uint8_t *pData;
    }
    DFContextBuffer;

    typedef struct _DFEvent
    {
        /*! event type */
        uint8_t event;

        /*! data range from remote file */
        RFData data;
    }
    DFEvent;

    typedef struct _DFStatus
    {
        /*! state of remote and partial file */
        uint8_t state;

        /*! set if download is closed */
        bool remoteClosed;

        /*! set of partial is closed */
        bool partialWritten;
        
        /*! copy of url */
        DivXString *pUrl;

        /*! copy of thumb url */
        DivXString *pThumbUrl;

        /*! copy of file description */
        DivXString *pDescription;

        /*! copy of file title */
        DivXString *pTitle;

        /*! copy of login */
        DivXString *pLogin;

        /*! copy of password */
        DivXString *pPassword;

        /*! output file path */
        DivXString *pOutputPath;

        /*! copy of data path */
        DivXString *pDataPath;

        /*! copy of status path */
        DivXString *pStatusPath;
    }
    DFStatus;

    class CDivXHttpFile
    {
    public:

        /*! constructor */
        CDivXHttpFile(DivXMem hMem);

        /*! deconstructor */
        virtual ~CDivXHttpFile();

        /*! Initializes the sockets */
        DivXError Init(void);

        /*! DeInitializes the sockets */
        DivXError DeInit(void);

        /*! get info on downloading file */
        int GetInfo(DFInfo *pInfo);

        /*! open file for reading */
        int OpenFile(DFParams *pParams);

        /*! close opened file */
        int CloseFile();

        /*! request data */
        DCDM_ERR_e Request(uint64_t pos, uint64_t len, bool block);

        /*! read data */
        DCDM_ERR_e ReadData(uint64_t pos, uint32_t len, uint8_t *pBuf, bool block);

        /*! cancel all reads */
        int CancelRead();

        /*! allow reads */
        int ResumeRead();

        /*! discard all pending requests */
        /*! @note, any pending read will abort */
        int Flush();

        /*! check if byte range has been downloaded */
        int CheckDownloadedRange(uint64_t *pPos, uint64_t *pLen);
        
        /*! get context data from un-opened file */
        int GetFileContext(DivXString *pFilename, DFContextData **ppContext, DivXMem hMem);

        /*! release context data */
        void ReleaseContext(DFContextData *pContextData, DivXMem hMem);

        /*! count number of bytes to be downloaded */
        int CheckComplete(DivXString *pStatusFile, DivXString *pDataFile, uint64_t *pFileSize, uint64_t *pDownloadSize, DivXMem hMem);

        /*! post event from remote file */
        int PostEvent(uint8_t event, RFData *pData);

        /*! accessor for address of % downloaded */
        int8_t GetProgress();

    private:

        /*! Primary media controller thread */
        THREAD_FUNC_FWD(CDivXHttpFile, ThreadFuncFwd, EventThread);
            
        /*! handle events from remote file */
        void EventThread(bool *pStopFlag, void* pContext, DivXMem hMem);

        /*! get context data */
        static int GetContextData(DFContextBuffer *pContextBuffer, DFContextData **ppContextData, DivXMem hMem);

        /*! get context buffer */
        static int GetContextBuffer(DFContextData *pContextData, DFContextBuffer **ppContextBuffer, DivXMem hMem);

        /*! release context buffer */
        static void ReleaseContextBuffer(DFContextBuffer *pContextBuffer, DivXMem hMem);

        /*! check range within queue */
        void CheckRange(uint64_t *pPos, uint64_t *pLen);

        /*! set error state */
        void SetError();

        /*! write drm header */
        int WriteHeader(uint8_t *pHeader, uint32_t headerSize);

        /*! write string to file */
        int WriteBytes(uint64_t pos, uint8_t *pHeader, uint32_t headerSize);

        /*! remote file pointer */
        CDivXRemoteFile remoteFile;

        /*! partial file pointer */
        CDivXPartialFile partialFile;

        /*! protect file stat info */
        CDivXMutex statusMutex;

        /*! serialize requests */
        CDivXMutex requestMutex;

        /*! status of divx download file */
        DFStatus status;

        /*! serialize public api calls */
        CDivXMutexRW serialMutex;

        /*! set if requested range is generated internally */
        DivXBool guessRange;

        /*! serialize canceling and resuming reads */
        CDivXMutex cancelMutex;

        /*! count number of pending reads */
        CDivXMutex readMutex;
        
        /*! event set when flushing finished */
        CDivXEvent readEvent;

        /*! number of pending reads */
        uint32_t readCount;

        /*! set when reads should abort */
        DivXBool abort;

        /*! automated download position */
        uint64_t startPos;

        /*! number of bytes downloaded */
        int8_t progress;

        /*! Memory handler, use NULL for default */
        DivXMem hMem;
    };
}

#endif
