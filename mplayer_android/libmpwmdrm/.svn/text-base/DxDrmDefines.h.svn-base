#ifndef DX_DRM_DEFINES_H
#define DX_DRM_DEFINES_H

#include "DxTypes.h"

#define DX_FUNC
#define DX_SECURE_FUNC(securityLevel)

#if defined(DX_DRM_NO_EXPORTS) || (!defined(WIN32) && !defined(WINCE))
#define DRM_C_API 
#else
#ifdef DX_DRM_EXPORTS
#define DRM_C_API __declspec(dllexport) 
#else
#define DRM_C_API __declspec(dllimport) 
#endif
#endif


#define DX_ACTIVE_CONTENT   0xFFFFFFFF

#define DX_WMDRM_SECURE_CLOCK_ID		"WMDRMSecureClock"
#define DX_PLAYREADY_SECURE_CLOCK_ID	"PlayReadySecureClock"
#define DX_OMA_V1_SECURE_CLOCK_ID		"OmaV1SecureClock"

typedef void* DxOSFileHandle;

//! \file DxDrmDefines.h
//! \brief Defines most of the DRM related data types.


//! List of error codes returned by the functions in the DRM modules.
	typedef enum EDxDrmStatus {
    /*0*/DX_DRM_SUCCESS,							//!< Operation was successful.
    /*1*/DX_ERROR_NO_RIGHTS,
    /*2*/DX_ERROR_SECURE_CLOCK_IS_NOT_SET,
    /*3*/DX_ERROR_DOMAIN_CONTEXT_IS_MISSING,
	/*4*/DX_ERROR_RIGHTS_PENDING,
	/*5*/DX_ERROR_METERING_CONSENT_REQUIRED,
	/*6*/DX_ERROR_METERING_CONSENT_OPTIONAL,

    /*7*/DX_ERROR_NOT_INITIALIZED,
    /*8*/DX_ERROR_FATAL_ERROR,
    /*9*/DX_ERROR_GENERAL_FAILURE,
    /*10*/DX_ERROR_BAD_ARGUMENTS,
    /*11*/DX_ERROR_MEMORY_ALLOCATION_FAILURE,
    /*12*/DX_ERROR_NOT_SUPPORTED,
	/*13*/DX_ERROR_ITEM_NOT_FOUND,

    /*14*/DX_ERROR_FILE_ACCESS_ERROR,
    /*15*/DX_ERROR_BUFFER_IS_TOO_SMALL,
	/*16*/DX_ERROR_CONTENT_NOT_RECOGNIZED,
	/*17*/DX_ERROR_INSUFFICIENT_FREE_SPACE,

    /*18*/DX_ERROR_SECURE_STORAGE_IS_CORRUPTED,
    /*19*/DX_ERROR_IPC_FAILURE,

    /*20*/DX_ERROR_MIME_TYPE_NOT_RECOGNIZED,
	/*21*/DX_ERROR_CONTENT_TYPE_NOT_SUPPORTED,

	/*22*/DX_ERROR_INVALID_FORMAT,
	/*23*/DX_ERROR_INVALID_RO_XML,

    /*24*/DX_ERROR_DEST_FILE_NAME_REQUIRED,
    /*25*/DX_ERROR_MISSING_BOUNDARY,

    /*26*/DX_ERROR_FILE_IS_BEING_DOWNLOADED,
    /*27*/DX_ERROR_FILE_SIZE_INCLUDES_PADDING,
    /*28*/DX_ERROR_INTENT_WAS_NOT_SET,

    /*29*/DX_ERROR_TAG_ALREADY_EXIST,
    /*30*/DX_ERROR_ASSET_ALREADY_EXIST,

	/*31*/DX_USER_MESSAGE_IS_AVAILABLE,
	/*32*/DX_HTTP_REQUEST_IS_AVAILABLE,
	/*33*/DX_BROWSER_WAS_LAUNCHED,
    /*34*/DX_ERROR_NOT_AVAILABLE,

    /*35*/DX_ERROR_TIMEOUT_EXPIRED,
    /*36*/DX_ERROR_CANNOT_ACQUIRE_RIGHTS,
    /*37*/DX_ERROR_CANNOT_ADJUST_CLOCK,
    /*38*/DX_ERROR_OPERATION_CANCELLED_BY_USER,
	/*39*/DX_ERROR_OPERATION_FAILED,
	
	/*40*/DX_ERROR_BACKUP_NOT_FOUND,

	DX_ERROR_ROAP_OPERATION_FAILED,

    DX_ERROR_INVALID_STATE,
    DX_ERROR_PROVISIONING_NOT_DONE,
	DX_ERROR_PROVISIONING_ALREADY_DONE,
    DX_ERROR_VERIFICATION_FAILURE,

	DX_ERROR_NO_INTERNET_CONNECTION,
	DX_ERROR_CANNOT_CONNECT_TO_SERVER,
	DX_ERROR_COMMUNICATION_FAILURE,

	DX_ERROR_CONTEXT_EXPIRED,
	DX_ERROR_CONTEXT_MISSING,
    DX_ERROR_FILE_IS_NOT_FORWARDABLE,

	//Personalization specific error messages
	DX_ERROR_PERS_INVALID_MESSAGE,
	DX_GENERAL_PERS_SERVER_ERROR,
	DX_ERROR_NEW_BLOB_REQUIRED,
	DX_ERROR_VERSION_TOO_OLD,
	DX_ERROR_VERSION_NOT_SUPPORTED,
	DX_ERROR_INVALID_KEY,
	DX_ERROR_INVALID_BLOB,
	DX_ERROR_INVALID_PLATFORM,
	DX_ERROR_INVALID_CREDENTIALS,
	DX_ERROR_INVALID_TIMESTAMP,

    DX_ERROR_LAST_ERROR
} EDxDrmStatus;

typedef enum EDxAutoMode {
    DX_AUTO_INVALID,
    DX_AUTO_DISABLED,
    DX_AUTO_NO_UI_AND_COMMUNICATION,
    DX_AUTO_NO_UI,
    DX_AUTO_ENABLED,
} EDxAutoMode;

typedef enum EDxDrmScheme {
    DX_SCHEME_UNKNOWN,
    DX_SCHEME_OMA_V1,
    DX_SCHEME_OMA_V2,
	DX_SCHEME_WMDRM,
	DX_SCHEME_PLAYREADY,
} EDxDrmScheme;

typedef enum EDxRightsStatus {
	DX_RIGHTS_STATUS_NOT_PROTECTED,
    DX_RIGHTS_STATUS_NOT_VALID,
    DX_RIGHTS_STATUS_VALID,
	DX_RIGHTS_STATUS_FORWARD_LOCK,
	DX_RIGHTS_STATUS_UNKNOWN
} EDxRightsStatus;

typedef enum EDxTextAttribute {
	DX_ATTR_CONTENT_ID,
	DX_ATTR_CONTENT_MIME_TYPE,
	DX_ATTR_FILE_MIME_TYPE,
    DX_ATTR_TITLE,
    DX_ATTR_DESCRIPTION,
    DX_ATTR_AUTHOR,
    DX_ATTR_RI_URL,
    DX_ATTR_ICON_URI,
    DX_ATTR_VENDOR,
    DX_ATTR_ENCRYPTION_METHOD,

	//OMA V2
	//CommonHeaders
	DX_ATTR_COMMON_HDR_VERSION,
	DX_ATTR_PADDING_SCHEME,

	//PlayReady
	DX_ATTR_PRDY_DOMAIN_LIST,

	//TextualHeaders
	DX_ATTR_INFO_URL,
	DX_ATTR_COVER_URI,
	DX_ATTR_LYRICS_URI,
	DX_ATTR_SILENT_METHOD,
	DX_ATTR_SILENT_URL,
	DX_ATTR_PREVIEW_METHOD,
	DX_ATTR_PREVIEW_URL,
	DX_ATTR_CONTENT_URL,
	DX_ATTR_CONTENT_VERSION,
	DX_ATTR_CONTENT_LOCATION,
	DX_ATTR_PROFILE_NAME,
	DX_ATTR_GROUP_ID,
	DX_ATTR_CLASSIFICATION_ENTITY,
	DX_ATTR_CLASSIFICATION_TABLE,
	DX_ATTR_CLASSIFICATION_INFO,
	DX_ATTR_RECORDING_YEAR,
	DX_ATTR_COPYRIGHT_NOTICE,
	DX_ATTR_PERFORMER,
	DX_ATTR_GENRE,
	DX_ATTR_ALBUM_TITLE,
	DX_ATTR_ALBUM_TRACK_NUM,
	DX_ATTR_RATING_ENTITY,
	DX_ATTR_RATING_CRITERIA,
	DX_ATTR_RATING_INFO,
	DX_ATTR_KEYWORDS,
	DX_ATTR_LOCATION_INFO,
	DX_ATTR_LOCATION_TYPE,
	DX_ATTR_LOCATION_LONGITUDE,
	DX_ATTR_LOCATION_LATITUDE,
	DX_ATTR_LOCATION_ALTITUDE,
	DX_ATTR_TRANSACTION_ID,
	DX_ATTR_CUSTOM_DATA,

	DX_NUM_OF_ATTRIBUTES,
	DX_INVALID_ATTR = DX_NUM_OF_ATTRIBUTES
} EDxTextAttribute;

//! Represents the intention of using the DRM file.
typedef enum EDxIntent {
    DX_INTENT_PEEK,         //!< Use if content is accessed only for preview or thumbnail generation
    DX_INTENT_PLAY,         //!< Use for audio or video files
    DX_INTENT_DISPLAY,      //!< Use for image files.
    DX_INTENT_EXECUTE,      //!< Use for applications or Java elements
    DX_INTENT_PRINT,        //!< Use when going to print image file.
    DX_INTENT_RINGTONE,     //!< Use whenever audio/video file is used as ring-tone/video-tone
    DX_INTENT_AUTO_PLAY,    //!< Use whenever the audio/video file is used without explicit user consent
                            //!< (i.e. screen saver, alert, etc...)
    DX_INTENT_AUTO_DISPLAY, //!< Use whenever the image file is used without explicit user consent
                            //!< (i.e. contact attached picture, alert, etc...)
    DX_NUM_OF_INTENTS,
    DX_INVALID_INTENT = DX_NUM_OF_INTENTS
} EDxIntent;

//! A consumption event indicates the DRM agent when content consumption starts, stops, pauses and resumed.
typedef enum EDxConsumptionEvent {
    DX_EVENT_START,         //!< Use when consumption starts
    DX_EVENT_STOP,          //!< Use when consumption ends
    DX_EVENT_PAUSE,         //!< Use when consumption is temporarily paused.
    DX_EVENT_RESUME,        //!< Use when consumption that was previously paused is resumed
    DX_EVENT_TICK           //!< Use during consumption
} EDxConsumptionEvent;

//! A list of flags that represents attributes of DRM content.
typedef enum EDxContentFlag {
    //! True if the file is DRM protected (i.e. encrypted)
    DX_FLAG_IS_PROTECTED         = 0x00000001,
    //! True if the file has any rights (even if they are expired or pending)
    DX_FLAG_HAS_ANY_RIGHTS       = 0x00000002,
    //! True if the file has valid rights for any permission
    DX_FLAG_HAS_VALID_RIGHTS     = 0x00000004,
    //! True if the file has any future rights (i.e. rights that will become valid in the future)
    DX_FLAG_HAS_FUTURE_RIGHTS	 = 0x00000008,
    //! True if the file has any pending rights (i.e. rights that were acquired but not yet arrived)
    DX_FLAG_HAS_PENDING_RIGHTS   = 0x00000010,
    //! True if the file has rights that already expired. 
    //! \note that rights that expired may be deleted automatically. In this case this flag will be false.
    DX_FLAG_HAS_EXPIRED_RIGHTS   = 0x00000020,
    //! True if the file has at least one RO that contains stateful constraint 
    //! (i.e. count, timed count, inactive interval, accumulated
    DX_FLAG_HAS_STATEFUL_RIGHTS  = 0x00000040,
	//! True if the file has at least one RO without any constraints. 
	DX_FLAG_HAS_UNLIMITED_RIGHTS = 0x00000080,
    //! True if the file can be consumed automatically (i.e. without user consent, ringtone, wallpaper)
    //DX_FLAG_IS_AUTOMATIC         = 0x00000080,
    //! True if the file can be rewind. This is usually false if the active RO type is count or timed count.
    DX_FLAG_CAN_REWIND           = 0x00000100,
    //! True if the file has the rights to play
    DX_FLAG_CAN_PLAY             = 0x00000200,
    //! True if the file has the rights to display
    DX_FLAG_CAN_DISPLAY          = 0x00000400,
    //! True if the file has the rights to execute
    DX_FLAG_CAN_EXECUTE          = 0x00000800,
    //! True if the file has the rights to print
    DX_FLAG_CAN_PRINT            = 0x00001000,
    //! True if the file has the rights to ring tune
    DX_FLAG_CAN_RINGTONE         = 0x00002000,
    //! True if the file has the rights to play automatically (i.e. as screen saver)
    DX_FLAG_CAN_AUTO_PLAY        = 0x00004000,
    //! True if the file has the rights to display automatically (i.e. as wallpaper)
    DX_FLAG_CAN_AUTO_DISPLAY     = 0x00008000,
    //! True if the file can be forwarded to other device
    DX_FLAG_IS_FORWARDABLE		 = 0x00010000,
    //! True if the file can be copied to removable media (some of the schemes allows only to move the content)
    DX_FLAG_IS_COPYABLE			 = 0x00020000,
    //! True if rights can be acquired for this content
    DX_FLAG_IS_EXTENDABLE        = 0x00040000,
    //! True if the clock of the file's DRM Scheme can be adjustable by a call to IDxDrmCoreFile::AdjustClock()
    DX_FLAG_CAN_ADJUST_CLOCK     = 0x00080000,
    //! True for OMA V2 content if the file has "Preview-Rights" or instant preview
    DX_FLAG_CAN_PREVIEW          = 0x00100000,
    //! True for OMA V2 content if the file has "Silent-URL"
    DX_FLAG_CAN_SILENT_ACQUIRE   = 0x00200000,
} EDxContentFlag;

typedef enum EDxDrmSettingsItem
{
    DX_SETTINGS_TRANSACTION_TRACKING,
	DX_SETTINGS_SILENT_ON_ROAMING,
    DX_NUM_OF_SETTINGS_ITEMS
}EDxDrmSettingsItem ;

typedef enum EDxPermission{
    DX_PERMISSION_NONE          = 0x00000000,
    DX_PERMISSION_TO_PLAY       = 0x00000001,   // WM, OMA-V1, OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_DISPLAY    = 0x00000002,   // OMA-V1, OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_EXECUTE    = 0x00000004,   // OMA-V1, OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_PRINT      = 0x00000008,   // OMA-V1, OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_COPY       = 0x00000010,   // OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_MOVE       = 0x00000020,   // OMA-V2.0, OMA_V2.1
    DX_PERMISSION_TO_ACCESS     = 0x00000040,   // OMA_V2.1
    DX_PERMISSION_TO_SAVE       = 0x00000080,   // OMA_V2.1
    DX_PERMISSION_TO_USE        = 0x0000000F,
    DX_ALL_PERMISSIONS          = 0x000000FF,
    DX_NUM_OF_PERMISSIONS		= 8
} EDxPermission;

typedef enum EDxConstraint {
    DX_CONSTRAINT_INDIVIDUAL	= 0x01,			// OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_SYSTEM		= 0x02,			// OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_TIME          = 0x04,         // WM, OMA-V1, OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_INTERVAL      = 0x08,         // WM, OMA-V1, OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_TIMED_COUNT   = 0x10,         // OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_COUNT         = 0x20,         // WM, OMA-V1, OMA-V2.0, OMA-V2.1
    DX_CONSTRAINT_ACCUMULATED   = 0x40,         // OMA-V2.0, OMA-V2.1
} EDxConstraint;

typedef enum EDxStoreAsset {
	DX_STORE_OMA_V2_CERTIFICATE         = 0x200,
	DX_STORE_OMA_V2_PROVISION_PACKAGE,
    DX_STORE_WM_CERTIFICATE_TEMPLATE    = 0x300,		// WMDRM certificate templated	- "devcerttemplate.dat"
    DX_STORE_WM_CERTIFICATE,							// WMDRM device certificate		- "devcert.dat"
    DX_STORE_WM_GROUP_PRIVATE_KEY,						// WMDRM group private key		- "priv.dat"
    DX_STORE_WM_FALLBACK_KEY,							// WMDRM fallback private key
	DX_STORE_PLAYREADY_CERTIFICATE_TEMPLATE  = 0x400,   // PlayReady model certificate template used to generate the final device certificate - "bgroupcert.dat"
	DX_STORE_PLAYREADY_CERTIFICATE,						// PlayReady device certificate				- "bdevcert.dat"
	DX_STORE_PLAYREADY_MODEL,							// PlayReady private key of the model cert and is used to sign the final device certificate - "zgpriv.dat"
	DX_STORE_PLAYREADY_DEVICE_SIGN,						// PlayReady private device signing key		- "zprivsig.dat"
	DX_STORE_PLAYREADY_DEVICE_ENCRYPT,					// PlayReady private device encryption key	- "zprivsig.dat"
	DX_STORE_PLAYREADY_PROVISION_PACKAGE,  // A protected package containing all the credentials required by playready
	DX_STORE_INVALID_VALUE = DX_INVALID_VALUE
} EDxStoreAsset;

typedef enum EDxRequirements{
	DX_REQUIREMENT_TRACKED          = 0x001,
	DX_REQUIREMENT_ALL              = 0x1FF
} EDxRequirements;

typedef enum EDxAsset {
    DX_ASSET_OMA_V1_RO				= 0x0001,
    DX_ASSET_OMA_V2_RO              = 0x0002,
    DX_ASSET_OMA_V2_RI_CONTEXT      = 0x0004,
    DX_ASSET_OMA_V2_DOMAIN_CONTEXT  = 0x0008,
    DX_ASSET_OMA_V2_WHITE_LIST      = 0x0010,
	DX_ASSET_OMA_V2_METERING		= 0x0020,
	DX_ASSET_OMA_V2_METERING_CONSENT= 0x0040,
    DX_ASSET_OMA_V2_PROVISIONING    = 0x0080,
    DX_ASSET_WM_LICENSE             = 0x0100,
    DX_ASSET_WM_PROVISIONING        = 0x0200,
    DX_ASSET_INTERNAL_CACHE         = 0x0400,
	DX_ASSET_SECURE_CLOCK			= 0x0800,
	DX_ASSET_HTTP_AUTHENTICATION	= 0x1000,
	DX_ASSET_PLAYREADY_PROVISIONING	= 0x2000,
	DX_ASSET_PLAYREADY_STORE		= 0x4000,
    DX_ASSET_ALL                    = 0xFFFF
} EDxAsset;

typedef enum EDxMessageCode {
    DX_MSG_ROAP_REGISTRATION_CONSENT,
    DX_MSG_ROAP_RO_JOIN_DOMAIN_CONSENT,
	DX_MSG_ROAP_LEAVE_DOMAIN_CONSENT,
	DX_MSG_ROAP_IDENTIFICATION_CONSENT,
	DX_MSG_ROAP_RO_UPLOAD_CONSENT,
	DX_MSG_METERING_CONSENT,
	DX_MSG_METERING_CONSENT_OPTIONAL,
	DX_MSG_INTERVAL_CONSENT,
	DX_MSG_INTERNET_CONNECTION_CONSENT,
	DX_MSG_DOWNLOAD_CONSENT,
	DX_MSG_METERING_COMPLETE,
	DX_MSG_IDENTIFICATION_COMPLETE,
	DX_MSG_RIGHTS_UPLOADING_COMPLETE,
	DX_MSG_REGISTRATION_COMPLETE,
	DX_MSG_JOIN_DOMAIN_COMPLETE,
	DX_MSG_LEAVE_DOMAIN_COMPLETE,
	DX_MSG_DOWNLOAD_COMPLETE,
	DX_MSG_ROAP_ERROR,
	DX_MSG_DOWNLOAD_FAILED, 
	DX_MSG_RIGHTS_EXPIRED,
	DX_MSG_SECURE_STORAGE_IS_CORRUPT,
	DX_MSG_COMMUNICATION_FAILURE,
	DX_MSG_IPC_FAILURE,
    DX_MSG_NO_RIGHTS,
	DX_MSG_RIGHTS_ABOUT_TO_EXPIRE,
	DX_MSG_STATEFUL_RIGHTS_CONSUMPTION,
    DX_MSG_LAST_COUNT,
    DX_MSG_CLOCK_NOT_SET,
    DX_MSG_RESTORE_STORAGE,
    DX_MSG_DELETE_ASSERTS,
    DX_MSG_BACKUP_CONTENT,
	DX_MSG_EXPIRED_RIGHTS,
	DX_MSG_RIGHTS_ARRIVED,
	DX_MSG_INVALID_RIGHTS_ARRIVED,
	DX_MSG_UNEXPECTED_RIGHTS_ARRIVED,
    DX_MSG_RIGHTS_PENDING,
    DX_MSG_DRM_OBJECT_NOT_RECOGNIZED,
    DX_MSG_DRM_OBJECT_NOT_SUPPORTED,
	DX_MSG_FILE_LOCKED,
	DX_MSG_FILE_LOCKED_FUTURE,
	DX_MSG_FILE_LOCKED_ROAMING,
	DX_MSG_FILE_LOCKED_FUTURE_ROAMING,
    DX_MSG_FILE_CANNOT_UNLOCK,
	DX_MSG_FILE_CANNOT_UNLOCK_FUTURE,
	DX_MSG_FILE_CANNOT_USE_AS_RINGTONE,
	DX_MSG_FILE_CANNOT_USE_FOR_AUTO_CONSUMPTION,
	DX_MSG_FILE_USE_ONLY_FOR_AUTO_CONSUMPTION,
	DX_MSG_FILE_USE_ONLY_AS_RINGTONE,
	DX_MSG_PREVIEW_CONTENT,
	DX_MSG_PREVIEW_CONTENT_FUTURE,
	DX_MSG_PREVIEW_CONTENT_ROAMING,
	DX_MSG_PREVIEW_CONTENT_FUTURE_ROAMING,
	DX_MSG_DL_FAILED_INVALID_CONTENT_FORMAT,
	DX_MSG_DL_FAILED_INVALID_RIGHTS,
	DX_MSG_DL_FAILED_INSUFFICIENT_SPACE,
    DX_MSG_SOAP_ERROR_WITH_CUSTOM_DATA,
	DX_NUM_OF_MSG_CODES,
    DX_MSG_INVALID_CODE = DX_NUM_OF_MSG_CODES
} EDxMessageCode;

typedef enum EDxMessageParamCode {
    DX_MSG_PARAM_RI_DOMAIN_NAME,
    DX_MSG_PARAM_RI_ALIAS,
    DX_MSG_PARAM_RI_FULL_NAME,
    DX_MSG_PARAM_DOMAIN_ID,
    DX_MSG_PARAM_DOMAIN_ALIAS,
    DX_MSG_PARAM_DOMAIN_NAME,
    DX_MSG_PARAM_RO_ALIAS,
    DX_MSG_PARAM_ROAP_ERROR_MSG,
    DX_MSG_PARAM_ROAP_STATUS,
    DX_MSG_PARAM_ROAP_ERROR_REDIRECT_URL,
    //DX_MSG_PARAM_ROAP_OPERATION,
    DX_MSG_PARAM_CONTENT_ID,
	DX_MSG_PARAM_INFO_URL,
	DX_MSG_PARAM_ICON_URI,
	DX_MSG_PARAM_NAME,
	DX_MSG_PARAM_DESCRIPTION,
	DX_MSG_PARAM_VENDOR,
	DX_MSG_PARAM_SIZE,
	DX_MSG_PARAM_TYPE,
	DX_MSG_PARAM_OBJECT_URI,
	DX_MSG_PARAM_CONTENT_FILE_NAME,
	DX_MSG_PARAM_COUNTS_LEFT,
	DX_MSG_PARAM_TIME_LEFT,
    DX_MSG_PARAM_SOAP_ERROR_CUSTOM_DATA,
	DX_NUM_OF_MSG_PARAMS,
} EDxMessageParamCode;

typedef enum EDxMessageType {
    DX_MSG_TYPE_OK,
    DX_MSG_TYPE_OK_CANCEL,
    DX_MSG_TYPE_YES_NO,
} EDxMessageType;

typedef enum EDxMessageSeverity
{
    DX_MSG_SEVERITY_ERROR,
    DX_MSG_SEVERITY_WARNING,
	DX_MSG_SEVERITY_INFORMATION,
	DX_MSG_SEVERITY_QUESTION,
} EDxMessageSeverity;

typedef enum EDxUserResponse
{
    DX_USER_RESPONSE_OK,
    DX_USER_RESPONSE_YES = DX_USER_RESPONSE_OK,
    DX_USER_RESPONSE_CANCEL,
    DX_USER_RESPONSE_NO,
} EDxUserResponse;

typedef enum EDxContentSeek
{
    DX_CONTENT_SEEK_SET,
    DX_CONTENT_SEEK_CUR,
    DX_CONTENT_SEEK_END
} EDxContentSeek;

typedef enum EDxHttpMethod
{
    DX_HTTP_METHOD_GET,
    DX_HTTP_METHOD_POST
} EDxHttpMethod;

typedef enum EDxRightsObjectStatus
{
    DX_RO_STATUS_NOT_VALID = 0,
    DX_RO_STATUS_SIM_MISMATCH,
    DX_RO_STATUS_NOT_IN_DOMAIN,
    DX_RO_STATUS_CLOCK_NOT_SET,
    DX_RO_STATUS_EXPIRED,
	DX_RO_STATUS_FUTURE,
	DX_RO_STATUS_METERING_FORBIDDEN,
	DX_RO_STATUS_METERING_CONSENT_REQUIRED,
	DX_RO_STATUS_METERING_CONSENT_OPTIONAL,
    DX_RO_STATUS_VALID,
} EDxRightsObjectStatus;

typedef enum EDxFileOpenMode
{
    DX_OPEN_FOR_READ = 1,
    DX_OPEN_FOR_WRITE = 2,
    DX_OPEN_FOR_READWRITE = 3,
} EDxFileOpenMode;

typedef enum EDxRightsAcquisitionMethod
{
    DX_RIGHT_ACQ_METHOD_NORMAL,
    DX_RIGHT_ACQ_METHOD_SILENT,
    DX_RIGHT_ACQ_METHOD_PREVIEW,
    DX_NUM_OF_ACQ_METHODS
} EDxRightsAcquisitionMethod;

typedef enum
{
    DX_USE_NO_RESTRICTION,
    DX_USE_ONLY_USER_INITIATED,
    DX_USE_ONLY_AUTOMATED_USE,
    DX_USE_NOT_AS_RINGTONE,
    DX_USE_NOT_AS_WALLPAPER_OR_SCREEN_SAVER,
} EDxUseRestriction;

typedef enum
{
	DX_CONTENT_TYPE_UNKNOWN,
	DX_CONTENT_TYPE_DRM_FILE,
	DX_CONTENT_TYPE_DRM_STREAM,
	DX_CONTENT_TYPE_DRM_FILE_AND_STREAM,
} EDxDrmContentType;

typedef enum
{
	DX_RO_ITERATION_DISPLAY,
	DX_RO_ITERATION_FULL_INFO,
	DX_RO_ITERATION_INVALID,
} EDxROIterationMode;

#define DX_MAX_MIME_TYPE_SIZE    ((256+1)*2)// for prevent utf8 to utf16 conversion problem. 

//! A structure that holds time-date value.
typedef struct {
    DxUint32 tm_year;    /*!< full year */
    DxUint32 tm_mon;     /*!< months since January - [1 - 12] */
    DxUint32 tm_mday;    /*!< day of the month - [1,31] */
    DxUint32 tm_hour;    /*!< hours since midnight - [0,23] */
    DxUint32 tm_min;     /*!< minutes after the hour - [0,59] */
    DxUint32 tm_sec;     /*!< seconds after the minute - [0,59] */
} DxTimeStruct;

//! A structure that holds Mtp command buffers for WMDRM.
typedef struct 
{
	DxUint32 m_Length;
	DxByte* m_Data;
} DxDrmBuffer;


#endif

