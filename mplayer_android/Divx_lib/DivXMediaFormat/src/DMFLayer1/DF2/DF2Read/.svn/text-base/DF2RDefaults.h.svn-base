/*!
    @file
@verbatim
$Id: DF2RDefaults.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _DF2RDEFAULTS_H_
#define _DF2RDEFAULTS_H_

#define AVI_STDIDX_NKEYFRAME    1U << 31

/*
    These are the default settings for:

      Home Theater Profile
 */
#ifdef HOME_THEATER_PROFILE

#define MAX_VID_TRACKS 1
#define MAX_AUD_TRACKS 8
#define MAX_SUB_TRACKS 8

/*
    These are the default settings for:

      Mobile Profile
 */
#elif defined ( MOBILE_PROFILE )

#define MAX_VID_TRACKS 1
#define MAX_AUD_TRACKS 1
#define MAX_SUB_TRACKS 0

/*
    These are the default settings for:

      undefined profile
 */
#else

#define MAX_VID_TRACKS 1
#define MAX_AUD_TRACKS 8
#define MAX_SUB_TRACKS 8

#endif

#endif /* _DF2RDEFAULTS_H_ */
