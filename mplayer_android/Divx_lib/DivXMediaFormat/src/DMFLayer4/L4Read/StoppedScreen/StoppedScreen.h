/*!

@file
@verbatim
$Header$

Copyright (c) 2009 DivXNetworks, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

 */

#ifndef _STOPPEDSCREEN_H
#define _STOPPEDSCREEN_H

/*********************************************************************************************
 *  Includes
 */
#include "DPRGlobals.h"

/*********************************************************************************************
 *  Defines
 */

/*! Return codes */
#define PB_OK                               DPR_OK
#define PB_FAIL                             DPR_FAIL

/*! Progress bar co-ordinates */
#define STOPPED_SCREEN_POS_X                  90
#define STOPPED_SCREEN_POS_Y                  400
#define STOPPED_SCREEN_WIDTH                  470

/*! Tick bar params */
#define STOPPED_SCREEN_TICK_OFFSET_Y1         (STOPPED_SCREEN_POS_Y - 8)
#define STOPPED_SCREEN_TICK_OFFSET_Y2         (STOPPED_SCREEN_POS_Y + 6)
#define STOPPED_SCREEN_TICK_W                 10
#define STOPPED_SCREEN_TICK_COLOR             0xFFAAAAAA

/*! FF Icon (Used for REW too) */
#define PNG_FILE_BACKGROUND                         "background2.png"

/*********************************************************************************************
 *  Typedefs
 */

/*! Opaque Progressbar handle */
typedef struct _StoppedScreen
{
    /*! Handle to DPROverlay*/
    HDPROVERLAY   overlayHandle;

    /*! Handle to resized png */
    uint8_t       *pIcon;
    uint32_t      iconwidth;
    uint32_t      iconheight;
    uint32_t      size;
} *HSS;

/*********************************************************************************************
 *  External APIs
 */

/*!
  Create a stopped screen bar.

  @param phStoppedScreen (OUT) - Progress bar handle.
  @param hDPROverlay   (IN)  - DPROverlay handle.

  @return PB_OK for success
    PB_FAIL for other error
 */
int StoppedScreen_Load(HSS *phStoppedScreen, HDPROVERLAY hDPROverlay);

/*!
  Deletes a stopped screen bar.

  @param hStoppedScreen (IN) - Progress bar handle.

  @return PB_OK for success
    PB_FAIL for other error
 */
int StoppedScreen_Delete(HSS hStoppedScreen);

/*!
  Displays a stopped screen bar, based on the params passed in

  @param hStoppedScreen (IN) - Progress bar handle.
  @param pParams      (IN) - Progress bar handle params, defined in struct StoppedScreenParams.

  @return PB_OK for success
    PB_FAIL for other error
 */

#endif
