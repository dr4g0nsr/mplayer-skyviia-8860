/*!
    @file
   @verbatim
   $Id: DMFModuleInfo.h 52604 2008-04-23 05:33:29Z jbraness $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMODULEINFO_H_
#define _DMFMODULEINFO_H_

#include "DivXInt.h"
#include "DMFModuleAttributes.h"
#include "DMFModuleEntryPoints.h"

/*! Module info structure containing information and function entry pointers
**/

typedef struct _DMFModuleInfo
{
    int32_t                  ModuleID;  /*!< Unique module ID (constant) */
    DMFModuleAttributes     *Attrib;
    DMFModuleEntryPoints    *Fun;
}
DMFModuleInfo;

#endif /* _DMF_MODULE_INFO_H_ */
