/*----------------------------------------------------------------------------------------
 *
 *			S K Y M E D I   P R O P R I E T A R Y
 *
 * 	COPYRIGHT (c) 2003 BY SKYMEDI CORPORATION TAIWAN.
 * 		--  ALL RIGHTS RESERVED  --
 *       	SKYMEDI Confidential; Need to Know only.
 *       	Protected as an unpublished work.
 *
 *	The computer program listings, specifications and documentation
 *	herein are the property of SKYMEDI CORPORATION and shall not be
 * reproduced, copied, disclosed, or used in whole or in part for
 * any reason without the prior express written permission of
 * SKYMEDI CORPORATION.
 *
 *		File Name:      a52.h
 *		Author:         Duncan Lee
 *		Created:        Feb. 03, 2009
 *		Revision:         
 *
 *----------------------------------------------------------------------------------------
 */

/*
 * a52.h
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of a52dec, a free ATSC A-52 stream decoder.
 * See http://liba52.sourceforge.net/ for updates.
 *
 * a52dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * a52dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef A52_H
#define A52_H

#ifndef LIBA52_FIXED
#define LIBA52_FIXED
#endif

typedef int sample_t;
typedef int level_t;

typedef sample_t convert_t;


#ifdef LIBA52_FIXED
// #define CONVERT_LEVEL (2 << 26)
//#define CONVERT_LEVEL (2 << 24)              // Q28
#define CONVERT_LEVEL (2 << 24)              // Q28

#define CONVERT_BIAS             (0)
#else
#define CONVERT_LEVEL            (2)
#define CONVERT_BIAS             (384)
#endif

typedef struct a52_state_s a52_state_t;

#define A52_CHANNEL              (0)
#define A52_MONO                 (1)
#define A52_STEREO               (2)
#define A52_3F                   (3)
#define A52_2F1R                 (4)
#define A52_3F1R                 (5)
#define A52_2F2R                 (6)
#define A52_3F2R                 (7)
#define A52_CHANNEL1             (8)
#define A52_CHANNEL2             (9)
#define A52_DOLBY                (10)
#define A52_CHANNEL_MASK         (15)

#define A52_LFE                  (16)
#define A52_ADJUST_LEVEL         (32)

a52_state_t * a52_init (unsigned int mm_accel);
sample_t * a52_samples (a52_state_t * state);
int a52_syncinfo (unsigned char * buf, int * flags,
		  int * sample_rate, int * bit_rate);
int a52_frame (a52_state_t * state, unsigned char * buf, int * flags,
	       level_t * level, sample_t bias);
void a52_dynrng (a52_state_t * state,
		 level_t (* call) (level_t, void *), void * data);
int a52_block (a52_state_t * state);
void a52_free (a52_state_t * state);

void* a52_resample_init(unsigned int mm_accel,int flags,int chans);
extern int (* a52_resample) (sample_t * _f, short * s16);

static void swab_16(void* dst, void* src, int len)
{
   unsigned short *d16, *s16;
   unsigned short t;
   int i;
   d16 = (unsigned short *)dst;
   s16 = (unsigned short *)src;
   len >>= 1;
   for (i=0; i<len; i++){
      t = *s16++;
      t = (t>>8) + ((t&0xff)<<8);
      *d16++ = t;
   }
}

#endif /* A52_H */
