/*!
    @file
   @verbatim
   $Id: DMFBlockNode.h 60063 2009-05-11 22:42:32Z snaderi $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFBLOCKNODE_H_
#define _DMFBLOCKNODE_H_

#include "DivXInt.h"
#include "DivXTime.h"
#include "DMFBlockType.h"


typedef struct _DMFBlockNode_t DMFBlockNode_t;

/*!
    Callback function that will be called prior to 
    DMFBlockNode_t->blockBuffer being written to
 **/
typedef DivXError (*pfnBlockAllocateCallback)( unsigned char **pBlockBuffer, uint32_t blockSize );

/*!
    This structure is used to pass blocks of data from the parser to the
    upper level applications through functions such as ReadNextBlock.
 **/

struct _DMFBlockNode_t
{
    unsigned char              *blockBuffer;                /*!< frame data */
    uint32_t                    blockSize;                  /*!< frame size */
    uint32_t                    allocatedSize;              /*!< number of bytes allocated in frameBuffer */
    uint32_t                    flags;                      /*!< (for keyframes and misc) */
    DivXTime                    absoluteStartTime;          /*!< the absolute time to display  */
    DivXTime                    blockPeriod;                /*!< the time to display this frame */
    DMFBlockType_t              type;                       /*!< the block type */
    uint8_t                    *drmBuffer;                  /*!< the drm data */
    uint32_t                    drmDataSize;                /*!< the actual size of the drm data */
    uint32_t                    drmBufferSize;              /*!< the total size of the drm buffer */
    uint32_t                    fieldSplitOffset;           /*!< offset that separates fields when fields are being combined in a single block */
    pfnBlockAllocateCallback    fnBlockAllocateCallback;    /*!< callback function called prior writing to blockBuffer */
    struct _DMFBlockNode_t *next;       /*!< this is used by the interleaver the interleaver should create its own */
};

#endif /* _DMFBLOCKNODE_H_ */
