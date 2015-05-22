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

#ifndef CDIVXHTTPPARSE_H
#define CDIVXHTTPPARSE_H

#include "DivXInt.h"
#include "DivXWchar.h"
#include "DivXBool.h"
#include "DivXNULL.h"
#include "DivXString.h"
#include "DivXMem.h"
/*
namespace DivXOS
{
*/
/*! return codes */
#define SH_OK           0x00
#define SH_FAIL         0x01
#define SH_INCOMPLETE   0x02
#define SH_NOMEM        0x03
#define SH_NOTIMPL      0x04
#define SH_INVALID_SERV 0x05

/*! response codes */
//#define SH_OK           0x00
//#define SH_FAIL         0x01
#define SH_AUTHFAIL     0x02
#define SH_REDIRECT     0x03
#define SH_MULTIPART    0x04
#define SH_PARTIAL      0x05
#define SH_FILE         0x06

    /*! url info */
    typedef struct _HttpUrlInfo
    {
        /*! pointer to server segment */
        DivXString *pHost;

        /*! size of server string */
        uint32_t hostLen;

        /*! pointer to content segment */
        DivXString *pContent;

        /*! size of content string */
        uint32_t contentLen;

        /*! destination port */
        uint16_t port;
    }
    HttpUrlInfo;

    /*! parameters for write init request */
    typedef struct _HttpInitRequest
    {
        /*! url string */
        DivXString *pUrl;

        /*! login string */
        DivXString *pLogin;

        /*! password string */
        DivXString *pPassword;
    }
    HttpInitRequest;



    /*! parameters for parse init response */
    typedef struct _HttpInitResponse
    {
        /*! response code */
        uint8_t code;

        /*! if redirect, redirection url, pointer inside buffer */
        uint8_t *pUrl;

        /*! if redirect, size of url */
        uint32_t urlSize;

        /*! if not redirect, content size */
        uint64_t contentSize;

        /*! if not redirect, modified timetsamp, pointer inside buffer */
        uint8_t *pTimestamp;

        /*! if not redirect, size of timestamp */
        uint32_t timestampSize;

        /*! set if byte ranges are supported */
        uint8_t byteRanges;
    }
    HttpInitResponse;


    /*! parameters for a file request */
    typedef struct _HttpFileRequest
    {
        /*! url string */
        DivXString *pUrl;

        /*! login string */
        DivXString *pLogin;

        /*! password string */
        DivXString *pPassword;
    }
    HttpFileRequest;

        
    /*! byte range */
    typedef struct _HttpByteRange
    {
        /*! start position */
        uint64_t pos;

        /*! length of byte range */
        uint64_t len;
    }
    HttpByteRange;

    /*! parameters for byte reqeust */
    typedef struct _HttpByteRequest
    {
        /*! number of byte ranges */
        uint32_t num;

        /*! array of byte ranges */
        HttpByteRange *pRanges;

        /*! url string */
        DivXString *pUrl;

        /*! login string */
        DivXString *pLogin;

        /*! password string */
        DivXString *pPassword;
    }
    HttpByteRequest;


    /*! parameters for file response */
    typedef struct _HttpFileResponse
    {
        /*! response code */
        uint8_t code;

        /* reference in buffer to timestamp */
        uint8_t *pTimestamp;

        /*! size of timestamp field */
        uint32_t timestampSize;

        /*! set if connection is closing */
        DivXBool closing;
    }
    HttpFileResponse;


    /*! parameters for byte response */
    typedef struct _HttpByteResponse
    {
        /*! response code */
        uint8_t code;

        /*! if not multipart, payload byte range */
        HttpByteRange range;

        /*! reference in buffer to timetsamp */
        uint8_t *pTimestamp;

        /*! size of timetsamp field */
        uint32_t timestampSize;

        /*! set if connection is closing */
        DivXBool closing;
    }
    HttpByteResponse;


    class HttpParse
    {
    public:
        HttpParse(){};
        virtual ~HttpParse(){};
        /*! parse basic http head response */
        static uint8_t HttpParseInitResponse(void *pBuf, uint32_t *pSize, HttpInitResponse *pResponse);
        /*! write basic http head request */
        static uint8_t HttpWriteInitRequest(void *pBuf, uint32_t *pSize, HttpInitRequest *pRequest, DivXMem hMem);
        /*! parse an http url */
        static uint8_t HttpParseUrl(DivXString *pUrl, HttpUrlInfo *pInfo);
        /*! write a generic request for full file */
        static uint8_t HttpWriteFileRequest(void *pBuf, uint32_t *pSize, HttpFileRequest *pRequest, DivXMem hMem);
        /*! write byte request */
        static uint8_t HttpWriteByteRequest(void *pBuf, uint32_t *pSize, HttpByteRequest *pRequest, DivXMem hMem);
        /*! parse file response */
        static uint8_t HttpParseFileResponse(void *pBuf, uint32_t *pSize, HttpFileResponse *pResponse);
        /*! parse byte response */
        static uint8_t HttpParseByteResponse(void *pBuf, uint32_t *pSize, HttpByteResponse *pResponse);
        /*! parameters for part response */
        static uint8_t HttpParsePartResponse(void *pBuf, uint32_t *pSize, HttpByteRange *pRange);
    };

#endif
