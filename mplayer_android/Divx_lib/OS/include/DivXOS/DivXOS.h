/*!
    @file
@verbatim
$Id: DivXOS.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXOS_H_
#define _DIVXOS_H_

#include "DivXError.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DIVX_OS_ERR_OUT_OF_MEMORY -1

/* DivXFileCreate Error codes */
#define DFC_BadFileName -11
#define DFC_CannotCreateInReadOnlyMode -12
#define DFC_MustSpecifyPermissions -13
#define DFC_AccessModeError -14
#define DFC_FileExists -15
#define DFC_InvalidOption -16
#define DFC_TooManyOpenFiles -17
#define DFC_InvalidFileOrPath -18
#define DFC_FileCreateFailed -19

/* DivXFileOpen error codes*/
#define DFO_CannotTruncateInReadOnlyMode -21
#define DFO_CannotAppendInReadOnlyMode -22
#define DFO_AccessModeError -23
#define DFO_FileExists -24
#define DFO_InvalidOption -25
#define DFO_TooManyOpenFiles -26
#define DFO_InvalidFileOrPath -27
#define DFO_FileOpenFailed -28

/* DivXFileSeek  error codes */
#define DFS_BadSeekLocation -31
#define DFS_InvalidHandle -32
#define DFS_InvalidOffset -33

/* DivXFileRead error codes */
#define DFR_BadHandle -41
#define DFR_UnableToRead -42

/* DivXFileWrite error codes */
#define DFW_CantWrite -51
#define DFW_NoSpaceOnDest -52

/* DivXFileClose error codes */
#define DFCL_BadHandle -61

/* DivXLoadTextFile error codes */
#define DLTF_CantOpenFile -71

#ifdef __cplusplus
}
#endif

#endif
