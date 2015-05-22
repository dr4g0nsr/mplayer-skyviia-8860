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

#ifndef PROGBAR_H
#define PROGBAR_H

/*********************************************************************************************
 *  Includes
 */
#include "DPRGlobals.h"
#include "ProgressBarParams.h"

/*********************************************************************************************
 *  Defines
 */

/*! Return codes */
#define PB_OK                               DPR_OK
#define PB_FAIL                             DPR_FAIL

/*! Background progress bar */
#define PNG_FILE_PROGRESS_BKG               "progress_handle.png"

/*! Progress bar co-ordinates */
#define PROGRESS_BAR_POS_X                  90
#define PROGRESS_BAR_POS_Y                  400
#define PROGRESS_BAR_WIDTH                  470

/*! Tick bar params */
#define PROGRESS_BAR_TICK_OFFSET_Y1         (PROGRESS_BAR_POS_Y - 8)
#define PROGRESS_BAR_TICK_OFFSET_Y2         (PROGRESS_BAR_POS_Y + 6)
#define PROGRESS_BAR_TICK_W                 10
#define PROGRESS_BAR_TICK_COLOR             0xFFAAAAAA

/*! FF Icon (Used for REW too) */
#define PNG_FILE_FF                         "fastforward.png"

/*! Time in hh:mm:ss format */
#define DISPLAY_TIME_SIZE                   9
#define DISPLAY_FONT_COLOR                  0xFFFFFFFF
#define DISPLAY_CURR_TIME_X                 6
#define DISPLAY_TOTAL_TIME_X                (PROGRESS_BAR_WIDTH + PROGRESS_BAR_POS_X + 4)

/*! Pause Icon */
#define PNG_FILE_PAUSED                    "pause.png"

/*! Trick mode icon position */
#define TRICK_MODE_ICON_X                   320
#define TRICK_MODE_ICON_Y                   370

/*! Trick Mode speed */
#define TRICK_MODE_SPEED_X                  (TRICK_MODE_ICON_X + 40)
#define TRICK_MODE_SPEED_Y                  (TRICK_MODE_ICON_Y + 10)

/*! Opaque Progressbar handle */
typedef struct _ProgressBar *HPB;

/*********************************************************************************************
 *  External APIs
 */

/*!
  Create a progress bar.

  @param phProgressBar (OUT) - Progress bar handle.
  @param hDPROverlay   (IN)  - DPROverlay handle.

  @return PB_OK for success
    PB_FAIL for other error
 */
int ProgressBar_New(
    HPB              *phProgressBar,
    HDPROVERLAY      hDPROverlay);

/*!
  Deletes a progress bar.

  @param hProgressBar (IN) - Progress bar handle.

  @return PB_OK for success
    PB_FAIL for other error
 */
int ProgressBar_Delete(
    HPB               hProgressBar);

/*!
  Displays a progress bar, based on the params passed in

  @param hProgressBar (IN) - Progress bar handle.
  @param pParams      (IN) - Progress bar handle params, defined in struct ProgressBarParams.

  @return PB_OK for success
    PB_FAIL for other error
 */

int ProgressBar_Show(
    HPB               hProgressBar,
    ProgressBarParams *pParams,
    void**            ppBmp,
    void* hDivXFont);

#endif
