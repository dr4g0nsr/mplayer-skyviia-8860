/*!
    @file
@verbatim
$Id: mp3ContainerReadInternal.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _MP3CONTAINERREADINTERNAL_H_
#define _MP3CONTAINERREADINTERNAL_H_

#include "DivXTypes.h"
#include "DivXError.h"
#include "mp3ContainerReadStructures.h"
#include "DMFAudioStreamInfo1.h"
#include "L1ContainerHandle.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
    Reads a block from the mp3 container.

    @param pMp3Container (IN) - The container structure.
    @param pMp3FrameHeader (OUT) - The mp3 frame header structuct for the frame read.
    @param pBlockNode - (OUT) - The block node where the actual frame buffer is.

    @note The pBlockNode can be null if the data for the block is not needed.  It is also possible for
    the pBlockNode->pBuffer parameter to be null to just get back the timing information for the block
    without the actual frame buffer being copied over.
 */
DivXError L1mp3r_Internal_ReadNextBlock( Mp3Container_t     *pMp3Container,
                                         mp3_frame_header_t *pMp3FrameHeader,
                                         DMFBlockNode_t     *pBlockNode );

/**
    Gets a blocks offset.

    @param pMp3Container (IN) - The container structure.
    @param nBlock (IN) - The number of the block to get the offset for.
    @param pOffset (OUT) - The offset of the block
 */
DivXError L1mp3r_Internal_GetBlocksOffset( Mp3Container_t *pMp3Container,
                                           int32_t         nBlock,
                                           uint64_t       *pOffset );

/**
    Finds the first frame in the mp3 container.  It is possible for it to not
    start at offset zero.

    @param pMp3Container (IN) - The container structure.
    @param pByteOffset (OUT) - The offset of the first frame.
 */
DivXError L1mp3r_Internal_FindFirstFrame( Mp3Container_t *pMp3Container,
                                          int32_t        *pByteOffset );

/**
    Reads the frame header from a specific offset.

    @param pMp3Container (IN) - The container structure.
    @param nFrameOffset (IN) - The offset of the start of the frame.
    @param mp3FrameHeader (OUT) - The mp3 frame header.
 */
DivXError L1mp3r_Internal_ReadFrameHeader( Mp3Container_t     *pMp3Container,
                                           int32_t             nFrameOffset,
                                           mp3_frame_header_t *mp3FrameHeader );

/**
    Reads the frame data.  The function assumes that the current position is
    just after the mp3 frame header.  The mp3 frame header is copied from an
    internal buffer.

    @param pMp3Container (IN) - The container structure.
    @param frameSize (IN) - The size of the frame (including mp3 frame header).
    @param pData (OUT) - The actual mp3 frame (including mp3 frame header).

 */
DivXError L1mp3r_Internal_ReadFrameData( Mp3Container_t *pMp3Container,
                                         int32_t         frameSize,
                                         unsigned char  *pData );

/**
    Calculates the size of the frame (including the size of the frame header).

    @param mp3FrameHeader (IN) - The mp3 frame header.
 */
int32_t L1mp3r_Internal_CalcFrameSize( mp3_frame_header_t *mp3FrameHeader );

/**
    Tests the stream to see if it is a vbr or cbr stream.

    @param pMp3Container (IN) - The container structure.
    @param nBlocks (IN) - The number of blocks to test.

 */
DivXError L1mp3r_Internal_TestForVBR( Mp3Container_t *pMp3Container,
                                      int32_t         nBlocks );

DivXError L1mp3r_Internal_ParseFrameHeader( Mp3Container_t     *pMp3Container,
                                            unsigned char      *curFrameHeader,
                                            mp3_frame_header_t *mp3FrameHeader );

DivXError L1mp3r_Internal_GetFrameHeader( Mp3Container_t   *pMp3Container,
                                          mp3_frame_header_t *mp3FrameHeader );

DivXError L1mp3r_Internal_SetBlockFromTime( Mp3Container_t *pMp3Container,
                                            int32_t        *pNumBlock,
                                            DivXTime       *pTime );

DivXError L1mp3r_Internal_SetBlockFromBlock( Mp3Container_t *pMp3Container,
                                             int32_t        *pNumBlock,
                                             DivXTime       *pTime );

DivXBool Mp3_ContainerRead_IsVariableBitrate( Mp3Container_t *pMp3Container );

DivXError L1mp3r_Internal_GetAudioInfo( Mp3Container_t        *pMp3Container,
                                        DMFAudioStreamInfo1_t *audStreamInfo );

DivXError L1mp3r_Internal_FindNextFrame( Mp3Container_t *pMp3Container,
                                         int32_t        *pByteOffset );

#ifdef __cplusplus
}
#endif

#endif /* _MP3CONTAINERREADINTERNAL_H_ */
