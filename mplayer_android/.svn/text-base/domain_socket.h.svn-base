/* Control */
#define SET 0x1
#define GET 0x2

// ==== SET ====
/***** Class *****/
#define PLAY_PROC			0x10	// play status pause, resume, stop
#define SEEK_PROC			0x20	// seek
#define VOLUME_PROC			0x30	// VOLUME control
#define AUDIO_PROC			0x40	//change audio stream
#define FF_PROC				0x50	// FF
#define FB_PROC				0x60	// FB
#define SUB_PROC			0x70	//change subtitle stream	,charleskao 10022009
#define CMD_PROC			0x80	// command instruction

/***** Value *****/
// play status pause, resume, stop
#define PLAY_PAUSE			0x01	// 
#define PLAY_STOP			0x02	// quit
#define PLAY_NORMAL_SPEED		0x03	// Play speed normal

//seek
#define SEEK_SEC			0x1	    // 
// seek other func
#define SEEK_FF_10_SEC			0xE	    // Seek to next 10 secs position
#define SEEK_FB_10_SEC			0xF	    // Seek to last 10 secs position

// VOLUME control
#define VOLUME_MUTE			0x1	    // 
#define VOLUME_UP			0x2	    // 
#define VOLUME_DOWN			0x3	    // 

//FF SPEED
#define FF_SPEED2X			0x1	    // 2x
#define FF_SPEED4X			0x2	    // 4x
#define FF_SPEED8X			0x3	    // 8x
#define FF_SPEED16X			0x4	    // 16x

//FB SPEED
#define FB_SPEED2X			0x1	    // 2x
#define FB_SPEED4X			0x2	    // 4x
#define FB_SPEED8X			0x3	    // 8x
#define FB_SPEED16X			0x4	    // 16x

//subtitle
#define SUB_UP				0x01	//next up
#define SUB_DOWN			0x02	//next down

//audio
#define AUDIO_UP			0x01	//next

//command instruction
#define CMD_INST			0x01

// ==== GET ====
/* Class */
#define GET_PLAYER_STATUS_PROC          0x10  // get player status pause,...
#define GET_CURRENT_POS_PROC            0x20  // get current position
#define GET_DURATION_PROC               0x30  // get duration
#define GET_AUDIO_LANG_PROC             0x40  // get audio multilingual
#define GET_SUBINFO_PROC                0x50
#define GET_AUDIOINFO_PROC              0x60
#define GET_BUFFERING_STATUS_PROC       0x70  // get stream buffering status

/* Value */ /* get control Return value */
// get player status pause, 
#define PLAYER_STATUS_STOP		0x00	// 
#define PLAYER_STATUS_PLAYING		0x01	// 
#define PLAYER_STATUS_PAUSE		0x02	// 
#define PLAYER_STATUS_IDLE		0x03	// 

// get current position
#define GET_CURRENT_POS_SEC			0x1	    // get current position return sec.
#define GET_CURRENT_POS_SEC_CALL_BACK_ON	0x2	    // on: get current position call back return sec.
#define GET_CURRENT_POS_SEC_CALL_BACK_OFF	0x3	    // off: get current position call back return sec.

// get duration
#define GET_DURATION_SEC		0x1	    // get duration return sec.

// get audio multilingual
#define GET_AUDIO_MULTILANG		0x1	    // get audio multilingual.

//	get subtitle infomations
#define GET_CURRENT_SUB			0x01
#define GET_SUB				0x02

//	get audio infomations
#define GET_CURRENT_AUDIO_LAN		0x01
#define GET_AUDIO_LAN			0x02
#define GET_CURRENT_AUDIO_VOLUME	0x03

//	get stream buffering status
#define GET_BUFFERING_PERCENTAGE	0x01
#define GET_BUFFERING_CALL_BACK_ON	0x02
#define GET_BUFFERING_CALL_BACK_OFF	0x03

/* ??
#define PLAYER_STATUS_SPEED_NORMAL  1	// 
#define PLAYER_STATUS_SPEED_FF_2X   2	// 
#define PLAYER_STATUS_SPEED_FF_4X   3	// 
#define PLAYER_STATUS_SPEED_FF_8X   4   // 
#define PLAYER_STATUS_SPEED_FF_16X  5	// 
#define PLAYER_STATUS_SPEED_FB_2X   6	// 
#define PLAYER_STATUS_SPEED_FB_4X   7	// 
#define PLAYER_STATUS_SPEED_FB_8X   8	// 
#define PLAYER_STATUS_SPEED_FB_16X  9   // 
*/

#define MAX_TIME_OUT_COUNT	10000

typedef struct _GENERAL_PKT
{
	//header
	unsigned char 	Control;
	unsigned char 	Subtype;

	unsigned short	DataSize;
	unsigned char	Data[4];

	unsigned char	CmdInst[1024];
}GeneralPkt, *pGeneralPkt;

