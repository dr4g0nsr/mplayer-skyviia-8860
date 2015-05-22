/*!
	Skyviia carlostai
 **/

#ifndef _DRM_MPLAYER_H_
#define _DRM_MPLAYER_H_

#include "DivXBool.h"
#include "DrmApi.h"
#include "DrmTypes.h"

#include "DMFQueryIDs.h"
#include "DivXFile.h"
#include "DivXString.h"
#include "./L2Common/MappingLayerCommon.h"
#include "./L2Read/MappingLayerRead.h"
#include "DMFVideoStreamInfo1.h"
//#include "crypto/DivXAesDecrypt.h"
#include "DivXAesDecrypt.h"
#include "libmpdemux/aviheader.h"

#define NOT_INCLUDE_DIVX_DRM_DATA	-3
#define DIVX_LIBRARY_INIT_FAILED	-2
#define DIVX_DRM_FILE_INITIAL_FAILED	-1
#define DIVX_NEED_IPC_CALLBACK_ERROR	-4
#define DIVX_DECODE_VIDEO	0x01
#define DIVX_DECODE_AUDIO 	0x02

int DRMDecode(int frame_size, unsigned char *frame_data, DRM_chunk *drm_chunk, char type);

#define FILENAME_LEN 256
typedef struct _t_options
{
    char in_name[FILENAME_LEN];
    char out_name[FILENAME_LEN];
    int  frames;
    int  post_process;
}
t_options;

#ifndef mmioFOURCC
typedef unsigned char BYTE;
typedef unsigned int  DWORD;
#define mmioFOURCC( ch0, ch1, ch2, ch3 ) ( (DWORD)(BYTE)( ch0 ) | ( (DWORD)(BYTE)( ch1 ) << 8 ) | ( (DWORD)(BYTE)( ch2 ) << 16 ) | ( (DWORD)(BYTE)( ch3 ) << 24 ) )
#endif

//#define BUILD_NUMBER     21
#define SDK_VERSION		"Divx SDK 6.1"

#define RANDOM_BYTE_BUCKET_SIZE			8

// Playback Context States.
typedef enum drmApiState
{
    DRM_STATE_BOOT = 1,
    DRM_STATE_INITIALIZED,
    DRM_STATE_RENTAL_QUERIED,
    DRM_STATE_CGMSA_QUERIED,
    DRM_STATE_ACPTB_QUERIED,
    DRM_STATE_DIGITAL_PROTECTION_QUERIED,
	DRM_STATE_ICT_QUERIED,
    DRM_STATE_COMMITTED,
    DRM_STATE_FINALIZED
} drmApiState_t;

typedef struct playbackContext
{
	uint8_t baseKeyId[SIZEOF_BASE_KEY_ID_SIZE_BYTES];
	uint8_t* strdPtr;
    drmTargetHeader_t targetHeader;
    drmTransactionInfoHeader_t transactionHeader;
    drmFrameKeys_t frameKeys;
	aesKeyHandle keys[VIDEO_KEY_COUNT_MAX];
    uint32_t randomSamples;
    uint8_t randomByteBucket[RANDOM_BYTE_BUCKET_SIZE];
    uint8_t localTicker;
	drmApiState_t state;
	drmErrorCodes_t drmLastError;
    drmHeaderSignature_t  signature;
} playbackContext_t;

int decrypt_init_drm( uint8_t *drm_chunk );
#endif // end of ifndef _DRM_MPLAYER_H_
