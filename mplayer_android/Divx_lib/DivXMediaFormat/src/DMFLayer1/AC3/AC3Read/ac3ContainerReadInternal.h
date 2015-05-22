/*!
    @file
@verbatim
$Id: ac3ContainerReadInternal.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AC3CONTAINERREADINTERNAL_H_
#define _AC3CONTAINERREADINTERNAL_H_

#include "DivXTypes.h"
#include "DivXError.h"
#include "./AC3/AC3Read/ac3ContainerReadStructures.h"
#include "./AC3/AC3Common/ac3FrameHeader.h"
#include "DMFAudioStreamInfo1.h"

/**
    Reads a block from the ac3 container.

    @param pAc3Container (IN) - The container structure.
    @param pAc3FrameHeader (OUT) - The ac3 frame header structuct for the frame read.
    @param pBlockNode - (OUT) - The block node where the actual frame buffer is.

    @note The pBlockNode can be null if the data for the block is not needed.  It is also possible for
    the pBlockNode->pBuffer parameter to be null to just get back the timing information for the block
    without the actual frame buffer being copied over.
 */
DivXError L1ac3r_Internal_ReadNextBlock( Ac3Container_t     *pAc3Container,
                                         ac3_frame_header_t *pAc3FrameHeader,
                                         DMFBlockNode_t     *pBlockNode );

/**
    Gets a blocks offset.

    @param pAc3Container (IN) - The container structure.
    @param nBlock (IN) - The number of the block to get the offset for.
    @param pOffset (OUT) - The actual block offset.
 */
DivXError L1ac3r_Internal_GetBlocksOffset( Ac3Container_t *pAc3Container,
                                           int32_t         nBlock,
                                           uint64_t       *pOffset );

/**
    Finds the first frame in the ac3 container.  It is possible for it to not
    start at offset zero.

    @param pAc3Container (IN) - The container structure.
    @param pByteOffset (OUT) - The offset of the first frame.
 */
DivXError L1ac3r_Internal_FindFirstFrame( Ac3Container_t *pAc3Container,
                                          int32_t        *pByteOffset );

/**
    Reads the frame header from a specific offset.

    @param pAc3Container (IN) - The container structure.
    @param nFrameOffset (IN) - The offset of the start of the frame.
    @param ac3FrameHeader (OUT) - The ac3 frame header.
 */
DivXError L1ac3r_Internal_ReadFrameHeader( Ac3Container_t     *pAc3Container,
                                           int32_t             nFrameOffset,
                                           ac3_frame_header_t *ac3FrameHeader );

/**
    Reads the frame data.  The function assumes that the current position is
    just after the ac3 frame header.  The ac3 frame header is copied from an
    internal buffer.

    @param pAc3Container (IN) - The container structure.
    @param frameSize (IN) - The size of the frame (including ac3 frame header).
    @param pData (OUT) - The actual ac3 frame (including ac3 frame header).

 */
DivXError L1ac3r_Internal_ReadFrameData( Ac3Container_t *pAc3Container,
                                         int32_t         frameSize,
                                         uint8_t        *pData );

/**
    Calculates the size of the frame (including the size of the frame header).

    @param ac3FrameHeader (IN) - The ac3 frame header.
 */
int32_t L1ac3r_Internal_CalcFrameSize( ac3_frame_header_t *ac3FrameHeader );

/**
    Tests the stream to see if it is a vbr or cbr stream.

    @param pAc3Container (IN) - The container structure.
    @param nBlocks (IN) - The number of blocks to test.

 */
DivXError L1ac3r_Internal_TestForVBR( Ac3Container_t *pAc3Container,
                                      int32_t         nBlocks );

/**
    Tests to see if the stream has been parsed fully already.  If not it calls
    test for vbr if so it just returns the saved value.

    @param pAc3Container (IN) - The container structure.
 */
DivXBool L1ac3r_Internal_IsVariableBitrate( Ac3Container_t *pAc3Container );

DivXError L1ac3r_Internal_SetBlockFromTime( Ac3Container_t *pAc3Container,
                                            int32_t        *pNumBlock,
                                            DivXTime       *pTime );

DivXError L1ac3r_Internal_SetBlockFromBlock( Ac3Container_t *pAc3Container,
                                             int32_t        *pNumBlock,
                                             DivXTime       *pTime );

DivXError L1ac3r_Internal_GetAudioInfo( Ac3Container_t        *pAc3Container,
                                        DMFAudioStreamInfo1_t *audStreamInfo );

DivXError L1ac3r_Internal_FindNextFrame( Ac3Container_t *pAc3Container,
                                         int32_t        *pByteOffset );

DivXError L1ac3r_Internal_ParseFrameHeader( Ac3Container_t     *pAc3Container,
                                            uint8_t            *curFrameHeader,
                                            ac3_frame_header_t *ac3FrameHeader );

#endif /* _AC3CONTAINERREADINTERNAL_H_ */
