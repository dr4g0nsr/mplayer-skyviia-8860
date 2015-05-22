/*!
    @file
   @verbatim
   $Id: DMFVariant.h 56354 2008-10-06 01:02:30Z sbramwell $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFVARIANT_H_
#define _DMFVARIANT_H_

#include "DivXInt.h"
#include "DivXString.h"
#include "DMFVariant.h"

/*! Variant for type-independent information retrieval. This has a built-in 256 character string allocation.
**/

typedef struct _DMFVariant
{
    uint8_t  Type;          /*!< DMF_QIDT_INT8, ... (from DMFQueryIDs.h) */
    uint32_t Length;        /*!< Used only with DMF_QIDT_UINT8P */
    union _v
    {
        int8_t      i8;
        uint8_t     u8;
        int16_t     i16;
        uint16_t    u16;
        int32_t     i32;
        uint32_t    u32;
        int64_t     i64;
        uint64_t    u64;
        double      f;
        float       spf;
        DivXString *s;      /*!< This is the only supported string type in variant */
        uint8_t    *u8p;    /*!< Use this to send raw buffers of data */
    }
    v;
}
DMFVariant;

#endif /* _DMFVARIANT_H_ */
