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

#ifndef PROGBARPARAMS_H
#define PROGBARPARAMS_H

enum TrickMode
{
    TRICKMODE_PAUSED,
    TRICKMODE_FF,
    TRICKMODE_REW,
    TRICKMODE_NONE
};

/*********************************************************************************************
 *  Typedefs
 */

typedef struct _ProgressBarParams
{
    /*! Current media time (in seconds) */
    uint32_t currMediaTime;

    /*! Total duration of media (in seconds) */
    uint32_t totalMediaTime;

    /*! Trickmode (for displaying correct icon) */
    enum     TrickMode trickMode;

    /*! TrickMode speed (for FF/REW) */
    uint32_t trickSpeed;
}
ProgressBarParams;

#endif
