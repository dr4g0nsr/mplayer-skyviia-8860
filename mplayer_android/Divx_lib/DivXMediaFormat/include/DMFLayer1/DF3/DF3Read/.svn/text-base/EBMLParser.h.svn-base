/*!
    @file
@verbatim
$Id: EBMLParser.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _EBMLPARSER_H_
#define _EBMLPARSER_H_

#include "DF3/DF3Common/EBMLElementReader.h"
#include "DF3/DF3Common/DMFInputStream.h"
#include "EBMLMap.h"
#include "DivXInt.h"
#include "DF3/DF3Read/MKVCache.h"

struct __EBMLParser__;

/*! EBMLData definition */
typedef void* EBMLData_t;

/*! EBMLType definition */
typedef int32_t EBMLType_t;

/*! EBMLFilePosition definition */
typedef uint64_t EBMLFilePosition;

/*! @brief Function pointer to notification callback function for parser */
typedef DivXError ( *pfnNotify )                 ( EBMLType_t*, EBMLData_t*);

typedef DivXError ( *pfnNotifyElement )          ( uint64_t elementId, uint64_t size, uint64_t offset );

typedef DivXError ( *pfnNotifyElement2 )         ( void * inst, uint64_t elementId, uint64_t elementStartOffset, uint64_t elementDataOffset, uint64_t elementDataSize );

/*! EBML Parser Open function definition */
typedef DivXError (* pfnDMFebmlParser_Open)      (struct __EBMLParser__**, DivXString*, DivXMem);

/*! EBML Parser Close function definition */
typedef DivXError (* pfnDMFebmlParser_Close)     (struct __EBMLParser__*, DivXMem);

/*! EBML Parser Set Notify function definition */
typedef DivXError (* pfnDMFebmlParser_SetNotify) (struct __EBMLParser__*, pfnNotify*);

/*! EBML Parser Parse function definition */
typedef DivXError (* pfnDMFebmlParser_Parse)        (struct __EBMLParser__*, EBMLFilePosition);

/*! EBML Structure definition */
typedef struct __EBMLParser__
{
    pfnDMFebmlParser_Open      fnOpen;
    pfnDMFebmlParser_Close     fnClose;
    pfnDMFebmlParser_SetNotify fnSetNotify;
    pfnDMFebmlParser_Parse     fnParse;
    DMFInputStreamHandle            *hInputStream;
    pfnNotifyElement           fnNotifyClient;
} EBMLParser;

/*! Errors for DMFebmlParser_Open function */
typedef enum {
    DMFebmlParser_OpenErr_NoError,
    DMFebmlParser_OpenErr_NoFileExists,
    DMFebmlParser_OpenErr_FileOpenErr,
    DMFebmlParser_OpenErr_MemHandleInvalid,
    DMFebmlParser_OpenErr_InvalidArgument,
    DMFebmlParser_OpenErr_NotEnoughMemory,
    DMFebmlParser_OpenErr_NumErrors
}DMFebmlParser_OpenErr;
/*! Open an EBML Parser object for parsing

    @note

    @param phEBMLParser    (OUT) - Pointer to EBMLParser handle
    @param filename         (IN) - File name to open
    @param hMem             (IN) - Memory handle for allocation

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DMFebmlParser_Open(EBMLParser** ppEBMLParser, DivXString* filename, DivXMem hMem);

DivXError DMFebmlParser_Create(EBMLParser** ppEBMLParser, DMFInputStreamHandle *hInputStream, DivXMem hMem);

/*! Errors for DMFebmlParser_Close function */
typedef enum {
    DMFebmlParser_CloseErr_NoError,
    DMFebmlParser_CloseErr_FileCloseErr,
    DMFebmlParser_CloseErr_MemHandleInvalid,
    DMFebmlParser_CloseErr_InvalidArgument,
    DMFebmlParser_CloseErr_NumErrors
}DMFebmlParser_CloseErr;
/*! Close an EBML Parser object

    @note

    @param phEBMLParser     (IN) - EBMLParser handle
    @param hMem             (IN) - Memory handle for deallocation

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DMFebmlParser_Close(EBMLParser* pEBMLParser, DivXMem hMem);

/*! Errors for DMFebmlParser_SetNotify function */
typedef enum {
    DMFebmlParser_SetNotifyErr_NoError,
    DMFebmlParser_SetNotifyErr_InvalidFuncPtr,
    DMFebmlParser_SetNotifyErr_InvalidArgument,
    DMFebmlParser_SetNotifyErr_NumErrors
}DMFebmlParser_SetNotifyErr;
/*! Set notification callback for parser

    @note   This function only accepts one client callback

    @param fnNotify         (IN) - Pointer to notification function callback

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DMFebmlParser_SetNotify(EBMLParser* pEBMLParser, pfnNotify* fnNotify);

/*! Errors for DMFebmlParser_SetNotify function */
typedef enum {
    DMFebmlParser_ParseErr_NoError,
    DMFebmlParser_ParseErr_InvalidFuncPtr,
    DMFebmlParser_ParseErr_InvalidArgument,
    DMFebmlParser_ParseErr_NumErrors
}DMFebmlParser_ParseErr;
/*! Parse the file from the map

    @note   Perform parse operation

    @param fnNotify         (IN) - Pointer to notification function callback

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DMFebmlParser_Parse(EBMLParser* pEBMLParser, EBMLMap* ebmlMap, EBMLFilePosition pos);

#endif
/* _EBMLPARSER_H_ */
