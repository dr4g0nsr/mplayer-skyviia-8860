/*!
    @file
@verbatim
$Id: L4ReadStream.h 59666 2009-04-15 02:41:53Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _L4READSTREAM_H_
#define _L4READSTREAM_H_

#include "DivXError.h"
#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXTime.h"

#include "DMFBlockType.h"
#include "DMFVideoStreamInfo1.h"
#include "DMFAudioStreamInfo1.h"
#include "DMFSubtitleStreamInfo1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _L4ReadStreamInst_t* L4ReadStreamHandle;

/*!
    Create an instance of a stream.

    @param pHandle (OUT)  - A pointer to receive instance.
    @param hL4Title (IN)  - Title handle
    @param hMem    (IN)   - Memory handle the stream will use.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Stream_New( L4ReadStreamHandle *pHandle, void* hL4Title, DivXMem hMem );

/*!
    Delete an instance of a stream.

    @param handle (IN) - A handle to instance.

    @return The normal return code is DIVX_ERR_SUCCESS
*/
DivXError L4Stream_Delete( L4ReadStreamHandle handle );

/*!
    Sets the current stream number

    @param handle  (IN) - A handle to instance.
    @param nStream (IN) - Stream index to set to

    @return void
*/
void L4Stream_SetStreamNum( L4ReadStreamHandle handle, int32_t nStream);

/*!
    Returns the current stream number

    @param handle (IN) - A handle to instance.

    @return int32_t  - The stream number
*/
int32_t L4Stream_GetStreamNum( L4ReadStreamHandle handle );

/*!
    Retreives the type of stream.

    @param handle (IN) - A handle to instance.

    @return The stream type.
*/
DMFBlockType_t L4Stream_GetType( L4ReadStreamHandle handle );

/*!
    Sets the stream type.

    @param handle (IN) - A handle to instance.
    @param type   (IN) - Type of the stream.
*/
void L4Stream_SetType( L4ReadStreamHandle handle, DMFBlockType_t type);

/*!
    Gets the stream priority.

    @param handle (IN) - A handle to instance.

    @return The stream priority 0 highest.

*/
int32_t L4Stream_GetPriority( L4ReadStreamHandle handle );

/*!
    Sets the stream priority.

    @param handle   (IN) - A handle to instance.
    @param priority (IN) - The stream priority 0 highest
*/
void L4Stream_SetPriority( L4ReadStreamHandle handle, int32_t priority );

/*!
    Gets the current time.

    @param handle (IN) - A handle to the instance.
    
    @return The current time.
*/
DivXTime L4Stream_GetCurTime( L4ReadStreamHandle handle );

/*!
    Sets the current time.

    @param handle (IN) - A handle to the instance.
    @param time   (IN) - The current time.
*/
void L4Stream_SetCurTime( L4ReadStreamHandle handle, DivXTime time );

/*!
    Gets the current display time.

    @param handle (IN) - A handle to the instance.
    
    @return The current display time.
*/
DivXTime L4Stream_GetCurDisplayTime( L4ReadStreamHandle handle );

/*!
    Sets the current time.

    @param handle (IN) - A handle to the instance.
    @param time   (IN) - The current display time.
*/
void L4Stream_SetCurDisplayTime( L4ReadStreamHandle handle, DivXTime time );

/*!
    Gets if the stream is preloaded already.

    @param handle (IN) - A handle to the instance.

    @return DIVX_TRUE stream is preloaded else DIVX_FALSE.
*/
DivXBool L4Stream_GetPreloaded( L4ReadStreamHandle handle );

/*!
    Sets if the stream is preloaded already.

    @param handle     (IN) - A handle to the instance.
    @param bPreloaded (IN) - DIVX_TRUE stream is preloaded else DIVX_FALSE.
*/
void L4Stream_SetPreloaded( L4ReadStreamHandle handle, DivXBool bPreloaded );

/*!
    Get the preload time.

    @param handle     (IN) - A handle to the instance.
    @return The preload time.
*/
DivXTime L4Stream_GetPreloadTime( L4ReadStreamHandle handle );

/*!
    Set the preload time.

    @param handle (IN) - A handle to the instance.
    @param time   (IN) - The preload time.
*/
void L4Stream_SetPreloadTime( L4ReadStreamHandle handle, DivXTime time );

/*!
    Get if the stream is continuous. (ie sub stream is not contininuous)

    @param handle (IN) - A handle to the instance.
    
    @return DIVX_TRUE stream continuous, else DIVX_FALSE.
*/
DivXBool L4Stream_GetContinuous( L4ReadStreamHandle handle );

/*!
    Set if the stream is continuous. (ie sub stream is not contininuous)

    @param handle      (IN) - A handle to the instance.
    @param bContinuous (IN) - DIVX_TRUE stream continuous, else DIVX_FALSE.

    @return DivXError
*/
void L4Stream_SetContinuous(L4ReadStreamHandle handle, DivXBool bContinuous );

/*!
    Returns the video format

    @param handle         (IN) - A handle to the instance.
    @param videoStruct   (OUT) - The video structure to be returned
    @param streamIndex    (IN) - The index of the video stream to retrieve the video struct for

    @return DivXError
*/
DivXError L4Stream_GetVideoFormat( L4ReadStreamHandle handle, DMFVideoStreamInfo1_t *videoStruct, uint32_t streamIndex );

/*!
    Returns the audio format

    @param handle         (IN) - A handle to the instance.
    @param audioStruct   (OUT) - The audio structure to be returned
    @param streamIndex    (IN) - The index of the audio stream to retrieve the audio struct for

    @return DivXError
*/
DivXError L4Stream_GetAudioFormat( L4ReadStreamHandle handle, DMFAudioStreamInfo1_t *audioStruct, uint32_t streamIndex );

/*!
    Returns the subtitle format

    @param handle         (IN) - A handle to the instance.
    @param subtitleStruct   (OUT) - The subtitle structure to be returned
    @param streamIndex    (IN) - The index of the subtitle stream to retrieve the subtitle struct for

    @return DivXError
*/
DivXError L4Stream_GetSubtitleFormat( L4ReadStreamHandle handle, DMFSubtitleStreamInfo1_t *subtitleStruct, uint32_t streamIndex );

/*!
    Form a string to display for the subtitle

    @param pInst         (IN)     - A stream handle.
    @param nChapter      (IN)     - The index of the chapter to string for
    @param buffer        (IN/OUT) - A buffer to hold string
    @param bufferSize    (IN)     - Size of the buffer

    @return DivXError
*/
DivXError L4Stream_GetChapterDisplayString( L4ReadStreamHandle pInst, uint32_t nChapter, DivXString *buffer, int32_t bufferSize);

/*!
    Form a string to display about the stream

    @param pInst         (IN)     - A stream handle.
    @param streamIndex   (IN)     - The index of the stream to string for
    @param buffer        (IN/OUT) - A buffer to hold string
    @param bufferSize    (IN)     - Size of the buffer

    @return DivXError
*/
DivXError L4Stream_GetVideoDisplayString( L4ReadStreamHandle pInst, uint32_t streamIndex, DivXString *buffer, int32_t bufferSize);

/*!
    Form a string to display about the stream

    @param pInst         (IN)     - A stream handle.
    @param streamIndex   (IN)     - The index of the stream to string for
    @param buffer        (IN/OUT) - A buffer to hold string
    @param bufferSize    (IN)     - Size of the buffer

    @return DivXError
*/
DivXError L4Stream_GetAudioDisplayString( L4ReadStreamHandle pInst, uint32_t streamIndex, DivXString *buffer, int32_t bufferSize);

/*!
    Form a string to display about the stream

    @param pInst         (IN)     - A stream handle.
    @param streamIndex   (IN)     - The index of the stream to string for
    @param buffer        (IN/OUT) - A buffer to hold string
    @param bufferSize    (IN)     - Size of the buffer

    @return DivXError
*/
DivXError L4Stream_GetSubtitleDisplayString( L4ReadStreamHandle pInst, uint32_t streamIndex, DivXString *buffer, int32_t bufferSize);

/*!
    Get total duration

    @param handle          (IN) - A handle to the instance.
    @param pTotalDuration (OUT) - Returns the total duration for this track

    @return DivXError
*/
DivXError L4Stream_GetTotalDuration( L4ReadStreamHandle handle, DivXTime* pTotalDuration);


#ifdef __cplusplus
}
#endif

#endif
