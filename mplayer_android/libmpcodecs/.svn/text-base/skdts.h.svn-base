

#ifndef LIBDCA_COMPAT_H
# define LIBDCA_COMPAT_H 1


typedef int level_t;

#define DCA_MONO 0
#define DCA_CHANNEL 1
#define DCA_STEREO 2
#define DCA_STEREO_SUMDIFF 3
#define DCA_STEREO_TOTAL 4
#define DCA_3F 5
#define DCA_2F1R 6
#define DCA_3F1R 7
#define DCA_2F2R 8
#define DCA_3F2R 9
#define DCA_4F2R 10

#define DCA_DOLBY 101 /* FIXME */

#define DCA_CHANNEL_MAX  DCA_3F2R /* We don't handle anything above that */
#define DCA_CHANNEL_BITS 6
#define DCA_CHANNEL_MASK 0x3F

#define DCA_LFE 0x80
#define DCA_ADJUST_LEVEL 0x100

//typedef struct dca_state_s dca_state_t;

//////////////////////////////////////////////////////////////////////


# define DTS_MONO DCA_MONO
# define DTS_CHANNEL DCA_CHANNEL
# define DTS_STEREO DCA_STEREO
# define DTS_STEREO_SUMDIFF DCA_STEREO_SUMDIFF
# define DTS_STEREO_TOTAL DCA_STEREO_TOTAL
# define DTS_3F DCA_3F
# define DTS_2F1R DCA_2F1R
# define DTS_3F1R DCA_3F1R
# define DTS_2F2R DCA_2F2R
# define DTS_3F2R DCA_3F2R
# define DTS_4F2R DCA_4F2R
# define DTS_DOLBY DCA_DOLBY
# define DTS_CHANNEL_MAX DCA_CHANNEL_MAX
# define DTS_CHANNEL_BITS DCA_CHANNEL_BITS
# define DTS_CHANNEL_MASK DCA_CHANNEL_MASK
# define DTS_LFE DCA_LFE
# define DTS_ADJUST_LEVEL DCA_ADJUST_LEVEL
# define HEADER_SIZE 14


int skdts_syncinfo(void *state, unsigned char * buf, int * flags,
                  int * sample_rate, int * bit_rate, int *frame_length, int buflen,int cons);

int sadts_syncinfo(void *state, unsigned char * buf, int * flags,
                  int * sample_rate, int * bit_rate, int *frame_length, int buflen,int cons);

int skdts_frame (void * state, unsigned char * buf, int * flags, int header_len);
int skdts_blocks_num (void * state);
int skdts_block (void * state);
int * skdts_samples (void * state);
void * skdts_init (void);
void skdts_free (void * state);
void skdts_dynrng (void * state,
                 level_t (* call) (level_t, void *), void * data);
void skdts_convert2s16(int *f, short *s16, int flags, int ch_out);


#endif
