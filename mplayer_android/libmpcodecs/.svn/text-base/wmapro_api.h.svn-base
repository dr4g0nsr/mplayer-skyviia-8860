
#define MAX_BUFFER_OUT  8192*8

int Skymedia_WmaPro_Init(char *filename, int filelen,int *ch, int *srate, int *bitrate, int *sz, int *cm, int outchannel);
int Skymedia_Wma_Decode(short * outbuf);
void Skymedia_WmaPro_Uninit(void);

int WMAPRO_Init(unsigned short nDecoderFlags,                                    
                                    unsigned int nDstChannelMask,  
                                    int nSamplesPerSec, int *nChannels, int nAvgBytesPerSec,
                                    int wValidBitsPerSample, int wFormatTag, int dwChannelMask,
                                    short nEncodeOpt, int nBlockAlign, int wBitsPerSample,
                                    short wAdvancedEncodeOpt, int dwAdvancedEncodeOpt2, int *ss, int *srate, int *BPFS);





int WMAPRO_Decode(unsigned char *buffer, unsigned char *inbuf, int in_len, int *status);
void WMAPRO_Uninit(void);
void WMAPro_Resync(void);



