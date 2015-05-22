/*!
    @file
   @verbatim
   $Id: decompress.cpp 6706 2008-06-09 17:05:11Z jbraness $

   Copyright (c) 2002-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "drm_mplayer.h"

uint8_t           *drm_info;
uint8_t           *drmContext;
DMFContainerHandle    hInputContainer = NULL;
DivXString           *in_name;
DivXError             divxError = 0;
t_options options; // carlos add merge from options_decompress.c
DRM_chunk *audio_dd_chunk=NULL;
DRM_chunk *video_dd_chunk=NULL;

int decrypt_init_drm( uint8_t *drm_chunk )
{
    drmErrorCodes_t result = DRM_SUCCESS;
    uint8_t         rentalMessageFlag = 0;
    uint8_t         useLimit = 0;
    uint8_t         useCount = 0;
    uint8_t         cgmsaSignal = 0;
    uint8_t         acptbSignal = 0;
    uint8_t         digitalProtectionSignal = 0;
	uint8_t         ict = 0;
    uint32_t        drmContextLength = 0;
    uint32_t        error = 0;
	playbackContext_t *context = NULL;
	drmTargetHeader_t fileInfo;


    result = drmInitSystem( NULL,
                            &drmContextLength );
    drmContext = (uint8_t *)malloc( drmContextLength  );

    result = drmInitSystem( drmContext,
                            &drmContextLength );

    if( DRM_SUCCESS != result )
    {
        printf( "This player is not authorized to play this video. in [%s][%d] errno[%d]\n", __func__, __LINE__, drmGetLastError( drmContext ) );
        return DIVX_NEED_IPC_CALLBACK_ERROR;
    }

    result = drmInitPlayback( drmContext,
                              drm_chunk );
    if( DRM_SUCCESS != result )
    {
        printf( "This player is not authorized to play this video. in [%s][%d] errno[%d]\n", __func__, __LINE__, drmGetLastError( drmContext ) );
        return DIVX_NEED_IPC_CALLBACK_ERROR;
    }

	context = (playbackContext_t *)drmContext;
	fileInfo = context->targetHeader;
#if 1 // carlos add for debug 
	printf("==== filInf.drmMode [%x]====\n", fileInfo.drmMode);
	printf("==== filInf.protectedAudioOffset[%x] fileInfo.protectedAudioCryptoSize [%x]====\n", fileInfo.protectedAudioOffset, fileInfo.protectedAudioCryptoSize);
	printf("#### fileInfo.drmSubMode[%x] useLimitId[%x] optionFlags[%x]####\n", fileInfo.drmSubMode,  fileInfo.rentalRecord.useLimitId, fileInfo.optionFlags);
#endif 	
	if (fileInfo.optionFlags & PROTECTED_AUDIO_MASK) 
	{
		audio_dd_chunk = malloc(sizeof(DRM_chunk));
		if (audio_dd_chunk)
		{
			memset(audio_dd_chunk, 0, sizeof(DRM_chunk));
			audio_dd_chunk->KeyIdx = 0;
			audio_dd_chunk->EncryptionOffset = fileInfo.protectedAudioOffset;
			audio_dd_chunk->EncryptionLength = fileInfo.protectedAudioCryptoSize;
			printf("==== keyIdx[%x] offset [%x] length [%x]====\n", audio_dd_chunk->KeyIdx, audio_dd_chunk->EncryptionOffset, audio_dd_chunk->EncryptionLength);
		}
		else
			printf("####malloc audio_dd_chunk failed ===\n");
	}

	/* Check Signed */
	if (fileInfo.drmMode == DRM_TYPE_ACTIVATION_PURCHASE || fileInfo.drmMode == DRM_TYPE_ACTIVATION_RENTAL)
	{
		/* File is not signed, reject this file */
		if (!(fileInfo.optionFlags & SIGNED_ACTIVATION_MASK))
		{
			printf("File is not signed\n");
            return DIVX_NEED_IPC_CALLBACK_ERROR;
		}
	}

    result = drmQueryRentalStatus( drmContext,
                                   &rentalMessageFlag,
                                   &useLimit,
                                   &useCount );
    if( DRM_SUCCESS != result )
    {
        if( DRM_RENTAL_EXPIRED == result )
        {
            //printf( "Rental Expired useLimit[%d] useCount[%d] rentalMessageFlag[%d]\n", drmGetLastError( drmContext ) , useLimit, useCount, rentalMessageFlag);
            printf( "Rental Expired useLimit[%d] useCount[%d] rentalMessageFlag[%d]\n", useLimit, useCount, rentalMessageFlag);
            return DIVX_NEED_IPC_CALLBACK_ERROR;
        }
		printf( "Other failed Expired\n", drmGetLastError( drmContext ) );
    }

	if( DIVX_TRUE == rentalMessageFlag )
		printf( "\nThis rental has %d views left.\nYou will use one of your %d views now. total Limit is [%d] ready count [%d]\n", useLimit - useCount, useLimit - useCount , useLimit, useCount);

	drmSetRandomSample( drmContext );
	usleep(10000);
	drmSetRandomSample( drmContext );
	usleep(10000);
	drmSetRandomSample( drmContext );
	usleep(10000);
	drmSetRandomSample( drmContext );
	usleep(10000);

    result = drmQueryCgmsa( drmContext,
                            &cgmsaSignal );
    if( DRM_SUCCESS != result )
    {
        printf( "This player is not authorized to play this video in [%s][%d].\n", __func__, __LINE__);
        return DIVX_NEED_IPC_CALLBACK_ERROR;
    }
    printf( "\nCGMS-A: ");
    switch(cgmsaSignal)
    {
    case 0:
        printf( "Copy Freely\n");
        break;
    case 1:
        printf( "Copy No More\n");
        break;
    case 2:
        printf( "Copy Once\n");
        break;
    case 3:
        printf( "Copy Never\n");
        break;
    default:
        printf( "Unknown\n");
        break;
    };


    result = drmQueryAcptb( drmContext,
                            &acptbSignal );
    if( DRM_SUCCESS != result )
    {
        printf( "This player is not authorized to play this video in [%s][%d].\n", __func__, __LINE__);
        return DIVX_NEED_IPC_CALLBACK_ERROR;
    }
    printf( "ACPTB(Macrovison): ");
    switch(acptbSignal)
    {
    case 0:
        printf( "off\n");
        break;
    case 1:
        printf( "Automatic Gain Control\n");
        break;
    case 2:
        printf( "2 line color burst\n");
        break;
    case 3:
        printf( "4 line color burst\n");
        break;
    default:
        printf( "Unknown\n");
        break;
    };

    result = drmQueryDigitalProtection( drmContext,
                                        &digitalProtectionSignal );
    if( DRM_SUCCESS != result )
    {
        printf( "This player is not authorized to play this video. [%s][%d]\n", __func__, __LINE__);
        return DIVX_NEED_IPC_CALLBACK_ERROR;
    }
    printf( "Digital Protection(HDCP): ");
    digitalProtectionSignal ? printf("YES\n\n") : printf("NO\n\n");

	/* SDK 6.1 only */
	result = drmQueryIct( drmContext, &ict );
	if( DRM_SUCCESS != result )
	{
		printf( "This player is not authorized to play this video in [%s][%d].\n", __func__, __LINE__);
		return DIVX_NEED_IPC_CALLBACK_ERROR;
	}
	printf( "Image Constraint Token (ICT): ict[%d]", ict);/* 1 analog HD allowed, 0 analog image must be constrained */
	ict ? printf("Analog High Definition Image allowed\n\n") : printf("Analog Constrained Image\n\n");

	extern int is_ICT;
	if(!ict)
		is_ICT = 1;


    result = drmCommitPlayback( drmContext );
    error = drmGetLastError( drmContext );
    switch(error)
    {
    case DRM_ERROR_GUARD_MISMATCH:
        printf("\nINFO: Owner Guard in file does not match Owner Guard in DrmMemory.\nThe registration code may be bad or device activation may have already occured.\n\n");
        break;
    case DRM_ERROR_MODEL_MISMATCH:
        printf("\nINFO: Model ID in file does not match Model ID in Firmware.\n\n");
        break;
    case DRM_ERROR_NO_SIGNATURE:
        printf("\nINFO: activation file is not signed\n\n");
        break;
	default:
		printf("\nUnkonwn error: [%d]result [%d]\n", error, result);
    }
#if 0	
/* testing Purchase */	
	if (result == 1 && error ==1)
	return 1;
#endif	
    if((DRM_SUCCESS != result  || error != DRM_SUCCESS) && (!(fileInfo.drmMode == 0xC3C3 || fileInfo.drmMode == 0x3C3C) || (error != DRM_ERROR_GUARD_MISMATCH)))
    {
        printf( "This player is not authorized to play this video. [%s][%d] useLimit[%d] fileInfo.drmMode[%x]\n" , __func__, __LINE__, useLimit, fileInfo.drmMode);
		return DIVX_NEED_IPC_CALLBACK_ERROR;
    }
    return 1;
}

DivXError getDrm1Chunk( DMFContainerHandle hInputContainer,
                        uint32_t          *sizeData,
                        uint8_t          **pChunk )
{
    DivXError  divxError;
    DMFVariant variant;

    DIVX_CHECKPOINTER( hInputContainer, DIVX_ERR_INVALID_ARG );
    DIVX_CHECKPOINTER( sizeData, DIVX_ERR_INVALID_ARG );
    DIVX_CHECKPOINTER( pChunk, DIVX_ERR_INVALID_ARG );

    divxError = L2car_GetInfo( hInputContainer, DMF_QID_GET_TITLE_DRM1_DATA_SIZE, &variant, 0 );

    if( divxError == DIVX_ERR_SUCCESS )
    {
        uint32_t drmDataSize = variant.v.u32;

        /* check for success because all containers do not support drm */
        if( drmDataSize > 0 )
        {
            *sizeData = drmDataSize;
            *pChunk   = (uint8_t *)DivXMemAlloc( drmDataSize, NULL ); /* TODO Linux Refactor (requires hMem) */

            variant.v.u8p  = *pChunk;
            variant.Length = drmDataSize;

            /* grab the actual drm data chunk */
            divxError = L2car_GetInfo( hInputContainer, DMF_QID_GET_TITLE_DRM1, &variant, 0 );
            if( divxError != DIVX_ERR_SUCCESS )
            {
                return divxError;
            }

            return DIVX_ERR_SUCCESS;
        }
    }

    /* no drm data */
    return DIVX_ERR_FAILURE;
}


int DRMInit(const char *in_filename)	//DRM_debug
{
    int                   result = 0;
	int					  ret = 0;
    DMFVariant            inValueVid;
    DMFVideoStreamInfo1_t vidInfo;
    DMFVariant            variant;
    uint32_t  size = 0;
    drm_info    = NULL;
	extern unsigned int no_osd;


    printf( "Decompress (build %s) ", SDK_VERSION);

	memcpy(options.in_name, in_filename, strlen(in_filename));
    // open the input file
    printf( "Opening input file - %s\n", options.in_name );

    in_name = DivXStringCharStringToDivXString( options.in_name );

    /* intialize the module */
    divxError = L2car_Init( DMF_DEFAULT_MODULES );
    if( divxError != DIVX_ERR_SUCCESS )
    {
        printf( "Failed to intialize L2car_Init.Error:%d", divxError );
        return DIVX_LIBRARY_INIT_FAILED;
    }

    /* open up the input container */
    divxError = L2car_OpenContainer( &hInputContainer, in_name, NULL );
    if( DIVX_IS_FATAL_ERROR(divxError) )
    {
        printf( "Failed in L2car_OpenContainer.Error:%d", divxError );
		ret = DIVX_LIBRARY_INIT_FAILED;
		goto cleanup;
    }

    /* set to the first title */
    divxError = L2car_SetTitle( hInputContainer, 0 );
    if( divxError != DIVX_ERR_SUCCESS )
    {
        printf( "Failed in L2car_SetTitle.Error:%d", divxError );
		ret = DIVX_LIBRARY_INIT_FAILED;
		goto cleanup;
    }

    /*
       Set the active streams
     */
    divxError = getDrm1Chunk( hInputContainer, &size, &drm_info );
    if( divxError != DIVX_ERR_SUCCESS )
    {
        printf( "Failure to retrieve strd chunk!!!, not drm file\n" );
        return NOT_INCLUDE_DIVX_DRM_DATA;
    }

    inValueVid.Length = sizeof( DMFVideoStreamInfo1_t );
    inValueVid.v.u8p  = (uint8_t *)&vidInfo;

    divxError = L2car_GetInfo( hInputContainer, DMF_QID_VID_STREAM_FORMAT, &inValueVid, 0 );
    if( divxError != DIVX_ERR_SUCCESS )
    {
        printf( "Failed in L2car_GetInfo.Error:%d", divxError );
		ret = DIVX_LIBRARY_INIT_FAILED;
		goto cleanup;
    }

	if (no_osd)
		ret = DIVX_NEED_IPC_CALLBACK_ERROR;
	else
		ret = decrypt_init_drm(drm_info) ;

cleanup:

    /* close the container */
    L2car_CloseContainer( hInputContainer );

    /* cleanup */
    if( in_name != NULL )
    {
        DivXStringDeallocate( in_name );
		in_name = NULL;
    }
	return ret;

}

int DRMDecode(int frame_size, unsigned char *frame_data, DRM_chunk *drm_chunk, char type)
{
	playbackContext_t *context = (playbackContext_t *) drmContext;
    int32_t decryptedFrameSize = 0;

	if (drm_chunk)
	{
		if (drm_chunk->EncryptionLength && ((drm_chunk->EncryptionLength + drm_chunk->EncryptionOffset ) <= frame_size))
		{
			#if 0
			if (type == DIVX_DECODE_VIDEO)
				printf("===carlos say [%s][%s][%d]: key[%x], offset[%x], length[%x], type [%x]==\n", __FILE__, __func__, __LINE__, drm_chunk->KeyIdx, drm_chunk->EncryptionOffset, drm_chunk->EncryptionLength, type);
			#endif
			decryptedFrameSize = drm_chunk->EncryptionLength;
			DivXAesECBDecrypt( context->keys[drm_chunk->KeyIdx],
					frame_data + drm_chunk->EncryptionOffset,
					drm_chunk->EncryptionLength,
					frame_data + drm_chunk->EncryptionOffset,
					&decryptedFrameSize );
			if ( decryptedFrameSize != drm_chunk->EncryptionLength)
			{
				printf("### carlos say: decryption size is not match in[%d]-out[%d][%d]\n", drm_chunk->EncryptionLength, decryptedFrameSize, type);
			}
			else
			{
				if (type == DIVX_DECODE_VIDEO)
					drm_chunk->EncryptionLength = 0;
			}
		}
		else if ((drm_chunk->EncryptionLength + drm_chunk->EncryptionOffset) > frame_size)
		{
			printf("[%s] size mismatch drm_chunk->EncryptionLength[%d] offset[%d] frame_size [%d] in [%s][%d]\n", type == DIVX_DECODE_VIDEO ? "Video" : "Audio", drm_chunk->EncryptionLength, drm_chunk->EncryptionOffset, frame_size, __func__, __LINE__);
		}
		else
			printf("carlos say :  EncryptionLength is zero\n");
	}
	else
	{
		printf("carlos say :  drm_chunk is NULL\n");
	}
	return 0;
}

void DRMUninit(void)
{
	int result = 0;
    /*
       Cleanup
     */
	//printf("@@@@@@@@@@@@@@@@In [%s][%d] #######################\n", __func__, __LINE__);
	result = drmFinalizePlayback(drmContext);
	if (result != DRM_SUCCESS)
		printf("Failed to drmFinalizePlayback:%d\n", drmGetLastError( drmContext ));
	else
		printf("drmFinalizePlayback is successful\n");

	if (audio_dd_chunk)
	{
		free(audio_dd_chunk);
		audio_dd_chunk = NULL;
	}

    /* cleanup */
    if( in_name != NULL )
    {
        DivXStringDeallocate( in_name );
		in_name = NULL;
    }	
	memset (&options, 0, sizeof(t_options));

}

