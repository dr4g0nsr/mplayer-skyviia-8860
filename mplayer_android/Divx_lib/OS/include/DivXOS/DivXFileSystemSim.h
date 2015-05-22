/*!
    @file
@verbatim
$Id: DivXFileSystemSim.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXFILESYSTEMSIM_H_
#define _DIVXFILESYSTEMSIM_H_


#include "DivXInt.h"


#ifdef __cplusplus
extern "C"
{
#endif


#define FILESYSSIM_ACTION_CREATE        0x10
#define FILESYSSIM_ACTION_OPEN          0x20
#define FILESYSSIM_ACTION_WRITE         0x30
#define FILESYSSIM_ACTION_READ          0x40
#define FILESYSSIM_ACTION_SEEK          0x50
#define FILESYSSIM_ACTION_GENERIC       0xFF


#define FILESYSSIM_EFFECT_SUCCESS       0x00
#define FILESYSSIM_EFFECT_FAT_ACCESS    0x10
#define FILESYSSIM_EFFECT_FAILURE       0xFF


typedef struct s_FileSysSimConfig
{
    /* times in milliseconds */
    uint64_t    spinUpTime;     /* time before disk is usable if it's currently stationary */
    uint64_t    fatLoadTime;    /* time to access and load the FAT */
    uint64_t    seekMinTime;    /* minimum time on seeks > (readRate/2) */
    uint64_t    sleepThreshold; /* disk becomes stationary after this period of inactivity */

    /* data rates in kilobits per second */
    uint64_t    readRate;
    uint64_t    writeRate;

    /* actual seek delay = seekMinTime + ((pos.new-pos.old)%seekModu) */
    uint64_t    seekModu;
}
FileSysSimConfig;


extern const FileSysSimConfig
    FileSysDVD_1X, FileSysDVD_2X, FileSysDVD_3X,
    FileSysHDDVD_1X, FileSysHDDVD_2X,
    FileSysBD_1X, FileSysBD_2X;


#if defined(_DEBUG) && defined(WIN32)

void    DivXFileSysSimInit( FileSysSimConfig const *pConfigTable );

void    DivXFileSysSimInstrument( int action, int effect, ... );

#else   /* _DEBUG && WIN32 */

#ifdef  WIN32

_inline void    DivXFileSysSimInit( FileSysSimConfig const *pConfigTable )
{
}

_inline void    DivXFileSysSimInstrument( int action, int effect, ... )
{
}

#else   // WIN32

inline void    DivXFileSysSimInit( FileSysSimConfig const *pConfigTable )
{
}

inline void    DivXFileSysSimInstrument( int action, int effect, ... )
{
}

#endif  // WIN32

#endif  /* _DEBUG && WIN32 */


#ifdef __cplusplus
}
#endif

#endif  /* _DivXFILESYSTEMSIM_H_ */
