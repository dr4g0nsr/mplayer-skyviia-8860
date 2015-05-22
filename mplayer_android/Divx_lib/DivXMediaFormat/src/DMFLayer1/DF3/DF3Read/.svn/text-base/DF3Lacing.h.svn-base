/*!
    @file
@verbatim
$Id: DF3Lacing.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3LACING_H_
#define _DF3LACING_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXMem.h"
#include "DMFBlockNode.h"
//#include "MKVCache.h"
#include "DF3/DF3Common/DMFInputStream.h"
#include "DF3/DF3Common/EBMLElementReader.h"

#define MAX_DF3_LACE_ENTRIES 256

typedef enum {
    DF3_INVISIBLE_LACING = 0,
    DF3_EBML_LACING,
    DF3_FIXEDSIZE_LACING,
    DF3_XIPH_LACING,
    DF3_DISCARDABLE_LACING
} DF3Lacing_e;

typedef enum {
    DF3_DELACING_NOT_INIT = 0,
    DF3_DELACING_OFF,
    DF3_DELACING_ON,
    DF3_DELACING
} DF3DelacingState_e;

#define BLOCK_FLAG_INVISIBLE            0x08
#define BLOCK_FLAG_EBML_LACING          0x06
#define BLOCK_FLAG_FIXEDSIZE_LACING     0x04
#define BLOCK_FLAG_XIPH_LACING          0x02
#define BLOCK_FLAG_DISCARDABLE          0x01
#define BLOCK_FLAG_LACING_MASK          0x06

typedef struct _DF3Lacing_t* DF3LacingHandle;

/*! Create a new lacing object

    @param handle          (OUT) - Returns a DF3Lace handle
    @param hInputStream     (IN) - Input stream handle
    @param hMem             (IN) - Memory handle for object allocation

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_Create(DF3LacingHandle* handle, DMFInputStreamHandle hInputStream, DivXMem hMem);

/*! Destroy a lacing object

    @param handle           (IN)  - A DF3Lace handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_Destroy(DF3LacingHandle handle);

/*! Sets the delacing state

    @param handle           (IN)  - A DF3Lace handle
    @param state            (IN) - Sets the delacing state

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_SetState(DF3LacingHandle handle, DF3DelacingState_e state);

/*! Gets the delacing state

    @param handle            (IN) - A DF3Lace handle
    @param pState           (OUT) - Gets the delacing state

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_GetState(DF3LacingHandle handle, DF3DelacingState_e* pState);

/*! Resets the delacing state

    @param handle            (IN) - A DF3Lace handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_Reset(DF3LacingHandle handle, uint64_t* uiLastOffset);


/*! Sets the offset of the lacing object

    @param handle            (IN) - A DF3Lace handle

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_SetOffset(DF3LacingHandle handle, uint64_t offset);


/*! Takes care of all lacing/delacing operations based on uiFlag settings

    @param handle           (IN) - A DF3Lacing handle
    @param uiFlag           (IN) - Flag for determining lacing status in the block
    @param uiSize           (IN) - Size of the block
    @param pBlockNode      (OUT) - Returns the block node that was retrieved, if one was there to retrieve
    @param pbSuccess       (OUT) - Returns whether lacing occured or not
    @param puiLastOffset   (OUT) - Returns the last offset location

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_HandleLacing(DF3LacingHandle handle, uint8_t uiFlags, uint64_t uiSize, DMFBlockNode_t *pBlockNode, DivXBool* pbSuccess, uint64_t* puiLastOffset);

/*! Delaces if in a delacing mode

    @param handle           (IN)  - A DF3Lace handle
    @param pbContinue       (OUT) - Returns whether we should conntinue or exit with data

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_Read(DF3LacingHandle handle, DMFBlockNode_t *pBlockNode, uint64_t* uiLastOffset, DivXBool* pbContinue);

/*! Sets the absolute start time 

    @param handle           (IN)  - A DF3Lace handle
    @param startTime        (IN)  - Sets the absoluteStartTime for this lacing object

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_SetAbsoluteStartTime(DF3LacingHandle handle, DivXTime startTime);

/*! Reads the absolute start time for the block node based on the lacing data

    @param handle           (IN)  - A DF3Lace handle
    @param pBlockNode       (OUT) - Sets the block node absoluteStartTime parameter

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError df3Lacing_ReadAbsoluteStartTime(DF3LacingHandle handle, DMFBlockNode_t *pBlockNode);


#endif
/* _DF3LACING_H_ */
