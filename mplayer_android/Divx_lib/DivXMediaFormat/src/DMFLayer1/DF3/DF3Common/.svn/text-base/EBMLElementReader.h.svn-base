/*!
    @file
@verbatim
$Id: EBMLElementReader.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _EBMLELEMENTREADER_H_
#define _EBMLELEMENTREADER_H_

//@TODO this shouldn't be pulling from DF3Common, but input stream will eventually 
// be moved to DivXOS so this will be updated then
#include "DF3/DF3Common/DMFInputStream.h"
#include "MKVParserElements.h"
#include "DivXInt.h"

#if defined INT64_C
//#error INTC defined
#else
#if defined  WIN32
#define INT64_C(val) val##i64
#define UINT64_C(val) val##ui64
#else
#define INT64_C(val) val##LL
#define UINT64_C(val) val##ULL
#endif
#endif

// use this LUT to clear the variable length byte's leading 0s and 1
static const int VINT_CLEAR[] = { 0, 0x7f, 0x3f, 0x1f, 0x0F, 0x07, 0x03, 0x01, 0x00 };

// use this LUT to convert an unsigned vint to signed
static const int64_t VINT_SIGNED[] = {  0x3f, 0x1fff, 0x0fffff, 0x07fffff, 
                                    INT64_C(0x03ffffffff), INT64_C(0x01ffffffffff), 
                                    INT64_C(0x00ffffffffffff), INT64_C(0x007fffffffffffff) };

// Vint length LUT -- index in, # of leading zeros plus one out
static int VINT_LENGTH[] = {  9,                                                // 8 leading zeros
                              8,                                                // 7 leading zeros
                              7, 7,                                             // 6 leading zeros
                              6, 6, 6, 6,                                       // 5 leading zeros
                              5, 5, 5, 5, 5, 5, 5, 5,                           // 4 leading zeros
                              4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,   // 3 leading zeros
                              3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,   // 2 leading zeros
                              3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,   // 2 leading zeros 
                              2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   // 1 leading zeros
                              2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   // 1 leading zeros
                              2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   // 1 leading zeros
                              2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   // 1 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // 0 leading zeros
                              1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }; // 0 leading zeros


/*! Read an element ID from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param puiElementId     (OUT) - Pointer to a uint64_t where the ID will be stored
    @param pBytesRead       (OUT) - Number of bytes storage for this value

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Vint reading was cut short
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_ElementID(DMFInputStreamHandle hInputStream, MKVELEMENT_ID_e *puiElementID, uint32_t* pBytesRead);

/*! Read an element's size from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param puiElementSize   (OUT) - Pointer to a uint64_t where the size will be stored
    @param pBytesRead       (OUT) - Bytes read from element size

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Vint reading was cut short
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_ElementSize(DMFInputStreamHandle hInputStream, uint64_t *puiElementSize, uint32_t* pBytesRead);

/*! Read a variable-length integer (vint) from an input stream

    @note   bClearLeadingSetBit is useful for distinguishing between ID reads and size reads.
            Documented EBML IDs usually don't clear the leading set bit in the vint.  
            Size values, on the other hand, need to have the leading bit cleared or the value
            returned will be grossly incorrect.

    @param hInputStream         (IN) - DMFInputStream handle
    @param *puiVintOut         (OUT) - Pointer to an 8-byte buffer (uint64/int64) where the vint will be stored
    @param bSigned              (IN) - true = read signed vint
                                       false = read unsigned vint
    @param bClearLeadingSetBit  (IN) - true = leading bit will be cleared (return value 
                                              will be actual integer value represented by the vint)
                                       false = leading bit will be left set (return value
                                               will reflect the actual bytes read untouched)
    @param pBytesRead          (OUT) - Number of bytes this vint takes up

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Vint reading was cut short
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_ReadExVint(DMFInputStreamHandle hInputStream, uint64_t *puiVintOut, DivXBool bSigned, DivXBool bClearLeadingSetBit, uint32_t* pBytesRead);

/*! Read an unsigned variable-length integer (vint) from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param puiVintOut      (OUT) - Pointer to a uint64_t where the unsigned vint will be stored
    @param pBytesRead      (OUT) - Number of bytes for this vint

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Vint reading was cut short
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_vint(DMFInputStreamHandle hInputStream, uint64_t *puiVintOut, uint32_t* pBytesRead);

/*! Read a signed variable-length integer (vint) from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param piSvintOut      (OUT) - Pointer to an int64_t where the signed vint will be stored
    @param pBytesRead      (OUT) - Number of bytes for this vint

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Vint reading was cut short
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_svint(DMFInputStreamHandle hInputStream, int64_t *piSvintOut, uint32_t* pBytesRead);

/*! Read a signed int from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param piIntOut        (OUT) - Pointer to a int64_t where the signed int will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_INVALID_ARG
                                     - uiSize > 8 or uiSize = 0
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_int(DMFInputStreamHandle hInputStream, uint64_t uiSize, int64_t *piIntOut);

/*! Read a unsigned int from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param puiUintOut      (OUT) - Pointer to a uint64_t where the unsigned int will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_INVALID_ARG
                                     - uiSize > 8 or uiSize = 0
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_uint(DMFInputStreamHandle hInputStream, uint64_t uiSize, uint64_t *puiUintOut);

/*! Read a boolean value from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param puiUintOut      (OUT) - Pointer to a uint32_t where the boolean will be stored
                                   (1 for true, 0 for false)

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_INVALID_ARG
                                     - uiSize > 8 or uiSize = 0
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_bool(DMFInputStreamHandle hInputStream, uint64_t uiSize, uint32_t *puiUintOut);

/*! Read a float from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param pfFloatOut      (OUT) - Pointer to where the float will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than 4
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_float(DMFInputStreamHandle hInputStream, uint64_t uiSize, double *pfDoubleOut);

/*! Read binary data from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param puiBinaryOut    (OUT) - Pointer to a buffer where the binary data will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_binary(DMFInputStreamHandle hInputStream, uint64_t uiSize, void *puiBinaryOut);

/*! Read an element ID from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param psUTF8Out       (OUT) - Pointer to a DivXString where the UTF-8 string will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_UTF8(DMFInputStreamHandle hInputStream, uint64_t uiSize, DivXString *psUTF8Out);

/*! Read an element ID from an input stream

    @note

    @param hInputStream     (IN) - DMFInputStream handle
    @param uiSize           (IN) - Size of data to be read
    @param psStringOut     (OUT) - Pointer to a DivXString where the string will be stored

    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_READ_FAILURE
                                     - Number of bytes read was less than uiSize
                                    DFR_UnableToRead
                                     - Read failed
                                    DFR_BadHandle
                                     - Input stream did not have file open
*/
DivXError DMFebml_Read_string(DMFInputStreamHandle hInputStream, uint64_t uiSize, DivXString *psStringOut);

/*! Convert from iee754 input to float

    @note

    @param buffer     (IN) - Array of 4 input bytyes 
    @param pfFloatOut (OUT) - pointer to float
    @return DivXError       (OUT) - Error code:
                                    DIVX_ERR_INVALID_ARG - Input bytes represent +/- infinity or NaN
                                     
*/
DivXError ieee754ToFloat(const uint8_t *buffer, float *pfFloatOut);

/*! Convert from iee754 input to double

    @note

    @param buffer     (IN) - Array of 8 input bytyes 
    @param pdDoubleOut (OUT) - pointer to double
    @return DivXError  (OUT) - Error code:
                               DIVX_ERR_INVALID_ARG - Input bytes represent +/- infinity or NaN
                                     
*/
DivXError ieee754ToDouble(const uint8_t *buffer, double *pdDoubleOut);

#endif
/* _EBMLELEMENTREADER_H_ */
