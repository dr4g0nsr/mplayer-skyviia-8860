/*!
    @file
@verbatim
$Id: L4Menus.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _L4CONFIG_H_
#define _L4CONFIG_H_

#define REINIT_PRELOAD 2000*DIVX_TIME_MILLISECOND //DivXTime

/* Font defaults */
#ifdef WIN32
#define FONT_SIZE               48
#else
#define FONT_SIZE               32
#endif
#define DISPRES_DPI             72
#define BMP_HEADER_SIZE         62 + 2 + 2

#ifdef WIN32
#define LOCAL_FONT "C:/WINDOWS/Fonts/ARIALUNI.TTF"
#else
#define LOCAL_FONT "ARIALUNI.TTF"
#endif

#ifdef WIN32
#define HEIGHT_OFFSET           40
#else
#define HEIGHT_OFFSET           25
#endif
#define WIDTH_HD                1280
#define HEIGHT_HD               720
#define OVERLAY_CHUNK_SIZE      WIDTH_HD * HEIGHT_HD * 3
#define FONT_START_X            50
#define FONT_START_Y            100
#define MAX_STR_LEN             100

/* Defines the max supported resolution */
#define MAX_RESOLUTION_WIDTH  1920
#define MAX_RESOLUTION_HEIGHT 1080

/* Defines the max supported frame rates */
#define MAX_FRAMERATE_30FPS 30
#define MAX_FRAMERATE_60FPS 60
#define MAX_FRAMERATE_25FPS 25

/* Defines DRM version*/
#define MAX_DRM_VERSION 2

#ifdef __cplusplus
}
#endif

#endif //_L4CONFIG_H_
