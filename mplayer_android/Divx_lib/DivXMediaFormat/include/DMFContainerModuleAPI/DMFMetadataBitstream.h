/*!
    @file
   @verbatim
   $Id: DMFMetadataBitstream.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMETADATABITSTREAM_H_
#define _DMFMETADATABITSTREAM_H_

#include "DivXInt.h"

typedef struct DMFMetadataBitstream_t
{
    uint8_t *pGraphBitstream;
    uint32_t sizeGraphBitstream;
    uint8_t *pNodeBitstream;
    uint32_t sizeNodeBitstream;
}
DMFMetadataBitstream_t;

#endif /* _DMFMETADATABITSTREAM_H_ */
