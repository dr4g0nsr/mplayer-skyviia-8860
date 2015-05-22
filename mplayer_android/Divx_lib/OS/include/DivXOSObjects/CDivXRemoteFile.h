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

#ifndef REMOTE_FILE_H
#define REMOTE_FILE_H

#include "DivXInt.h"
#include "DivXWchar.h"
#include "DivXBool.h"
#include "DivXNULL.h"
#include "CDivXMutex.h"
#include "CDivXEvent.h"
#include "CDivXThread.h"
#include "CDivXTcpConn.h"

#include <list>

namespace DivXOS
{

    #define RF_EVENT_DATA           0x01    /*!< valid data has been received */
    #define RF_EVENT_DISCONN        0x02    /*!< remote connection failed */
    #define RF_EVENT_AUTHEXP        0x03    /*!< authentication expired */
    #define RF_EVENT_EMPTY          0x04    /*!< notify app the request queue is empty */
    #define RF_EVENT_EXIT           0x05    /*!< recv/send thread exit on error*/

    #define RF_CONNECTIONS          5       /*!< max number of allowable simultaneous connections */

    typedef struct _RFData
    {
        /*! start position of byte range */
        uint64_t pos;

        /*! length of byte range */
        uint32_t len;

        /*! pointer to data buffer */
        uint8_t *pBuf;
    }
    RFData;

    typedef struct _RFParams
    {
        /*! url of remote file */
        DivXString *pUrl;

        /*! login for remote access */
        DivXString *pLogin;

        /*! password for remote access */
        DivXString *pPassword;

        /*! granularity of requests */
        uint32_t blockSize;
    }
    RFParams;

    typedef struct _RFRange
    {
        /*! byte range pos */
        uint64_t pos;

        /*! byte range length */
        uint64_t len;
    }
    RFRange;

    typedef struct _RFInfo
    {
        /*! validating timestamp */
        void *pTimestamp;

        /*! size of timestamp */
        uint32_t timestampSize;

        /*! current determined file length */
        uint64_t fileSize;

        /*! download bitrate estimate in kbps */
        uint32_t bitrate;

        /*! active state */
        uint8_t state;

        /*! set to non-zero if byte ranges are requestable */
        uint8_t byteRanges;
    }
    RFInfo;

    enum RFConnState 
    { 
            Closed,     /*!< connection closed */
            Closing,    /*!< received a closing header, but not all payload */
            Ready,      /*!< either empty, or pending header received */
            Waiting     /*!< waiting to receive header */
    };

    typedef struct _RFBuf
    {

        /*! receive buffer */
        uint8_t *pBuf;

        /*! receive buffer size */
        uint64_t bufSize;

        /*! first valid byte in buffer */
        uint64_t bufStart;

        /*! first invalid byte in buffer */
        uint64_t bufEnd;

        /*! if buffer might contain valid data */
        DivXBool validData;
    }
    RFBuf;

    typedef struct _RFConn
    {
        /*! tcp connection */
        CDivXTcpConn *pTcp;

        /*! set if connection is closing */
        RFConnState state;

        /*! pending data ranges */
        std::list<RFRange*> request;    

        /*! set if header to be received */
        DivXBool header;

        /*! number of pending byte ranges, for current request */
        uint32_t ranges;

        /*! number of pending bytes */
        uint64_t bytes;

        /*! flag set when connection is reset */
        bool reset;
    }
    RFConn;

    /*! @note Only GetInfo, Request, CheckRequestSize, and CheckRange can be called from callback */
    typedef int (*RFEvent)(uint8_t event, RFData *pData, void *pContext);

    class CDivXRemoteFile
    {
    public:

        /*! constructor */
        CDivXRemoteFile(DivXMem hMem);

        /*! deconstructor */
        ~CDivXRemoteFile();

        /*! connect to remote server */
        int Connect(RFParams *pParams, RFEvent pCallback, void *pContext);

        /*! disconnect from remote server */
        int Disconnect();

        /*! start callback thread */
        int Start();

        /*! stop callback thread */
        int Stop();

        /*! get file info */
        /*! @note info will reference internal memory */
        int GetInfo(RFInfo *pInfo);

        /*! request a byte range */
        int Request(uint64_t pos, uint64_t len);

        /*! get number of outstanding bytes requested */
        int CheckRequestSize(uint64_t *pSize);

        /*! flush all possible requests */
        int Flush();

        /*! get closes unrequested byte range */
        int CheckRange(uint64_t *pPos, uint64_t *pLen);

    private:

        /*! utility function to set error state */
        void SetError(uint8_t state);

        /*! utility function to defragment recve buffer */
        void AlignBuffer(uint8_t *pBuf, uint32_t size, uint64_t *pStart, uint64_t *pEnd);

        /*! serialize state changing calls */
        CDivXMutex serialMutex;

        /*! thread for sending requests */
        THREAD_FUNC_FWD(CDivXRemoteFile, SendThreadFwd, SendThread);
                
        /*! forwarding function */
        void SendThread(bool *pStopFlag, void* pContext, DivXMem hMem);

        /*! thread for allocating new connections and sending data */
        CDivXThread sendThread;

        /*! event set when new connection needed or data ready to be sent */
        CDivXEvent sendEvent;

        /*! lock thread in known state */
        CDivXMutex sendMutex;



        /*! thread for receiving requests */
        THREAD_FUNC_FWD(CDivXRemoteFile, RecvThreadFwd, RecvThread);

        /*! forwarding function */
        void RecvThread(bool *pStopFlag, void* pContext, DivXMem hMem);

        /*! thread for receiving data and closing connections */
        CDivXThread recvThread;

        /*! event set when new connection active */
        CDivXEvent recvEvent;

        /*! lock thread in known state */
        CDivXMutex recvMutex;

        /*! flag set when delivery is paused */
        bool paused;



        /*! mutex protecting connections */
        CDivXMutex connMutex;

        /*! connections to remote server */
        RFConn conn[RF_CONNECTIONS];

        /*! flag set when connections flushing */
        DivXBool flush;


        
        /*! mutex protecting requests */
        CDivXMutex requestMutex;

        /*! linked list of requests */
        std::list<RFRange*> request;


        /*! event set when playback thread active */
        CDivXEvent playEvent;

        /*! copy of block size */
        uint32_t blockSize;

        /*! make a copy of url */
        DivXString *pUrl;

        /*! copy of login */
        DivXString *pLogin;

        /*! copy of password */
        DivXString *pPassword;

        /*! byte sized host string */
        uint8_t *pHost;

        /*! host port */
        uint16_t hostPort;

        /*! protect remote file info */
        CDivXMutex infoMutex;

        /*! timestamp buffer */
        uint8_t *pTimestamp;

        /*! timestamp buffer size */
        uint32_t timestampSize;

        /*! size of remote file */
        uint64_t fileSize;

        /*! set to non-zero if remote server accepts ranges */
        uint8_t byteRanges;

        /*! current bitrate */
        uint32_t bitrate;

        /*! current downloading state */
        uint8_t rfState;

        /*! callback function */
        RFEvent pExtCallback;

        /*! external context */
        void *pExtContext;

        /*! Memory handler, use NULL to default to memaligned memory */
        DivXMem hMem;
    };
}

#endif
