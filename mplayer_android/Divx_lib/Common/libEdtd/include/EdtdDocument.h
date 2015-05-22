/********************************************************************
* $Id: EdtdDocument.h 58500 2009-02-18 19:45:46Z jbraness $
*
* Copyright © 2008 DivX, Inc. http://www.divx.com/company
* All rights reserved.
*
* This software is the confidential and proprietary information of
* DivX, Inc. and may be used only in accordance with the terms of
* your license from DivX, Inc.
********************************************************************/

#ifndef _EdtdDocument_h_8FE7B4A3_7955_4564_897C_CD88709430C1__INCLUDED_
#define _EdtdDocument_h_8FE7B4A3_7955_4564_897C_CD88709430C1__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXError.h"
#include "DivXLinkedList.h"
#include "DivXMem.h"
#include "EdtdDocument.h"

typedef void *EdtdDocumentHandle;
//typedef void *EdtdElementHandle;
typedef struct _EdtdElement_t *EdtdElementHandle;
//typedef void *EdtdRangeHandle;
typedef struct _EdtdRange_t *EdtdRangeHandle;
typedef void *EdtdValueHandle;

typedef enum {
    EBML_UNKNOWN,       /* unknown type */
    EBML_MASTER,        /* AKA container */
    EBML_DERIVED,       /* A type defined in the EBML DTD that derives from some bais type */
    EBML_UVINT,         /* unsigned variable length integer, usually 64 bit */
    EBML_SVINT,         /* signed variable length integer, usually 64 bit */
    EBML_UINT,          /* unsigned integer, usually 64 bit */
    EBML_SINT,          /* signed integer, usually 64 bit */
    EBML_FLOAT,         /* a floating point number */
    EBML_UTF8_STRING,   /* a string of UTF-8 characters */
    EBML_ASCII_STRING,  /* technically, a MKV derived type, a UTF-8 string with all characters limited to be >= 0x20 and <= 0x7e */
    EBML_BOOL,          /* technically, a MKV derived type, an unsigned integer with the values limited to 0 or 1 */
    EBML_BINARY,        /* binary data */
    EBML_DATE           /* a 64 bit integer indicating the number of milliseconds from 01/01/2001 */
} EBMLDataType_e;


/*
typedef enum _EdtdBaseType_t
{
    EDTD_COMPLEX,   // complex - base type is a defined type
    EDTD_INT,       // integer
    EDTD_UINT,      // unsigned
    EDTD_FLOAT,     // float
    EDTD_STRING,    // string
    EDTD_DATE,      // date
    EDTD_BINARY,    // binary
    EDTD_CONTAINER  // container
} EdtdBaseType_t;
*/

typedef union _EdtdBaseValue_t
{
    void*       complexValue;   // complex
    int64_t     sintValue;      // signed integer
    uint64_t    uintValue;      // unsigned integer
    double      floatValue;     // float
    DivXString* stringValue;    // string
    int64_t     dateValue;      // date
    void*       binaryValue;    // binary
} EdtdBaseValue_t;

DivXError EdtdDocument_Create( EdtdDocumentHandle *pHandle, DivXMem hMem );

DivXError EdtdDocument_Destroy( EdtdDocumentHandle handle );

DivXError EdtdDocument_GetElementByName( EdtdDocumentHandle handle, EdtdElementHandle *pHandle, const DivXString* sName );

DivXError EdtdDocument_GetElementById( EdtdDocumentHandle handle, EdtdElementHandle *pHandle, const DivXString* sId );

DivXError EdtdDocument_GetElementByIntegerId( EdtdDocumentHandle handle, EdtdElementHandle *pHandle, uint64_t id );

DivXError EdtdDocument_GetElementName( EdtdElementHandle handle, const DivXString** psName );

DivXError EdtdDocument_GetElementTypeName( EdtdElementHandle handle, const DivXString** psType );

DivXError EdtdDocument_GetElementId( EdtdElementHandle handle, const DivXString** psId );

DivXError EdtdDocument_RangeDefined( EdtdElementHandle handle, DivXBool* rangeDefined);

DivXError EdtdDocument_TestRange( EdtdElementHandle handle, EdtdBaseValue_t* pValue, DivXBool* inRange );

DivXError EdtdDocument_GetElementUiId(EdtdElementHandle handle, uint64_t* pUId);

DivXError EdtdDocument_GetElementParents( EdtdElementHandle handle, LinkedList_t** ppParents );

DivXError EdtdDocument_GetElementLevels( EdtdElementHandle handle, EdtdRangeHandle* phLevels );

DivXError EdtdDocument_GetElementCardinality( EdtdElementHandle handle, DivXString* pCardinality );

DivXError EdtdDocument_GetElementOrdered( EdtdElementHandle handle, DivXBool* pOrdered );

DivXError EdtdDocument_GetElementRanges( EdtdElementHandle handle, LinkedList_t** ppRanges );

DivXError EdtdDocument_GetElementSizes( EdtdElementHandle handle, LinkedList_t** ppSizes );

DivXError EdtdDocument_GetElementDefault( EdtdElementHandle handle, EdtdValueHandle* phDefault );

DivXError EdtdDocument_GetValueBaseType( EdtdValueHandle handle, EBMLDataType_e* pBaseType );

DivXError EdtdDocument_GetValueBaseValue( EdtdValueHandle handle, EdtdBaseValue_t* pBaseValue );

#ifdef __cplusplus
}
#endif

#endif /* _EdtdDocument_h_8FE7B4A3_7955_4564_897C_CD88709430C1__INCLUDED_ */
