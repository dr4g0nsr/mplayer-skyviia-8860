/*!
    @file
   @verbatim
   $Id: DivXBitrateCalculator.h 37201 2007-07-25 18:43:55Z jbraness $

   Copyright (c) 2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DIVXBITRATECALCULATOR_H_
#define _DIVXBITRATECALCULATOR_H_

#include "DivXMem.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BRC_session  *DivXBrCalcHandle;

DivXBrCalcHandle    DivXBrCalcNewSession( uint32_t maxSamples,
                                          DivXMem  memHeap );

void        DivXBrCalcReset( DivXBrCalcHandle DivXBrCalcSession );

void        DivXBrCalcDeleteSession( DivXBrCalcHandle DivXBrCalcSession );

void        DivXBrCalcAddSample( DivXBrCalcHandle DivXBrCalcSession,
                                 uint32_t         sampleSize );

uint32_t    DivXBrCalcGetBitrate( DivXBrCalcHandle DivXBrCalcSession,
                                  uint32_t         sampleSize );

uint32_t    DivXBrCalcGetSleep( DivXBrCalcHandle divXBrCalcSession,
                                uint32_t         sampleSize,
                                uint32_t         maxBitrate );

#ifdef __cplusplus
}
#endif

#endif  /* _DIVXBITRATECALCULATOR */
