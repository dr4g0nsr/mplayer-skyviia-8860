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

#ifndef _DF3EBMLHELPER_H
#define _DF3EBMLHELPER_H

#include "DF3/DF3Common/EBMLTypes.h"
#include "./AVI/AVICommon/ByteSwap.h"
#include "DivXTypes.h"
#include "EdtdDocument.h"

#define SWAP_64(a) \
    ( \
    (( (uint64_t)( a ) ) << 56 ) & DIVX_UI64(0xFF00000000000000) |  \
    (( (uint64_t)( a ) ) << 40 ) & DIVX_UI64(0x00FF000000000000) |  \
    (( (uint64_t)( a ) ) << 24 ) & DIVX_UI64(0x0000FF0000000000) |  \
    (( (uint64_t)( a ) ) << 8  ) & DIVX_UI64(0x000000FF00000000) |  \
    (( (uint64_t)( a ) ) >> 8  ) & DIVX_UI64(0x00000000FF000000) |  \
    (( (uint64_t)( a ) ) >> 24 ) & DIVX_UI64(0x0000000000FF0000) |  \
    (( (uint64_t)( a ) ) >> 40 ) & DIVX_UI64(0x000000000000FF00) |  \
    (( (uint64_t)( a ) ) >> 56 ) & DIVX_UI64(0x00000000000000FF)    \
    )

#define SWAP_32(a) \
    ( \
    (( (uint32_t)( a ) ) << 24 ) & (0xFF000000) |  \
    (( (uint32_t)( a ) ) << 8  ) & (0x00FF0000) |  \
    (( (uint32_t)( a ) ) >> 8  ) & (0x0000FF00) |  \
    (( (uint32_t)( a ) ) >> 24 ) & (0x000000FF)   \
    )

#define SWAP_16(a) \
    ( \
    (( (int16_t)( a ) ) << 8 ) & (0xFF00) |  \
    (( (int16_t)( a ) ) >> 8 ) & (0x00FF) \
    )

typedef union detect
{
    uint8_t  cNumArray[sizeof(uint32_t)];
    uint32_t num;
} detect;

static __inline DivXBool IsBigEndian()
{
    detect d;
    d.num = (uint32_t)1;
    /* big endian arch will show detect.cNumArray[0] = 0
       little endian arch will show detect.cNumArray[0] = 1 */
    if (d.cNumArray[0] == 0)
    {
        return DIVX_TRUE;
    }
    else
    {
        return DIVX_FALSE;
    }
}

static __inline uint64_t DIVX_LITTLE_ENDIAN_64_VALUE(uint64_t a)
{
    uint64_t retVal = 0;
    if (IsBigEndian() == DIVX_FALSE)
    {
        retVal = a;
    }
    else
    {
        retVal = SWAP_64(a);
    }
    return retVal;
}


static __inline uint64_t DIVX_BIG_ENDIAN_64_VALUE(uint64_t a)
{
    uint64_t retVal = 0;
    if (IsBigEndian() == DIVX_TRUE)
    {
        retVal = a;
    }
    else
    {
        retVal = SWAP_64(a);
    }
    return retVal;
}

static __inline uint32_t DIVX_BIG_ENDIAN_32_VALUE(uint32_t a)
{
    uint32_t retVal = 0;
    if (IsBigEndian() == DIVX_TRUE)
    {
        retVal = a;
    }
    else
    {
        retVal = SWAP_32(a);
    }
    return retVal;
}

static __inline uint16_t DIVX_BIG_ENDIAN_16_VALUE(uint16_t a)
{
    uint16_t retVal = 0;
    if (IsBigEndian() == DIVX_TRUE)
    {
        retVal = a;
    }
    else
    {
        retVal = SWAP_16(a);
    }
    return retVal;
}

typedef struct _DF3ebml_value
{
    union
    {
        uint64_t uIntValue;
        int64_t sIntValue;
        double doubleValue;
        struct {
#ifdef DIVX_IS_BIG_ENDIAN
        float hi; // really just padding
        float low;
#else
        float low;
        float hi;
#endif
        } floatValue;
        uint8_t charValue[8];
    } ebmlInt;
    int length;
    EBMLDataType_e typeRepresentation;
} DF3ebml_value;

#ifdef __cplusplus
extern "C" {
#endif

unsigned int DF3ebml_MinLengthToEncodeUVInt(uint64_t number);

unsigned int DF3ebml_MinLengthToEncodeSVInt(int64_t number);

unsigned int DF3ebml_MinLengthToEncodeUInt(uint64_t number);

unsigned int DF3ebml_MinLengthToEncodeSInt(int64_t number);

unsigned int DF3ebml_MinLengthToEncodeDouble(double number);

DivXError DF3embl_NumberToEbmlUVInt( uint64_t number, DF3ebml_value* pValue, unsigned int minLength );

DivXError DF3embl_NumberToEbmlSVInt( int64_t number, DF3ebml_value* pValue, unsigned int minLength );

DivXError DF3ebml_NumberToEbmlUInt( uint64_t number, DF3ebml_value* pValue, unsigned int minLength );

DivXError DF3ebml_NumberToEbmlSInt( int64_t number, DF3ebml_value* pValue, unsigned int minLength );

DivXError DF3ebml_NumberToEbmlFloat( double number, DF3ebml_value* pValue, unsigned int length );

DivXError DF3ebml_EbmlUVIntToNumber( const DF3ebml_value* pValue, uint64_t* pNumber );

DivXError DF3ebml_EbmlSVIntToNumber( const DF3ebml_value* pValue, int64_t* pNumber );

DivXError DF3ebml_EbmlUIntToNumber( const DF3ebml_value* pValue, uint64_t* pNumber );

DivXError DF3ebml_EbmlSIntToNumber( const DF3ebml_value* pValue, int64_t* pNumber );

DivXError DF3ebml_EbmlFloatToNumber( const DF3ebml_value* pValue, double* pNumber );

#ifdef __cplusplus
}
#endif

#endif /*_DF3EBMLHELPER_H */
