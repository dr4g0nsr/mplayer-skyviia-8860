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

#ifndef _LIBPLAYBACKENGINEUTIL_H_
#define _LIBPLAYBACKENGINEUTIL_H_

#include "DivXMem.h"
#include "DivXInt.h"
#include "DivXTime.h"
#include "DivXString.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"

#include "PEUCommands.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PEUInt_t * PEUHandle;

typedef int32_t (*pfnReceiveCallback)( void *privateData, DMFBlockType_t blockType, uint32_t nStream, DMFBlockNode_t *pBlockNode);

typedef int32_t (*pfnCommandCallback) ( void *privateData, PEUCommands_e command, int64_t value );

typedef int32_t (*pfnCommandFinishedCallback) ( void *privateData, PEUCommands_e command, int64_t value );

DivXError PEU_New( PEUHandle *pHandle, DivXMem hMem );

DivXError PEU_Delete( PEUHandle handle );

DivXError PEU_OpenMedia( PEUHandle handle, const DivXString *pName, const DivXString* sGoldenFile );

DivXError PEU_SendCommand( PEUHandle handle, PEUCommands_e command, int64_t value );

DivXError PEU_SetReceiveCallback( PEUHandle handle, pfnReceiveCallback fnReceiveCallback, void *privateData );

DivXError PEU_SetCommandCallback( PEUHandle handle, pfnCommandCallback fnCommandCallback, void *privateData );

DivXError PEU_SetCommandFinishedCallback( PEUHandle handle, pfnCommandFinishedCallback fnCommandFinishedCallback, void *privateData );

DivXBool PEU_ErrorExists(PEUHandle handle, uint32_t* errCount);

DivXError PEU_GetError(PEUHandle handle, char* errString);

DivXError PEU_Play( PEUHandle handle );

DivXError PEU_CloseMedia(PEUHandle handle);

#ifdef __cplusplus
}
#endif

#endif //_LIBPLAYBACKENGINEUTIL_H
