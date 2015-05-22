/*!
    @file
@verbatim
$Id: DivXBitstreamUtil.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXBITSTREAMUTIL_H_
#define _DIVXBITSTREAMUTIL_H_

#include "DivXTypes.h"

#ifdef __cplusplus
extern "C" {
#endif


extern const uint8_t
    g_DivXBitMask[9];


/*!
    Function to write bits to a bitstream.

    @note This should only be used if writing less than 8 bits
    to the buffer. Otherwise make use of WriteUint08, WriteUint16,
    etc.

    @param pBuffer (IN/OUT) - Bitstream to write bits to.
    @param bitStart (IN) - Number of bit to start with. (zero based)
    @param newBitStart (OUT) - New bit position after writing. (zero based)
    @param bitSize (IN) - Number of bits to write.
    @param value (IN) - The bits to write to the buffer.
**/
DIVX_INLINE void WriteBits(void *pBuffer, uint32_t bitStart, uint32_t *newBitStart, uint32_t bitSize, uint32_t value)
{
    uint8_t *buff8 = (uint8_t *)pBuffer;
    uint32_t    byteOffs = bitStart / 8;
    uint32_t    slide;

    bitStart = bitStart%8;

    slide = 8 - bitStart - bitSize;

    /* make sure we aren't accessing outside the g_DivXBitMask array */
    if(bitSize > 8) { bitSize = 8; }

    /* make sure we aren't starting outside the byte */
    if(bitStart > 8) { bitStart = 8; }

    /* make sure bitSize fits into byte */
    if( (bitSize + bitStart) > 8 ) { bitSize = 8 - bitStart; }

    /* mask out unwanted bits */
    value &= g_DivXBitMask[bitSize];

    /* clear bits that we're updating */
    buff8[byteOffs] &= ~(g_DivXBitMask[bitSize] << slide);

    /* write value to buffer */
    buff8[byteOffs] |= value << slide;

    /* increment cursor, if provided */
    if(newBitStart != NULL) { *newBitStart += bitSize; }
}


/*!
    Function to get bits from a bitstream.

    @note This should only be used if reading less than 8 bits
    from a buffer. Otherwise make use of ReadUint08, ReadUint16,
    etc.

    @param pBuffer (IN) - Bitstream to get bits from.
    @param bitStart (IN) - Number of bit to start with. (zero based)
    @param newBitStart (OUT) - New bit position after reading. (zero based)
    @param bitSize (IN) - Number of bits to read.
    @return the value returned within an (uint8_t) type 
**/
DIVX_INLINE uint8_t ParseBits(void *pBuffer, uint32_t bitStart, uint32_t *newBitStart, uint32_t bitSize)
{
    uint8_t *buff8 = (uint8_t *)pBuffer;
    uint32_t    byteOffs = bitStart / 8;
    uint32_t    value;
    uint32_t    slide;

    bitStart = bitStart%8;

    slide = 8 - bitStart - bitSize;

    /* make sure we aren't accessing outside the g_DivXBitMask array */
    if(bitSize > 8) { bitSize = 8; }

    /* make sure we aren't starting outside the byte */
    if(bitStart > 8) { bitStart = 8; }

    /* make sure bitSize fits into byte */
    if( (bitSize + bitStart) > 8 ) { bitSize = 8 - bitStart; }

    /* trim off right side */
    value = buff8[byteOffs] >> slide;

    /* trim off left side to get value */
    value &= g_DivXBitMask[bitSize];

    /* increment cursor, if provided */
    if(newBitStart != NULL) { *newBitStart += bitSize; }

    return (uint8_t) value;
}


/*!
    Function to write an unsigned 8 bit integer to the buffer.

    @param pBuffer (IN/OUT) - Bitstream to write bits to.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @param value (IN) - The bits to write to the buffer.

**/
DIVX_INLINE void WriteUint08(void *pBuffer, uint32_t byteOffs, uint8_t value)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    buff8[0] = value;
}


/*!
    Function to read an unsigned 8 bit integer from a buffer.

    @param pBuffer (IN) - Bitstream to read bits from.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @return the 8-bit value
**/
DIVX_INLINE uint8_t ParseUint08(void *pBuffer, uint32_t byteOffs)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    return buff8[0];
}


/*!
    Function to write an unsigned 16 bit integer to the buffer.

    @param pBuffer (IN/OUT) - Bitstream to write bits to.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @param value (IN) - The bits to write to the buffer.

**/
DIVX_INLINE void WriteUint16(void *pBuffer, uint32_t byteOffs, uint16_t value)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    buff8[0] = (value & 0xFF00) >> 8;
    buff8[1] = (value & 0x00FF) >> 0;
}


/*!
    Function to read an unsigned 16 bit integer from a buffer.

    @param pBuffer (IN) - Bitstream to read bits from.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @return the 16-bit value
**/
DIVX_INLINE uint16_t ParseUint16(void *pBuffer, uint32_t byteOffs)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    return ( (buff8[0] << 8) | buff8[1] );
}


/*!
    Function to write an unsigned 24 bit integer to the buffer.

    @param pBuffer (IN/OUT) - Bitstream to write bits to.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @param value (IN) - The bits to write to the buffer.

**/
DIVX_INLINE void WriteUint24(void *pBuffer, uint32_t byteOffs, uint32_t value)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    buff8[0] = (value & 0x00FF0000) >> 16;
    buff8[1] = (value & 0x0000FF00) >> 8;
    buff8[2] = (value & 0x000000FF) >> 0;
}


/*!
    Function to read an unsigned 24 bit integer from a buffer.

    @param pBuffer (IN) - Bitstream to read bits from.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @return the 24-bit value
**/
DIVX_INLINE uint32_t ParseUint24(void *pBuffer, uint32_t byteOffs)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    return ( (buff8[0] << 16) | (buff8[1] << 8) | buff8[2] );
}


/*!
    Function to write an unsigned 32 bit integer to the buffer.

    @param pBuffer (IN/OUT) - Bitstream to write bits to.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @param value (IN) - The bits to write to the buffer.

**/
DIVX_INLINE void WriteUint32(void *pBuffer, uint32_t byteOffs, uint32_t value)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    buff8[0] = (value & 0xFF000000) >> 24;
    buff8[1] = (value & 0x00FF0000) >> 16;
    buff8[2] = (value & 0x0000FF00) >> 8;
    buff8[3] = (value & 0x000000FF) >> 0;
}


/*!
    Function to read an unsigned 32 bit integer from a buffer.

    @param pBuffer (IN) - Bitstream to read bits from.
    @param byteOffs (IN) - Number of byte to start with. (zero based)
    @return the 32-bit value
**/
DIVX_INLINE uint32_t ParseUint32(void *pBuffer, uint32_t byteOffs)
{
    uint8_t *buff8 = &((uint8_t *)pBuffer)[byteOffs];

    return ( (buff8[0] << 24) | (buff8[1] << 16) | (buff8[2] << 8) | buff8[3] );
}


#ifdef __cplusplus
}
#endif

#endif //DIVXBITSTREAMUTIL_H

