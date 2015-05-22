/*!
    @file
   @verbatim
   $Id: DMFModuleInitStruct.h 54494 2008-07-22 22:19:00Z jmurray $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFMODULEINITSTRUCT_H_
#define _DMFMODULEINITSTRUCT_H_

#include "DivXInt.h"
#include "DMFModuleInfo.h"
#include "DMFMetaModuleInfo.h"

/*!
    L1_Init structure is cast as a void * and passed to L1 Init()
    to accommodate access to L2 entry point tables, memory handles,
    and other data shared across all modules. The first member is
    a bitmap containing presence flags for all members, which avoids
    linkage dependency problems when updates occur.
 **/

typedef struct _DMFModuleInitStruct
{
    unsigned int
HasModuleInfo: 1,               /*!< moduleInfo and moduleInfoLength are valid */
HasMemHandle: 1,                /*!< memHandle is valid */
unused: 30;                     /*!< Must be set to 0 */
    DMFModuleInfo **moduleInfo; /*!< Pointer to base of module info* list */
    DMFMetaModuleInfo **metaModuleInfo; /*!< Pointer to base of metadata module info list*/
    int32_t         moduleInfoLength; /*!< Number of entries in module info list */
    void           *memHandle;  /*!< DivXMem handle - FIXME Eric set the type here... */
}
DMFModuleInitStruct;

#endif /* _DMFMODULEINITSTRUCT_H_ */
