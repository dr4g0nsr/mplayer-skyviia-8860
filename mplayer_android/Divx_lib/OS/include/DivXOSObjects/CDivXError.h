/*!
    @file
@verbatim
$Id: CDivXError.h 49106 2008-02-07 18:42:23Z cdurham $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _CDIVXERROR_H_
#define _CDIVXERROR_H_


/*! \name Download Manager return codes */
    /*! \{ */
typedef enum {
    DCDM_OK                  = 0x00,  /*!< Success! */
    DCDM_FAIL                = 0x01,  /*!< Generic failure */
    DCDM_NOTIMPL             = 0x02,  /*!< Functionality not implemented */
    DCDM_INVALID_PARAM       = 0x03,  /*!< Invalid parameter */
    DCDM_OUT_OF_MEMORY       = 0x04,  /*!< Out of memory */
    DCDM_ACCESS_DENIED       = 0x05,  /*!< Access denied */
    DCDM_INVALID_CALL        = 0x06,  /*!< Invalid call */
    DCDM_HOST_NOT_FOUND      = 0x07,  /*!< Host was not found */
    DCDM_BUFFER_FULL         = 0x08,  /*!< Buffer is full */
    DCDM_SOCKET_ERROR        = 0x09,  /*!< Generic socket error */
    DCDM_DISCONNECTED        = 0x0A,  /*!< Disconnected */
    DCDM_VALUE_NOT_SET       = 0x0B,  /*!< Value is not set */
    DCDM_PARSE_ERROR         = 0x0C,  /*!< Generic parsing error */
    DCDM_END_OF_FILE         = 0x0D,  /*!< End of file reached */
    DCDM_ABORT_READ          = 0x0E,  /*!< Read aborted before completed */
    DCDM_WOULD_BLOCK         = 0x0F,  /*!< Data not ready yet, and non-blocking */
    DCDM_EXPIRED             = 0x10,  /*!< Cached data is no longer valid */
    DCDM_INVALID_SERVER      = 0x11,  /*!< HTTP server is invalid */
    DCDM_NO_DATA_STAT_FILES  = 0x12,  /*!< The data or status files do not exist for existing */
    DCDM_HTTP_FILE_REQUEST_RANGE_NOT_READY    = 0x13,
    NUM_DCDM_ERR
} DCDM_ERR_e;

    /*! \} */

    /*! \name DownloadManager return code helper functions */
    /*! \{ */
    /*! Detect success error code */
    #define DCDM_SUCCESS(x) (x == DDM_OK)
    /*! Detect failure error code */
    #define DCDM_FAILED(x) (x != DDM_OK)
    /*! \} */

    /*! \name Download Manager File States */
    /*! \{ */
    #define DCDM_STATE_DOWNLOADING   0x01    /*!< remote connection active */
    #define DCDM_STATE_COMPLETED     0x02    /*!< download of file complete */
    #define DCDM_STATE_DISCONNECTED  0x03    /*!< connection to server lost before completed download */
    #define DCDM_STATE_EXPIRED       0x04    /*!< access to file has expired */
    #define DCDM_STATE_ERROR         0x05    /*!< generic error; temp files could be currupt; delete them */
    #define DCDM_STATE_CLOSED        0x06    /*!< all internal resources closed */


#endif
// The following line left intentionally blank
