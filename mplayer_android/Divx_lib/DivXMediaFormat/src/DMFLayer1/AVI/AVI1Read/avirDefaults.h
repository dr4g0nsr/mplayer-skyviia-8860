/*!
    @file
@verbatim
$Id: avirDefaults.h 59665 2009-04-15 02:37:25Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _AVIRDEFAULTS_H_
#define _AVIRDEFAULTS_H_

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

#define MAX_ALLOCATED_KEY_CACHE 3740 /* Enough for 3 hours at 3 frames per sec with a keyframe every second */
#define CACHE_ENTRIES 1000

#endif /* _AVIRDEFAULTS_H_ */
