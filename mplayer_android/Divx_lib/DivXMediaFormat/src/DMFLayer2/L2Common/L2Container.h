/*!
    @file
@verbatim
$Id: L2Container.h 56111 2008-09-25 00:47:33Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L2CONTAINER_H_
#define _L2CONTAINER_H_

#include "DivXMem.h"
#include "DivXBool.h"
#include "DMFFunctionPointers.h"
#include "DMFContainerWriteState.h"
#include "DMFContainerHandle.h"


typedef enum __L2ContainerTypes_t__
{
    L2CONTAINER = 100,
    L2AGGREGATECONTAINER
} L2ContainerTypes;

typedef struct _BlockBuffer_
{
    DMFBlockType_t *pblockType;
    uint32_t       *pnStream;
    DMFBlockNode_t *pBlockNode;
    void           *userData;
    uint32_t       needBlock;
} BlockBuffer, *pBlockBuffer;


#define MAX_TITLES 5
typedef struct __L2Container_t__
{
    L2ContainerTypes                     myType;
    int32_t                              ModuleID; /* /< Module ID for container */
    void                                *L1ContainerHandle; /* /< Handle the the Level 1 container; */
//    void                                *hDMFInterleaver; /* /< Handle to the Interleaver. */
    int32_t                              numTitles;       /* /< number of titles in the container */
    int32_t                              curTitle;        /* /< current title being worked on */
    void                                *hDMFInterleavers[MAX_TITLES]; /* handle to a particular interleavers title */
    void                                *hMetadata; /* /< Handle to the Metadata Module. */
    DMFContainerWriteState               currentState; /* /< Current write state of output container */
    int32_t                              currentNumOfState; /* /< The current number of the state */
    pfnDMFGetInfoErr_IntVariantpIntVoidp GetInfoHighLevelModel;    /* /< GetInfo call to hl model */
    void                                *privateGetInfoCallbackData; /* /< GetInfo private data */
    pfnDMFSetInfoErr_IntVariantpIntVoidp SetInfoHighLevelModel;    /* /< SetInfo call to hl model */
    void                                *privateSetInfoCallbackData; /* /< SetInfo private data */
    DivXBool                             bInteruptSignaled; /* /< Interupt command signalled */
    DivXMem                              hMem; /* /< Memory handle */
    pBlockBuffer                         blockBuffer; /* todo rename to be more descriptive of use */
} L2Container_t;

typedef struct __L2AggregateContainer_t__
{
    L2ContainerTypes                    myType;
    L2Container_t                       **containerHandles;
    uint32_t                            numContainers;
    DivXMem                             hMem;
} L2AggregateContainer_t;

#endif /* _L2CONTAINER_H_ */
