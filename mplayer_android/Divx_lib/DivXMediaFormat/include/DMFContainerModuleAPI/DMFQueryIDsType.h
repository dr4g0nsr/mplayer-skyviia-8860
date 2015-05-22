/*!
    @file
   @verbatim
   $Id: DMFQueryIDsType.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFQUERYIDSTYPE_H_
#define _DMFQUERYIDSTYPE_H_

/*! Data type definitions. These are 8-bit values
**/
/* this structure cannot exceed 16 entries */
typedef enum
{
    DMF_QIDT_INT8 = 0x01,   /*!< 8-bit int/char */
    DMF_QIDT_UINT8,         /*!< 8-bit unsigned */
    DMF_QIDT_INT16,         /*!< 16-bit int */
    DMF_QIDT_UINT16,        /*!< 16-bit unsigned */
    DMF_QIDT_INT32,         /*!< 32-bit int */
    DMF_QIDT_UINT32,        /*!< 32-bit unsigned */
    DMF_QIDT_INT64,         /*!< 64-bit int */
    DMF_QIDT_UINT64,        /*!< 64-bit unsigned */
    DMF_QIDT_FLOAT,         /*!< Floating point value (double) */
    DMF_QIDT_DSTRP,         /*!< DivXString character string (see DivXString type definition) */
    DMF_QIDT_UINT8P,        /*!< ptr to 8-bit unsigned array of data, see varaint for size */
    NUM_DMF_QIDT            /*!< Number of types supported by the API */
}
DMF_QIDT_ENUM;

/* this structure cannot exceed 16 entries */
typedef enum
{
    DMF_QIDT_NOSTREAM = 0x0,
    DMF_QIDT_VID,
    DMF_QIDT_AUD,
    DMF_QIDT_SUB,
    NUM_DMF_STREAMS
}
DMF_QIDT_STR;

/*!
   Given a sequence and datatype, create a unique identifier for a data item.
   In theory this allows overloading by data type.
 **/
#define QIDVAL( sequence, streamtype, datatype) ( (sequence << 8 ) | (streamtype << 4) | datatype )

/*! Get data type from ID
**/
#define QIDTYPE( id )     ( id & 0x0f )
#define QIDSTREAM( id )     ( (id & 0xf0) >> 4 )
#define QID_STREAM_BASED( id ) ( id & 0xf0 )
#endif /* _DMFQUERYIDSTYPE_H_ */
