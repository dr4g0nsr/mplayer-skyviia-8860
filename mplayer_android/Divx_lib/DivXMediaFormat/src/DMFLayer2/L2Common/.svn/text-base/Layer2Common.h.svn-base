/*!
    @file
@verbatim
$Id: Layer2Common.h 56111 2008-09-25 00:47:33Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LAYER2COMMON_H_
#define _LAYER2COMMON_H_

#include "DivXBool.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFContainerHandle.h"
#include "DMFModuleInitStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* / Layer 2 initialization. Must be called before using anything in Layer 2. */
DivXError L2_Init( DMFModuleInitStruct *init );

/* / Find module which supports a particular container by name */
DivXError L2FindModule( int32_t          *ModuleID,
                        const DivXString *ContainerName,
                        DivXBool          bOutputContainer );

DivXError L2_Internal_Initialize_Handle( DMFComponentContainerHandle hContainer );

DivXError GetModuleID( int32_t           *ModuleID,
                       DMFComponentContainerHandle hContainer );

DivXError L2CommonSetInfo( DMFComponentContainerHandle hContainer,
                           int32_t            QueryValueID,
                           DMFVariant        *InputValue,
                           int32_t            Index,
                           DMFVariant        *OutputValue );

DivXError L2GetInfo( DMFComponentContainerHandle hContainer,
                     int32_t            QueryValueID,
                     DMFVariant        *Value,
                     int32_t            Index );

DivXError L2GetMetaInfo( DMFComponentContainerHandle hContainer,
                         int32_t            queryValueID,
                         DMFVariant        *value,
                         int32_t            index );

DivXError L2LoadMetadataInfo( DMFComponentContainerHandle hContainer );

DivXError L2SetMetaInfo( void       *hMetadata,
                         int32_t     queryValueID,
                         DMFVariant *inputValue,
                         int32_t     index,
                         DMFVariant *outputValue );

DivXError L2ReadMetaInfo( DMFComponentContainerHandle hContainer,
                          int32_t            queryValueID,
                          DMFVariant        *value,
                          int32_t            index );

DivXError GetComponentContainer(DMFContainerHandle hContainer, 
                                DMFComponentContainerHandle *pCompHandle, 
                                uint32_t index);

DivXError AllocateAggregateContainer( DMFContainerHandle *hContainer, DivXMem hMem);

DivXError GetNumComponentContainers( DMFContainerHandle hContainer, uint32_t* numContainers);

DivXError CopyBlockNode(DMFBlockNode_t* dest, DMFBlockNode_t* src);

#ifdef __cplusplus
}
#endif

#endif /*_LAYER2COMMON_H_ */
