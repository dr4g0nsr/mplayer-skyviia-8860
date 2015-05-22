/*!
    @file
@verbatim
$Id: avir1Internal.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVI1INTERNAL_H_
#define _AVI1INTERNAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "DivXFile.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#include "./AVI/AVICommon/L1StreamName.h"
#include "./AVI/AVICommon/AVIStructures.h"
#include "./AVI/AVI1Read/ReadBuffer.h"
#include "./AVI/AVI1ReadMenu/AVI1MenuParse.h"

/* #define AVI1_AVI_BUFFER_SIZE 8192 */
#define AVI1_AVI_BUFFER_SIZE 128 * 1024  /* 64 * 32 */
/* #define AVI1_MENU_BUFFER_SIZE 128*1024 */
#define AVI1_MENU_BUFFER_SIZE 8 * 1024
#define AVI1_DRM_STRD_SIZE 2232
#define AVI1_DRM_STRD_VERSION 2

#ifdef __cplusplus
}
#endif
#endif /* _AVI1INTERNAL_H_ */
