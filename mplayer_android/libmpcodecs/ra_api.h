int Skymedia_Ra_Init(char *filename, int filelen, int *ch, int *srate, int *bitrate, int audio_out);
#ifndef RA_DEBUG
int Skymedia_Ra_Decode(short * outbuf);
#else
int Skymedia_Ra_Decode(short *outbuf, int *cmp);
#endif
void Skymedia_Ra_Uninit(void);

int Gecko2InitDecode(unsigned char* pBuf, int len, int nFrameBits, int nChannels, int sampRate);
#ifndef RA_DEBUG
int Gecko2Dec(int code_size, unsigned char *codebuf, int lostflag, int *size, short *outbuf, int ch);
#else
int Gecko2Dec(int code_size, unsigned char *codebuf, int lostflag, int *size, short *outbuf, int *outbuf1, int ch);
#endif
void Gecko2Uninit(void);