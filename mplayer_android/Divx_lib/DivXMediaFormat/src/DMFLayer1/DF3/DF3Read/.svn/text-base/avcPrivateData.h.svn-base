/*!
    @file
@verbatim
$Id: avcContainerRead.c 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/


#ifndef _AVCPRIVATEDATA_H_
#define _AVCPRIVATEDATA_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DMFErrors.h"
#include "avcContainerReadStructures.h"

DivXError df3_Internal_ParseSPS(Seq_Parameter_Set_Data_t * sps, 
                                 uint8_t *spsbufferraw, 
                                 uint32_t size,
                                 DivXBool *b_arFlag,
                                 uint32_t *arBitOffset);

void WriteUintBits(uint8_t *outBuffer, uint32_t outBitOffset, uint8_t *inBuffer, uint32_t inBitOffset, uint32_t count);

#endif /*AVCPRIVATEDATA*/