/*!
   @file L1DF3WriteEntry.h
   @verbatim
   $Id: L1DF3WriteEntry.h 58500 2009-02-18 19:45:46Z jbraness $

   Copyright (c) 2006-2008 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _L1DF3WRITEENTRY_H_
#define _L1DF3WRITEENTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "L1ContainerHandle.h"
#include "DMFModuleInfo.h"

/*! Initializes the DF3 module

   @param data     (IN)  - Pointer to DMFModuleInitStruct containing initialization values
   @return int32_t (OUT) - Returns 0 for success 1 for failure
 */
int32_t L1DF3w_Init( const void *data );

/*! Retrieves module info for given Module

    @param ModuleID (IN) - Unique module ID
    @return pointer to DMFModuleInfo associated with ModuleID
    @retval NULL if Module could be found
*/
DMFModuleInfo *L1DF3w_GetDMFModuleInfo( int32_t ModuleID );


/*! Retrieves the DMFModuleAttributes from module

   @return DMFModuleAttributes (OUT) - Returns the DMFModuleAttributes struct
                                       describing modules capabilities
 */
DMFModuleAttributes *L1DF3w_GetCaps( void );

/*! Checks if the container is supported

   @param SupportLevel (OUT) - Pointer to support level of the container
   @param Filename     (IN)  - File name of the container to open

   @return DivXError   (OUT) - See DivXError.h for more details
 */
DivXError L1DF3w_IsContainerSupported( int32_t    *SupportLevel,
                                        const DivXString *Filename );

#ifdef __cplusplus
}
#endif
#endif /* _L1DF3WRITEENTRY_H_ */
