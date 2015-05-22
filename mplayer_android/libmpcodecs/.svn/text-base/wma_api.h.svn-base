int Skymedia_Wma_Init(char *filename, int filelen, int *ch, int *srate, int *bitrate);
int Skymedia_Wma_Decode(short * outbuf, int *cmp_buf);
void Skymedia_Wma_Uninit(void);

#ifndef WMA_DEBUG
int wma_decode_superframe(void *data, int *data_size,
                                 unsigned char *buf, int buf_size);
#else
int wma_decode_superframe(void *data, int *cmp_buf, int *data_size,
                                 unsigned char *buf, int buf_size);
#endif
int wma_init(int ch, int sample_rate, int bit_rate, int block_align, int format, int flags2);
int wma_end(void);
void wma_resync();

