/*!
    @file
   @verbatim
   $Id: DMFModuleEntryPoints.h 57969 2009-01-14 01:23:14Z fchan $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMODULEENTRYPOINTS_H_
#define _DMFMODULEENTRYPOINTS_H_

#include "DMFFunctionPointers.h"

/* ! Module function pointer collection */
typedef struct _DMFModuleEntryPoints
{
    /* ! int ModuleInit( const void *data );
       ! *required*
       ! Module initialization. Called by L1Init() - should never be called directly.
       ! data is optional but must be accessible as L1InitStruct* (see below) */
    pfnDMFInt_Voidp fnInit;

    /* ! ModuleAttributes * GetCaps()
       ! *required*
       ! Return pointer to static module information */
    pfnDMFModAttr_Void fnGetCaps;

    /* ! DivXError IsContainerSupported( int *SupportLevel, DivXString *Filename )
       ! *required*
       ! Sets *SupportLevel from 0 (unsupported) to 100 (fully supported) for container named
       ! according to Filename */
    pfnDMFErr_IntpStr fnIsContainerSupported;

    /* ! DivXError OpenContainer(void* *ContainerHandle, DivXString *filename, DivXMem hMem)
       ! *required*
       ! Open container identified by Filename, return handle in *ContainerHandle
       ! and DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidppStrMem fnOpenContainer;

    /* ! DivXError CreateContainer(void* *ContainerHandle, DivXString *filename, pfnDMFErr_VoidpIntVariantpIntVoidp InfoCallback, void *CallbackData, DivXMem hMem)
       ! Create container identified by Filename and registers callback with
       ! callback data for data retrieval. Callback needs to persist until
       ! CloseContainer() is called. Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidppStrMem fnCreateContainer;

    /* ! DivXError EditContainer( HContainer containerHandle, DivXString *filename);
       ! Opens an existing container to be edited. */
    pfnDMFErr_VoidppStrMem fnEditContainer;

    /* ! DivXError CloseContainer(void* ContainerHandle);
       ! *required*
       ! Close container handle and free associated resources */
    pfnDMFErr_Voidp fnCloseContainer;

    /* ! DivXError SetTitle(void* ContainerHandle, HTitle TitleID)
       ! Set current title to specified ID. Title ID is gotten via GetInfo(QID_TITLE_ID)
       ! using the origin:0 index of the title. This may cause I/O activity.
       ! Overwrites previous title information exclusive of menu.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpHt fnSetTitle;

    /* ! DivXError SetMenu(void* ContainerHandle, HMenuBlock MenuID)
       ! Set menu block to specified ID. Currently undefined.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpInt fnSetMenu;

    /* ! DivXError GetInfo( HContainer ContainerHandle, int QueryValueID, DMFVariant *Value, int Index )
       ! Type-specific entry points are defined as well as DMFVariant.
       ! Variant can accommodate short strings. Index applies to certain
       ! queries. Query IDs are defined in L1QueryIDs.h.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpIntVariantpInt fnGetInfo;

    /* ! DivXError SetBlock( HContainer hContainer, DivXBlockType_t blockType, HStream nStream, int *pNumBlock, DivXTime *pTime )
       ! Set next block for read operations. Stream ID is required as is *pTime.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypeIntTimep fnSetPosition;

    /* DivXError SetTitleActiveStream(HL1Container hContainer, DivXBlockType_t blockType, uint32_t nStream, DivXBool bActive);
       Sets the stream active to be read back.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypepIntBool fnSetTitleActiveStream;

    /* DivXError ReadNextBlock(HL1Container hContainer, DivXBlockType_t *pBlockType, uint32_t *pnStream, DivXBlockNode_t *pBlockNode, void *userData);
       ! Read next block sequentially from entire file. Caller must first call set active stream on
        the streams they would like to receive.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypepIntpBlockpVoidp fnReadNextBlock;

    /* ! DivXError ReadNextBlockByStream(void* hContainer, DivXBlockType_t blockType, HStream nStream, DivXBlockNode_t *pBlockNode)
       ! Read next block sequentially. This is stream based reading.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypeHsBlockp fnReadNextBlockByStream;

    /* ! DivXError WriteNextBlock(void* hContainer, DivXBlockType_t blockType, HStream nStream, DivXBlockNode_t *pBlockNode)
       ! Write next block.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypeHsBlockp fnWriteNextBlock;

    /* ! DivXError AddStream(void* ContainerHandle, DivXBlockType_t BlockType, void *Data, HStream *StreamHandle)
       ! Adds stream to a newly created container. Returns stream handle of new
       ! stream in *StreamHandle. Data is DivXVideoStreamInfo1_t*, DivXAudioStreamInfo1_t*, etc. depending on type.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypeVoidpHsp fnAddStream;

    /* ! DivXError SetInfo( HContainer ContainerHandle, int QueryValueID, DMFVariant *InputValue, int Index, DMFVariant *OutputValue )
       ! Sets info using the same L1QueryIDs.h values as GetInfo().
       ! Some queries define output as well as input values.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpIntVariantpIntVariantp fnSetInfo;

    /* ! DivXError AddTitle( HContainer containerHandle, HTitle *TitleID )
       ! Add title in addition to implicit title created by CreateContainer
       ! *TitleID is the title handle for the newly created title.
       ! All AddStream operations will occur relative to the newly created title.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpHtp fnAddTitle;

    /* ! DivXError AddMenu( HContainer containerHandle, int headerLength, void *headerData );
       ! Add top-level menu block to a container. DMF only supports 1.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpIntVoidp fnAddMenu;

    /* ! DivXError AddMenuMedia( HContainer containerHandle, HTitle *menuMedia );
       ! Add a menu media segment to top-level menu block in a container.
       ! May be sequence-dependent. Must follow AddMenu(). Returns
       ! DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpHtp fnAddMenuMedia;

    /* ! DivXError SetMenuMedia( HContainer containerHandle, HTitle menuMedia );
       ! Set active menu media (MRIF in DMF). Operates independently of SetTitle
       ! and requires an HTitle returned by GetInfo( QID_MENUTITLE_ID, n ) where
       ! 0 <= n < GetInfo( QID_MENUTITLE_COUNT ) */
    pfnDMFErr_VoidpHt fnSetMenuMedia;

    /* ! DivXError SetMenuMediaBlock( HContainer containerHandle, DivXBlockType_t blockType, HStream nStream, int *pNumBlock, DivXTime *pTime )
       ! Set next block for reads on menu media. nStream is gotten via GetInfo( QID_MENUTITLE_{VIDEO,AUDIO,SUBTITLE}_STREAM_HANDLE, n )
       ! where 0 <= n < GetInfo( QID_MENUTITLE_STREAM_COUNT ) */
    pfnDMFErr_VoidpBtypeIntTimep fnSetMenuMediaPosition;

    /* DivXError SetTitleActiveStream(HL1Container hContainer, DivXBlockType_t blockType, uint32_t nStream, DivXBool bActive);
       Sets the stream active to be read back.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypepIntBool fnSetMenuActiveStream;

    /* ! Read next block sequentially from entire file. Caller must first call set active stream on
        the streams they would like to receive.
       ! Returns DIVX_ERR_SUCCESS if successful. */
    pfnDMFErr_VoidpBtypepIntpBlockpVoidp fnReadNextMenuMediaBlock;

    /* ! DivXError ReadNextMenuMediaBlock(void* hContainer, DivXBlockType_t blockType, HStream nStream, DivXBlockNode_t *pBlockNode)
       ! Read next block from menu media. Must follow SetMenuMediaBlock(). */
    pfnDMFErr_VoidpBtypeHsBlockp fnReadNextMenuMediaBlockByStream;

    /* ! DivXError WriteNextMenuMediaBlock(void* hContainer, DivXBlockType_t blockType, HStream nStream, DivXBlockNode_t *pBlockNode)
       ! Write a block to menu media. */
    pfnDMFErr_VoidpBtypeHsBlockp fnWriteNextMenuMediaBlock;

    /* ! DivXError AddMenuMediaStream(void* ContainerHandle, DivXBlockType_t BlockType, void *Data, HStream *StreamHandle)
       ! Add a stream within menu media. Returns stream handle for newly created stream. */
    pfnDMFErr_VoidpBtypeVoidpHsp fnAddMenuMediaStream;

    /* ! L1ContainerWriteState ResetState( HContainer containerHandle );
       ! Resets current state and returns default initial state for container. */
    pfnDMFCws_Voidp fnResetState;

    /* ! L1ContainerWriteState GetNextState( HContainer containerHandle );
       ! Returns the state currently desired by the L1 container module - in other words, "what do I want to write next?" */
    pfnDMFCws_Voidp fnGetNextState;

    /* ! void NoMore( HContainer containerHandle );
       ! Signals to L1 that there is no more of the requested state data - in other words, "you're asking for xxx and I don't have it..." */
    pfnDMFVoid_Voidp fnNoMore;

    /* ! DivXError FinishTitle( HContainer containerHandle );
       ! Finalizes writes to output container */
    pfnDMFErr_Voidp fnFinishTitle;

    /* ! DivXError FinishMenuRiff( HContainer containerHandle );
       ! Finalizes writes to menu riff */
    pfnDMFErr_Voidp fnFinishMenuRiff;

    /* ! DivXError FinishMRIF( HContainer containerHandle );
       ! Finalizes writes to mrif (menu media title within menu riff) */
    pfnDMFErr_Voidp fnFinishMRIF;

    /* ! DivXError OpenContainer(void* *ContainerHandle, void *file, DivXMem hMem)
    ! *required*
    ! Open container identified by externally opened file supplied as platform dependent descritor, return handle in *ContainerHandle
    ! and DIVX_ERR_SUCCESS if successful. 
    ! This function pointer exists but used for SYMBIAN32 platform only */
    
    // This function pointer allows to pass the platform specific descriptor of externally opened file. 
    // This is done because there is no sopen function in SYMBIAN and we need to use
    // MDataSource interface to read data from file. Trough *ForFile functions the defaced pointer reaches DivXFileOpen function 
    // and casted to exact platform specific file descriptor.
    pfnDMFErr_VoidppVoidpMem fnOpenContainerForFile;
}
DMFModuleEntryPoints;

#endif /* _DMFMODULEENTRYPOINTS_H_ */
