/*!
    @file
@verbatim
$Id: MappingLayerWriteMetaData.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MAPPINGLAYERWRITEMETADATA_H_
#define _MAPPINGLAYERWRITEMETADATA_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"
#include "DMFContainerHandle.h"
#include "DMFMetaDataHandle.h"

#include "DMFTypes.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFModuleInitStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

    DivXError L2CreateMetaData( DMFMetaDataHandle           *phMetaData,
                                DMFContainerHandle          hContainer,
                                DivXMem                     hMem);

    DivXError L2CreateSimpleTag( DMFContainerHandle         hContainer, 
                                 DMFMetaDataSimpleTagHandle *phSimpleTag,
                                 DMFMetaDataElemHandle      hMetaElem,
                                 DivXMem                    hMem);

    DivXError L2CreateMetaElem( DMFContainerHandle          hContainer,
                                DMFMetaDataElemHandle       *phMetaElem,
                                DMFMetaDataHandle           hMetaData,
                                DivXMem                     hMem);

    DivXError L2AddMetaStreamTarget( DMFContainerHandle          hContainer,
                                     DMFMetaDataElemHandle  hMetaElem,
                                     DMFBlockType_t         blockType,
                                     uint32_t               nStreamOfType,
                                     DivXMem                hMem);

    DivXError L2AddMetaChapterTarget( DMFContainerHandle          hContainer,
                                      DMFMetaDataElemHandle  hMetaElem,
                                      uint32_t               index,
                                      uint32_t               atomIndex,
                                      DivXMem                hMem);

    DivXError L2AddMetaEditionTarget( DMFContainerHandle          hContainer,
                                      DMFMetaDataElemHandle  hMetaElem,
                                      uint32_t               index,
                                      DivXMem                hMem);

    DivXError L2AddMetaAttachmentTarget( DMFContainerHandle          hContainer,
                                         DMFMetaDataElemHandle hMetaElem,
                                         uint32_t       index,
                                         DivXMem        hMem);

    DivXError L2AddMetaNameData(  DMFContainerHandle hContainer, 
                                  DMFMetaDataElemHandle hMetaElem, 
                                  DMFMetaDataSimpleTagHandle hSimpleTag,
                                  DivXString* name);

    DivXError L2AddMetaStringData(  DMFContainerHandle hContainer,
                                    DMFMetaDataElemHandle hMetaElem,
                                    DMFMetaDataSimpleTagHandle hSimpleTag,
                                    DivXString* strData);

    DivXError L2AddMetaBinData( DMFContainerHandle          hContainer,
                                DMFMetaDataElemHandle hMetaElem,
                                DMFMetaDataSimpleTagHandle hSimpleTag,
                                void* binData,
                                uint32_t size);

    DivXError L2AssociateMetaData( DMFContainerHandle          hContainer,
                                   DMFMetaDataHandle hMetaData );


#ifdef __cplusplus
}
#endif

#endif 