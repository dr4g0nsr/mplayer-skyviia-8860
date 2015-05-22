/*!
    @file
   @verbatim
   $Id: DMFContainerModules.h 57188 2008-11-12 13:01:32Z bbeyeler $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFCONTAINERMODULES_H_
#define _DMFCONTAINERMODULES_H_

#include "DMFModuleInfo.h"
#include "DMFMetaModuleInfo.h"

/*!
    Note that module identifiers are used as array indices for what may be
    a sparsely populated array. We don't want to skip any values and there
    should NEVER be an insertion other than at the end of the list (before
    L1MODULE_COUNT)
 **/
enum L1Modules
{
    L1MODULE_NULL = 0,      /*!< Used to indicate no module */
    L1MODULE_AVI1,          /*!< AVI 1.0 read */
    L1MODULE_SUB,           /*!< Subtitle read */
    L1MODULE_MP3_READ,      /*!< MP3 read */
    L1MODULE_MP3_WRITE,     /*!< MP3 write */
    L1MODULE_AC3_READ,      /*!< AC3 read */
    L1MODULE_AC3_WRITE,     /*!< AC3 write */
    L1MODULE_MENU,          /*!< Low-level menu parser */
    L1MODULE_AVI1_WRITE,    /*!< AVI 1.0 write */
    L1MODULE_MENU_WRITE,    /*!< Low-level menu write */
    L1MODULE_AVI2_READ,     /*!< AVI 2.0 read */
    L1MODULE_AVI2_WRITE,    /*!< AVI 2.0 write */
    L1MODULE_DF3_READ,     /*!< DF3 read */
    L1MODULE_DF3_WRITE,    /*!< DF3 write */
    L1MODULE_SRT_READ,      /*!< SRT read module*/
    L1MODULE_AVC_READ,      /*!< AVC read module*/
#ifdef MP4_SUPPORT
    L1MODULE_MP4_READ,      /*!< MP4 read module*/
#endif
    L1MODULE_COUNT          /*!< Count of defined modules */
};

/*! Global array of pointers to module info entries. May be null if not present.
 */
extern DMFModuleInfo *gDMFModules[L1MODULE_COUNT];
extern DMFMetaModuleInfo *gDMFMetaModules[L1MODULE_COUNT];

/*! Defines the first module location
 */
#define L1MODULE_FIRST  L1MODULE_NULL + 1

/*! Defines the last module location
 */
#define L1MODULE_LAST   L1MODULE_COUNT - 1

#endif /* _DMFCONTAINERMODULES_H_ */
