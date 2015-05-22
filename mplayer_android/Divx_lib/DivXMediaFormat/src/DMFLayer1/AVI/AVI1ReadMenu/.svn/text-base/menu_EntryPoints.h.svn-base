/*!
    @file
@verbatim
$Id: menu_EntryPoints.h 57969 2009-01-14 01:23:14Z fchan $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

/**
   menu_EntryPoints.h - Entry points for Layer 1 API for menu module
 **/

#ifndef _MENU_ENTRYPOINTS_H_
#define _MENU_ENTRYPOINTS_H_

#include "DivXInt.h"
#include "DivXString.h"
#include "DMFVariant.h"
#include "DMFModuleInfo.h"
#include "DMFModuleAttributes.h"
#include "./AVI/AVICommon/A1MPInstanceHandle.h"
#include "AVI/AVI1Read/AVI1ContainerHandle.h"

int menu_Init( const void *data );

DMFModuleAttributes *menu_GetCaps( void );

DivXError menu_IsContainerSupported( int        *SupportLevel,
                                     const DivXString *Filename );

/* / Container open, close, select title / menu */

/*
   DivXError menu_OpenContainer(DMFContainerHandle *ContainerHandle, DivXString *filename);
   DivXError menu_CloseContainer(DMFContainerHandle ContainerHandle);
   DivXError menu_CreateContainer(DMFContainerHandle *ContainerHandle, DivXString *filename, pfnDMFErr_HCIntVariantpIntVoidp InfoCallback, void *CallbackData );
 */
/* / Query functions */
DivXError menu_GetInfoInt8( A1MPInstanceHandle ContainerHandle,
                            int                QueryValueID,
                            int8_t            *Int8Value,
                            int                Index );

DivXError menu_GetInfoInt16( A1MPInstanceHandle ContainerHandle,
                             int                QueryValueID,
                             int16_t           *Int16Value,
                             int                Index );

DivXError menu_GetInfoInt32( A1MPInstanceHandle ContainerHandle,
                             int                QueryValueID,
                             int32_t           *Int32Value,
                             int                Index );

DivXError menu_GetInfoInt64( A1MPInstanceHandle ContainerHandle,
                             int                QueryValueID,
                             int64_t           *Int64Value,
                             int                Index );

DivXError menu_GetInfoFloat( A1MPInstanceHandle ContainerHandle,
                             int                QueryValueID,
                             double            *DoubleValue,
                             int                Index );

DivXError menu_GetInfoString( A1MPInstanceHandle ContainerHandle,
                              int                QueryValueID,
                              DivXString        *StringValue,
                              int                MaxLength,
                              int                Index );

DivXError menu_GetInfoString8( A1MPInstanceHandle ContainerHandle,
                               int                QueryValueID,
                               char              *String8Value,
                               int                MaxLength,
                               int                Index );

DivXError menu_GetInfo( AVI1ContainerHandle ContainerHandle,
                        int                QueryValueID,
                        DMFVariant        *Value,
                        int                Index );

DivXError menu_SetInfo( AVI1ContainerHandle ContainerHandle,
                        int                 QueryValueID,
                        DMFVariant         *InputValue,
                        int                 Index,
                        DMFVariant         *OutputValue );

/* Global functions which are not exported via function pointers */
void *menu_Alloc( int          HeapID,
                  unsigned int size );

void menu_Free( void *ptr );

/**
   Public entry points used by avi1 and other (future) modules which handle menus
   Note that the menu module publishes a list of function entry points
   via function pointers and passes it down at init time.
 **/

/* / Get module info structure for another module. Return NULL if not found. */
DMFModuleInfo *menu_GetDMFModuleInfo( int ModuleID );

#endif /* _MENU_ENTRYPOINTS_H_ */
