/*!
    @file
@verbatim
$Id: L1Avi1ReadEntry.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1AVI1READENTRY_H_
#define _L1AVI1READENTRY_H_

#include "DivXInt.h"
#include "DMFModuleInfo.h"
#include "DMFModuleAttributes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
    Initialize for read.

    @param data (IN) - Pointer to a _DMFModuleInitStruct struct containing init settings.

    @return The normal return code is 0 for success.
 */
int32_t L1avir_Init( const void *data );

/*!
    Retrieve modules attributes

    @return DMFModuleAttributes struct containing modules read attributes
*/
DMFModuleAttributes *L1avir_GetCaps( void );

/*! Checks if the container is supported

   @param SupportLevel (OUT) - Pointer to support level of the container
   @param Filename     (IN)  - File name of the container to open

   @return DivXError   (OUT) - See DivXError.h for more details
 */
DivXError L1avir_IsContainerSupported( int32_t          *SupportLevel,
                                       const DivXString *Filename );

DMFModuleInfo *L1avir_GetDMFModuleInfo( int32_t ModuleID );

#ifdef __cplusplus
}
#endif

#endif /* _L1AVI1READENTRY_H_ */
