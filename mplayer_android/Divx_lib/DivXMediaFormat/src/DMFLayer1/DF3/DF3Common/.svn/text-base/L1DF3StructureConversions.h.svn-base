/*!
    @file L1DF3StructureConversions.h
@verbatim
$Id: L1DF3StructureConversions.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2007-2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/


#ifndef _L1DF3STRUCTURECONVERSONS_H_
#define _L1DF3STRUCTURECONVERSONS_H_

#include "DivXBool.h"
#include "./DF3/DF3Common/MKVStructures.h"
#include "./DF3/DF3Common/MKVTypes.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"
#include "DMFBlockType.h"

#ifdef __cplusplus
extern "C" {
#endif

DivXBool IsVBRAudio( DMFAudioStreamInfo1_t *pData );
void GenericVideoFormatToMKVPrivateData(DMFVideoStreamInfo1_t *pVidFormat, MKVVideoPrivateData_t* mkvPrivateData);
DMFBlockType_t MKVTrackTypeToDMFBlockType(MKV_TRACK_TYPE_e mkvTrackType);
MKV_TRACK_TYPE_e DMFBlockTypeToMKVTrackType(DMFBlockType_t dmfBlockType);



#ifdef __cplusplus
}
#endif

#endif /*_L1DF3STRUCTURECONVERSONS_H_*/