/*!
    @file
@verbatim
$Id: DF3File.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DF3FILE_H_
#define _DF3FILE_H_

#include "DF3/DF3Common/DMFInputStream.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DivXInt.h"
#include "DivXError.h"
#include "EBMLMap.h"
#include "DF3Parser.h"

typedef struct _DF3File * DF3File_h;

/*! 
    @note Opens a DF3 file

    @param hFile            (IN) - Pointer to DF3File handle
    @param filename         (IN) - File name to open
    @param hMem             (IN) - Memory handle

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_Open( DF3File_h           *FileHandle,
                        const DivXString    *filename,
                        DivXMem             hMem );

/*! 
    @note Closes a DF3 file

    @param hFile            (IN) - DF3File handle

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_Close( DF3File_h FileHandle);

/*! 
    @note Sets the current working title of the DF3 file

    @param hFile            (IN) - DF3File handle
    @param TitleIndex        (IN) - The title index to select

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_SetTitle( DF3File_h hFile,
                           int32_t           TitleIndex );

/*! 
    @note Retrieves queries from the DF3 container

    @param hFile            (IN) - DF3File handle
    @param QueryValueID      (IN) - Query ID to retrieve
    @param Value            (OUT) - Retrieved query data
    @param Index             (IN) - Index of queried item to retrieve

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_GetInfo( DF3File_h        hFile,
                          int32_t           QueryValueID,
                          DMFVariant       *Value,
                          int32_t           Index );

/*! 
    @note Set the titles active streams

    @param hFile            (IN) - DF3File handle
    @param blockType         (IN) - Stream type to set active/inactive
    @param nStream           (IN) - Stream to set active/inactive
    @param bActive           (IN) - Active (DIVX_TRUE) or inactive (DIVX_FALSE) setting

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_SetTitleActiveStream( DF3File_h         hFile,
                                        DMFBlockType_t    blockType,
                                        int32_t           nStream,
                                        DivXBool          bActive );

/*! 
    @note Retrieves the next active block from the active title

    @param hFile            (IN)  - DF3File handle
    @param pBlockType       (OUT) - Returns the block type retrieved
    @param pnStream         (OUT) - Returns the stream type retrieved
    @param pBlockNode       (OUT) - Data from the block
    @param userData         (OUT) - Additional user defined data

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_ReadNextBlock( DF3File_h     hFile,
                                DMFBlockType_t   *pBlockType,
                                uint32_t         *pnStream,
                                DMFBlockNode_t   *pBlockNode,
                                void             *userData );

/*! 
    @note Retrieves the next block by stream and index

    @param hFile            (IN)  - DF3File handle
    @param blockType        (IN)  - Block type to retrieve
    @param nStream          (IN)  - Index of this block type
    @param pBlockNode       (OUT) - Data from the block

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_ReadNextBlockByStream(    DF3File_h       hFile,
                                            DMFBlockType_t  blockType,
                                            uint32_t        nStream,
                                            DMFBlockNode_t  *pBlockNode);

/*! 
    @note Sets data to the file (used primarily in menus)

    @param hFile             (IN) - DF3File handle
    @param QueryValueID      (IN) - Query ID to retrieve
    @param InputValue       (OUT) - Retrieved query data
    @param Index             (IN) - Index of queried item to retrieve
    @param OutputValue      (OUT) - Retrieved output value

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_SetInfo( DF3File_h        hFile,
                          int32_t           QueryValueID,
                          DMFVariant       *InputValue,
                          int32_t           Index,
                          DMFVariant       *OutputValue );

/*! 
    @note Sets the current position for ReadNextBlock and ReadNextBlockByStream

    @param hFile             (IN) - DF3File handle
    @param blockType         (IN) - Block type to set position on
    @param nStream           (IN) - Stream index to set position on
    @param pTime         (IN/OUT) - Time to set position to (returns time actually set)

    @return DivXError       (OUT) - Error code (see DivXError.h)
*/
DivXError DF3File_SetPosition( DF3File_h        hFile,
                              DMFBlockType_t    blockType,
                              int32_t           nStream,
                              DivXTime         *pTime );

/*! 
    @note Retrieves the duration of the file

    @param hFile             (IN) - DF3File handle

    @return double       (OUT) - duration of the file as a double
*/
double DF3File_GetDuration(DF3File_h hFile);

/*! 
    @note Retrieves the file timecode scale

    @param hFile             (IN) - DF3File handle

    @return double       (OUT) - timecode scale in uint64_t
*/
uint64_t DF3File_GetTimecodeScale(DF3File_h hFile);

/*! 
    @note Retrieves segment uid

    @param hFile              (IN) - DF3File handle
    @param uid               (OUT) - uid

    @return double       (OUT) - timecode scale in uint64_t
*/
void DF3File_GetSegmentUID(DF3File_h hFile, unsigned char *uid);

/*! 
    @note Retrieves previous uid

    @param hFile             (IN) - DF3File handle
    @param uid               (OUT) - uid

    @return void       (OUT) - void
*/
void DF3File_GetPrevUID(DF3File_h hFile, unsigned char *uid);

/*! 
    @note Retrieves next uid

    @param hFile             (IN) - DF3File handle
    @param uid               (OUT) - uid

    @return void       (OUT) - void
*/
void DF3File_GetNextUID(DF3File_h hFile, unsigned char *uid);

/*! 
    @note Retrieve the file name length

    @param hFile             (IN) - DF3File handle

    @return double       (OUT) - timecode scale in uint32_t
*/
uint32_t DF3File_GetFilenameLength(DF3File_h hFile);

/*! 
    @note Retrieve the file name

    @param hFile             (IN) - DF3File handle

    @return double       (OUT) - timecode scale in uint32_t
*/
void DF3File_GetFilename(DF3File_h hFile, DivXString *sFilename, uint32_t size);

/*! 
    @note Retrieve the previous sync point

    @param hFile             (IN) - DF3File handle

    @return DivXError       (OUT) - DivXError
*/
DivXError DF3File_GetPreviousSyncPoint(DF3File_h hFile, DMFVariant *Value);

/*! 
    @note Retrieve the next sync point

    @param hFile             (IN) - DF3File handle

    @return DivXError       (OUT) - DivXError
*/
DivXError DF3File_GetNextSyncPoint(DF3File_h hFile, DMFVariant *Value);

/*! 
    @note Parse the cues list or the hierarchical index of a file

    @param hFile             (IN) - DF3File handle

    @return DivXError       (OUT) - DivXError
*/
DivXError DF3File_ParseIndex(DF3File_h hFile);

#endif
/* _DF3FILE_H_ */
