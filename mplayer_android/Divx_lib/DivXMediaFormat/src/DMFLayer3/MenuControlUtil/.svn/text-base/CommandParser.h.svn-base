/*!
    @file
@verbatim
$Id: CommandParser.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _COMMANDPARSER_H_
#define _COMMANDPARSER_H_

#include "DivXInt.h"

typedef enum MenuControlCommandType
{
    MCC_UNKNOWN,
    MCC_UP,
    MCC_DOWN,
    MCC_LEFT,
    MCC_RIGHT,
    MCC_SELECT,
    MCC_ROOT,
    MCC_FINISHED
}MenuControlCommand;

#ifdef __cplusplus
extern "C" {
#endif

MenuControlCommand ParseCommand( const char *commandFile, int32_t nCommand );

#ifdef __cplusplus
}
#endif

#endif //_COMMANDPARSER_H_
