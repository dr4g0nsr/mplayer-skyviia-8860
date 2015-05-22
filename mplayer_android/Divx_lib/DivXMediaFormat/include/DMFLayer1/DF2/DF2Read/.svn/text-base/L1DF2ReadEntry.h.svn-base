/*!
    @file
@verbatim
$Id: L1DF2ReadEntry.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _L1DF2READENTRY_H_
#define _L1DF2READENTRY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DivXMem.h"
#include "DivXString.h"
#include "./DF2/DF2Read/L1DF2Read.h"

/*!
    Initialize for read.

    @param data (IN) - Pointer to a DMFModuleInitStruct struct containing init settings.

    @return The normal return code is 0 for success.
 */
int L1DF2r_Init( const void *data );

/*! Retrieves module info for given Module

    @param ModuleID (IN) - Unique module ID
    @return pointer to DMFModuleInfo associated with ModuleID
    @retval NULL if Module could be found
*/
DMFModuleInfo *L1DF2r_GetDMFModuleInfo( int32_t ModuleID );

/*! Retrieves the DMFModuleAttributes from module

   @return DMFModuleAttributes (OUT) - Returns the DMFModuleAttributes struct
                                       describing modules capabilities
 */
DMFModuleAttributes *L1DF2r_GetCaps( void );

/*! Checks if the container is supported

   @param SupportLevel (OUT) - Pointer to support level of the container
   @param Filename     (IN)  - File name of the container to open

   @return DivXError   (OUT) - See DivXError.h for more details
 */
DivXError L1DF2r_IsContainerSupported( int32_t          *SupportLevel,
                                       const DivXString *Filename );

#ifdef __cplusplus
}
#endif
#endif /* _L1DF2READENTRY_H_ */
