

#ifndef SKDTSHD2_H
#define SKDTSHD2_H 1


#include <stdio.h>                                                                                               
                                                                                                                 
                                                                                                                             
                                                                                                                                      
 typedef unsigned char dtsUint8 ;                                                                                            
 typedef signed char dtsInt8 ;                                                                                               
 typedef unsigned short dtsUint16 ;                                                                                          
 typedef short dtsInt16 ;                                                                                                    
 typedef unsigned int dtsUint32 ;                                                                                            
 typedef int dtsInt32 ;                                                                                                      
 typedef unsigned long long dtsUint64 ;                                                                                      
 typedef long long dtsInt64 ;                                                                                                
                                                                                                                             
 #define dtsUint16SizeBytes	(sizeof(dtsUint16))                                                                             
 #define dtsUint32SizeBytes	(sizeof(dtsUint32))                                                                             
 #define dtsUint64SizeBytes	(sizeof(dtsUint64))                                                                             
                                                                                                                             
 typedef enum                                                                                                                
 {                                                                                                                           
 	dtsTrue = 1,                                                                                                              
 	dtsFalse = 0                                                                                                              
 } dtsBoolean ;                                                                                                              
                                                                                                                             
 enum LEGACY_MASK                                                                                                            
 {                                                                                                                           
   	SKDTSSPKROUT_MASK_C 		= 0x00001,                                                                                        
     	SKDTSSPKROUT_MASK_LR		= 0x00002,	                                                                                      
     	SKDTSSPKROUT_MASK_LsRs 	= 0x00004,                                                                                        
     	SKDTSSPKROUT_MASK_LFE1 	= 0x00008,                                                                                        
     	SKDTSSPKROUT_MASK_Cs 		= 0x00010,                                                                                        
     	SKDTSSPKROUT_MASK_LhRh 	= 0x00020,                                                                                        
     	SKDTSSPKROUT_MASK_LsrRsr	= 0x00040,                                                                                      
     	SKDTSSPKROUT_MASK_Ch 		= 0x00080,                                                                                        
     	SKDTSSPKROUT_MASK_Oh 		= 0x00100,                                                                                        
     	SKDTSSPKROUT_MASK_LcRc	= 0x00200,                                                                                        
     	SKDTSSPKROUT_MASK_LwRw	= 0x00400,                                                                                        
     	SKDTSSPKROUT_MASK_LssRss	= 0x00800,                                                                                      
     	SKDTSSPKROUT_MASK_LFE_2	= 0x01000,                                                                                        
     	SKDTSSPKROUT_MASK_LhsRhs	= 0x02000,                                                                                      
     	SKDTSSPKROUT_MASK_Chr		= 0x04000,                                                                                        
     	SKDTSSPKROUT_MASK_LhrRhr	= 0x08000,                                                                                      
     	SKDTSSPKROUT_MASK_Clf		= 0x10000,                                                                                        
     	SKDTSSPKROUT_MASK_LlfRlf	= 0x20000,                                                                                      
     	SKDTSSPKROUT_MASK_LtRt	= 0x40000                                                                                         
     };                                                                                                                          
                                                                                                                                 
     typedef enum                                                                                                                
   {                                                                                                                           
   	dtsDecoderOperationMode_Lossy,                                                                                            
   	dtsDecoderOperationMode_LossLess                                                                                          
   } skdtsDecoderOperationMode ;                                                                                               
                                                                                                                               
                                                                                                                               
   typedef struct skdtsRepSet                                                                                                  
   {                                                                                                                           
   	dtsUint8	repGroup ;                                                                                                      
   	dtsUint8	repChSet ;                                                                                                      
   } skdtsRepSet;                                                                                                              
                                                                                                                               
                                                                                                                               
   typedef struct skdtsDecoderConfig                                                                                           
   {                                                                                                                           
   	dtsUint32				spkrOut ;                                                                                                 
   	dtsBoolean				remapRequired ;                                                                                
   	skdtsDecoderOperationMode mode ;                                                                                 
   	dtsUint32				subStreamId ;                                                                                    
   	dtsUint32				audioPresNum ;                                                                                   
   	dtsBoolean				interpolate ;                                                                                  
   	dtsBoolean				coreOnly ;                                                                                     
   	dtsBoolean				coreSStreamOnly ;                                                                              
   	dtsBoolean				only96From192 ;                                                                                
   	dtsBoolean				multiAsset ;                                                                                   
   	dtsUint32				upSampleRate ;                                                                                   
   	skdtsRepSet				replacementSet[3] ;                                                                            
   	dtsInt32				sndFldIdx ;                                                                                      
   	dtsBoolean				noDecDefSpkrRmp ;                                                                              
   	dtsBoolean				lfeMixedToFrontWhenNoDedicatedLFEOutput;                                                       
   	dtsBoolean				enableDownmix;                                                                                 
   dtsInt32				lfeFilterType;			// lfe filter type, 2011.01.25                                                     
   #ifndef NOCODEFOR_MODEL_ANALOG_COMPENSATION                                                                        
   	dtsBoolean				enableAnalogCompensation;                                                                      
   #endif                                                                                                             
   }  skdtsDecoderConfig;                                                                                             
                                                                                                                      
   typedef struct skdtsDRCConfig1                                                                                     
   {                                                                                                                  
   	dtsUint32	DRCPercent;				                                                                                     
   } skdtsDRCConfig1;                                                                                                 
                                                                                                                      
                                                                                                                      
   typedef enum                                                                                                       
   {                                                                                                                  
   	DTSPLAYERPEAKLIMIT_SOFT_LINEAR = 0,                                                                              
   	DTSPLAYERPEAKLIMIT_HARD	,	                                                                                       
   	DTSPLAYERPEAKLIMIT_SOFT_CUBIC_SPLINE                                                                             
   }skdtsPlayePeakLimiterType;	                                                                                       
                                                                                                                      
                                                                                                                      
   typedef struct                                                                                                     
   {                                                                                                                  
   	dtsBoolean	verifyMode;                                                                                          
   	dtsBoolean	decodeModeDownmix;                                                                                   
   	skdtsPlayePeakLimiterType	peakLimiter;                                                                           
   	dtsUint32 outputBitWidth;                                                                                        
   	dtsBoolean forceMonoOutput;                                                                                      
   	dtsBoolean autoStartFadeIn;                                                                                      
   	dtsBoolean disablePCMFadeIn;                                                                                     
   } skdtsPlayerControlConfig;                                                                                        
                                                                                                                      
                                                                                                                      
   typedef struct skdtsDialNormConfig1                                                                                
   {                                                                                                                  
   	dtsBoolean				enableDialNorm ;                                                                               
   } skdtsDialNormConfig1;                                                                                            
                                                                                                                      
                                                                                                                      
   typedef struct skdtsPlayerConfig                                                                                   
   {                                                                                                                  
   	skdtsDecoderConfig		primaryDecoderConfig;                                                                      
   	skdtsPlayerControlConfig	playerControlConfig;                                                                   
   	skdtsDialNormConfig1		dialNormConfig;                                                                          
   	skdtsDRCConfig1			DRCConfig;                                                                                   
   } skdtsPlayerConfig;                                                                                             
                                                                                                 
                                                                                                                             
                                                                                                                             
                                                                                                                             
                                                                                                                                                                                                              
                                                                                                                 
                                                                                                                 
    

typedef enum 
{
	SKDTS_SPEAKER_CENTRE = 0x00000001,
	SKDTS_SPEAKER_LEFT   = 0x00000002,
	SKDTS_SPEAKER_RIGHT  = 0x00000004,
	SKDTS_SPEAKER_LS     = 0x00000008,
	SKDTS_SPEAKER_RS     = 0x00000010,
	SKDTS_SPEAKER_LFE1   = 0x00000020,
	SKDTS_SPEAKER_Cs     = 0x00000040,
	SKDTS_SPEAKER_Lsr    = 0x00000080,
	SKDTS_SPEAKER_Rsr    = 0x00000100,
	SKDTS_SPEAKER_Lss    = 0x00000200,
	SKDTS_SPEAKER_Rss    = 0x00000400,
	SKDTS_SPEAKER_Lc     = 0x00000800,
	SKDTS_SPEAKER_Rc     = 0x00001000,
	SKDTS_SPEAKER_Lh     = 0x00002000,
	SKDTS_SPEAKER_Ch     = 0x00004000,
	SKDTS_SPEAKER_Rh     = 0x00008000,
	SKDTS_SPEAKER_LFE2   = 0x00010000,
	SKDTS_SPEAKER_Lw     = 0x00020000,
	SKDTS_SPEAKER_Rw     = 0x00040000,
	SKDTS_SPEAKER_Oh     = 0x00080000,
	SKDTS_SPEAKER_Lhs    = 0x00100000,
	SKDTS_SPEAKER_Rhs    = 0x00200000,
	SKDTS_SPEAKER_Chr    = 0x00400000,
	SKDTS_SPEAKER_Lhr    = 0x00800000,
	SKDTS_SPEAKER_Rhr    = 0x01000000,
	SKDTS_SPEAKER_Clf	= 0x02000000,
	SKDTS_SPEAKER_Llf	= 0x04000000,
	SKDTS_SPEAKER_Rlf	= 0x08000000,
	SKDTS_SPEAKER_Lt     = 0x10000000,
	SKDTS_SPEAKER_Rt     = 0x20000000
} dtsSpeakerMask ;

enum CONVERTED_SPKROUT_MASK 
{
	CONV_MASK_C 		= SKDTS_SPEAKER_CENTRE,
	CONV_MASK_LR		= SKDTS_SPEAKER_LEFT | SKDTS_SPEAKER_RIGHT,	
	CONV_MASK_LsRs 	= SKDTS_SPEAKER_LS | SKDTS_SPEAKER_RS,
	CONV_MASK_LFE1 	= SKDTS_SPEAKER_LFE1,
	CONV_MASK_Cs 	= SKDTS_SPEAKER_Cs,
	CONV_MASK_LhRh 	= SKDTS_SPEAKER_Lh | SKDTS_SPEAKER_Rh,
	CONV_MASK_LsrRsr	= SKDTS_SPEAKER_Lsr | SKDTS_SPEAKER_Rsr,
	CONV_MASK_Ch 	= SKDTS_SPEAKER_Ch,
	CONV_MASK_Oh 	= SKDTS_SPEAKER_Oh,
	CONV_MASK_LcRc	= SKDTS_SPEAKER_Lc | SKDTS_SPEAKER_Rc,
	CONV_MASK_LwRw	= SKDTS_SPEAKER_Lw | SKDTS_SPEAKER_Rw,
	CONV_MASK_LssRss	= SKDTS_SPEAKER_Lss | SKDTS_SPEAKER_Rss,
	CONV_MASK_LFE_2	= SKDTS_SPEAKER_LFE2,
	CONV_MASK_LhsRhs	= SKDTS_SPEAKER_Lhs | SKDTS_SPEAKER_Rhs,
	CONV_MASK_Chr	= SKDTS_SPEAKER_Chr,
	CONV_MASK_LhrRhr	= SKDTS_SPEAKER_Lhr | SKDTS_SPEAKER_Rhr,
	CONV_MASK_Clf	= SKDTS_SPEAKER_Clf,
	CONV_MASK_LlfRlf	= SKDTS_SPEAKER_Llf | SKDTS_SPEAKER_Rlf,
	CONV_MASK_LtRt	= SKDTS_SPEAKER_Lt | SKDTS_SPEAKER_Rt
};






typedef struct
{
	union
	{
		void *			addr;
		unsigned char * byte_addr;
		short *			int16_addr;
		int *			int32_addr;
	};
	unsigned	size;	// in unit of bytes
} buffer_t;




typedef enum {
	ID_SAMPLING_RATE = 0,
	ID_BITRATE,
	ID_CHANNEL_NUMBER,
	ID_FRAME_COUNT,
	ID_ERRNO,
	ID_VERSION,
	ID_TRACK_END_FLAG,
	ID_VERBOSE_LEVEL,
	ID_SPEAKER_MAP,
	ID_OTHER_START = 128
} audio_param_t;


//=== dtsd_api.h ======

#define DTS_BITSTREAM_BUFFER_SIZE	(4096 * 2)
#define DTS_MAX_FRAME_SAMPLES		4096

// Decoder interface
#define DTS_WORK_SPACE				(150*1024)	// bytes
typedef struct {
	long long __internal[(DTS_WORK_SPACE+sizeof(long long)-1)/sizeof(long long)];
} dts_decoder_t;

typedef struct dtsd_config_param_t
{
	int enableDownmix;				// enable downmix
	int DwnMixLFEMIXSet;			// mix LFE 
	int destinationSpeakerMask;		// speaker mask
	int enableAnalogCompensation;	// enable analog compensation
	int enableDialNorm;				// enable dial norm
	int	DRCPercent;					// DRC Percent
	int lfeFilterType;				// lfe filter type 2011.01.25 

} dtsd_config_param_t;


int dtsd_set_config(void *, void *);
int dtsd_init(void *, buffer_t *);
int dtsd_run(void *, int, int *);
int dtsd_get_read_index(void *);
int dtsd_set_int(void *, int, int);
int dtsd_reset(void *);
int dtsd_get_int(void *, int );


#endif
