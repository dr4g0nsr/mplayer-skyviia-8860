/*!
    @file
   @verbatim
   $Id: DMFMetadataEntryPoints.h 57969 2009-01-14 01:23:14Z fchan $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMETADATAENTRYPOINTS_H_
#define _DMFMETADATAENTRYPOINTS_H_

#include "DMFFunctionPointers.h"

typedef struct _MetadataEntryPoints_t
{
    /*! DivXError MetadataCreateNew(hDivXMetadata *pHandle, DivXMem hMemory); */
    /*! Creates an instance of the metadata module */
    pfnDMFErr_VoidpMem fnMetadataCreateNew;

    /*! DivXError MetadataDelete(hDivXMetadata handle); */
    /*! Deletes the metadata instance. */
    pfnDMFErr_Voidp fnMetadataDelete;

    /*! DivXError MetadataGetInfo(hDivXMetadata handle, int queryValueID, DMFVariant *value, int index); */
    /*! Function to get metadata info */
    pfnDMFErr_VoidpIntVariantpInt fnMetadataGetInfo;

    /*! DivXError MetadataSetInfo(hDivXMetadata handle, int queryValueID, DMFVariant *inputValue, int index, DMFVariant *outputValue ); */
    /*! Function to set metadata info */
    pfnDMFErr_VoidpIntVariantpIntVoidp fnMetadataSetInfo;

    /*! DF3 specific stuff. */
    pfnDMFErr_VoidppVoidpMem fnMetaDataCreateNewDF3;
    pfnDMFErr_VoidppVoidpMem fnMetaDataCreateElemDF3;
    pfnDMFErr_VoidpVoidppMem fnMetaDataCreateSimpleTagDF3;
    pfnDMFErr_VoidpBtypeUintMem fnMetaDataAddStreamTarget;
    pfnDMFErr_VoidpUintUintMem fnMetaDataAddChapterTarget;
    pfnDMFErr_VoidpUintMem fnMetaDataAddEditionTarget;
    pfnDMFErr_VoidpUintMem fnMetaDataAddAttachmentTarget;
    pfnDMFErr_VoidpVoidpStr fnMetaDataAddName;
    pfnDMFErr_VoidpVoidpStr fnMetaDataAddString;
    pfnDMFErr_VoidpVoidpVoidpUint fnMetaDataAddBin;
    pfnDMFErr_Voidp fnMetaDataAssociate;
    pfnDMFErr_VoidpStrStr fnMetaDataAddTitleData;
    pfnDMFErr_VoidpStrStr fnMetaDataAddCopyrightData;
    pfnDMFErr_VoidpStrStr fnMetaDataAddDescriptionData;
    pfnDMFErr_VoidpStrStr fnMetaDataAddURLData;
    pfnDMFErr_VoidpStrVoidpUint fnMetaDataAddJPEGData;
}
MetadataEntryPoints_t;

#endif /* _DMFMETADATAENTRYPOINTS_H_ */
