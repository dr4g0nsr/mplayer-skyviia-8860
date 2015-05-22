/*!
    @file
@verbatim
$Id: DivXUtility.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXUTILITY_H_
#define _DIVXUTILITY_H_

#include "DivXTypes.h"
#include "DivXMem.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*! Executes the specified command and waits for it to complete
	Variable argument list version, the command to execute is the
	first string in the list (argv[0]), the first parameter is in
	argv[1], the second is argv[2], and so on

	The command to execute (argv[0]) must either contain a full (or
	partial) path or be located in the current directory. The search
	path is not used to locate a command.

    @param argv (IN)          - The command and parameters in a
								null terminated list of strings
    @param handle (IN)        - Handle to a memory block (can be NULL)
    @param pReturnValue (OUT) - The return value of the executed command
    @return DivXError  (OUT)  - Error code (see DivXError.h)
 */
DivXError DivXUtilitySystem(DivXString* argv[], DivXMem handle, int32_t *pReturnValue);


#ifdef __cplusplus
}
#endif

#endif

