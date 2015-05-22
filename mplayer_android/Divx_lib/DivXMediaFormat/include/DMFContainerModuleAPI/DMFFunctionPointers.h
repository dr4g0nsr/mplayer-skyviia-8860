/*!
    @file
   @verbatim
   $Id: DMFFunctionPointers.h 58004 2009-01-15 19:31:31Z jmurray $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFFUNCTIONPOINTERS_H_
#define _DMFFUNCTIONPOINTERS_H_

#include "DivXInt.h"
#include "DivXBool.h"
#include "DivXString.h"

#include "DivXError.h"
#include "DivXTime.h"
#include "DivXMem.h"

#include "DMFVariant.h"
#include "DMFBlockType.h"
#include "DMFBlockNode.h"
#include "DMFContainerWriteState.h"

/*! @brief Function pointer definition */
typedef void *( *pfnDMFPtr_IntUint )( int32_t, uint32_t );

/*! @brief Function pointer definition */
typedef void *( *pfnDMFPtr_PtrUint )( void *, uint32_t );

/*! @brief Function pointer definition */
typedef void ( *pfnDMFVoid_Void )( void );

/*! @brief Function pointer definition */
typedef void ( *pfnDMFVoid_Ptr )( void * );

/*! @brief Function pointer definition */
typedef void ( *pfnDMFVoid_Voidp )( void * );

/*! @brief Function pointer definition */
/*  typedef void (*pfnDMFVoid_L1ContainerIn)(struct _L1ContainerIn *);  */

/*! @brief Function pointer definition */
typedef void ( *pfnDMFVoid_AllocfnFreefn )( pfnDMFPtr_IntUint, pfnDMFVoid_Ptr,
                                            pfnDMFPtr_PtrUint );

/*! @brief Function pointer definition */
typedef int32_t ( *pfnDMFInt_Void )( void );

/*! @brief Function pointer definition */
typedef int32_t ( *pfnDMFInt_Voidp )( const void * );

/*! @brief Function pointer definition */
typedef int32_t ( *pfnDMFInt_Int )( int );

/*! @brief Function pointer definition */
typedef int32_t ( *pfnDMFInt_StrVararg )( DivXString *, ... );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpVoidpStr )(void*, void*, DivXString*);

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpVoidpVoidpUint )(void*, void*, void*, uint32_t);

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpStrStr )(void*, DivXString*, DivXString* );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpStrVoidpUint )(void*, DivXString*, void*, uint32_t);

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_IntpStr )( int32_t *, const DivXString * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_Voidp )( void * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpHt )( void *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpHtp )( void *, int32_t * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpInt )( void *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntVoidp )( void *, int32_t, void * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntInt32pInt )( void *, int32_t, int32_t *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntInt8pInt )( void *, int32_t, int8_t *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntInt16pInt )( void *, int32_t, int16_t *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntInt64pInt )( void *, int32_t, int64_t *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntDblpInt )( void *, int32_t, double *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntStrIntInt )( void *, int32_t, const DivXString *,
                                                    int32_t, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntStr8IntInt )( void *, int32_t, char *, int32_t, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidppStrMem )( void **, const DivXString *, DivXMem );


/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntVariantpInt )( void *, int32_t, DMFVariant *, int32_t );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntVariantpIntVoidp )( void *, int32_t, DMFVariant *,
                                                           int32_t, void * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpIntVariantpIntVariantp )( void *, int32_t, DMFVariant *,
                                                              int32_t, DMFVariant * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidppStrInfofnVoidpMem )( void **, const DivXString *,
                                                          pfnDMFErr_VoidpIntVariantpIntVoidp,
                                                          void *, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpVariantp )( void *, DMFVariant * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeIntIntpTimep )( void *, DMFBlockType_t, int32_t,
                                                         int32_t *, DivXTime * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeUintMem )( void*, DMFBlockType_t, uint32_t, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpUintUintMem )( void*, uint32_t, uint32_t, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpUintMem )( void*, uint32_t, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeHsIntpTimep )( void *, DMFBlockType_t, int32_t,
                                                        int32_t *, DivXTime * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeIntTimep )( void *, DMFBlockType_t, int32_t,
                                                         DivXTime * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeIntBlockp )( void *, DMFBlockType_t, int32_t,
                                                      DMFBlockNode_t * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeHsBlockp )( void *, DMFBlockType_t, int32_t,
                                                     DMFBlockNode_t * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeVoidpIntp )( void *, DMFBlockType_t, void *,
                                                      int32_t * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypeVoidpHsp )( void *, DMFBlockType_t, void *, int32_t * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypepIntpBlockpVoidp )( void *, DMFBlockType_t *,
                                                             uint32_t *, DMFBlockNode_t *,
                                                             void * );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpBtypepIntBool )( void *, DMFBlockType_t, int32_t, DivXBool );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpMem )( void *, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidppVoidpMem )( void**, void*, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFErr_VoidpVoidppMem )( void*, void**, DivXMem );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFGetInfoErr_IntVariantpIntVoidp )( int32_t QueryValueID,
                                                             DMFVariant *Value, int32_t Index,
                                                             void *privateGetInfoData );

/*! @brief Function pointer definition */
typedef DivXError ( *pfnDMFSetInfoErr_IntVariantpIntVoidp )( int32_t QueryValueID,
                                                             DMFVariant *Value, int32_t Index,
                                                             void *privateSetInfoData );

/*! @brief Function pointer definition */
typedef DMFContainerWriteState ( *pfnDMFCws_Voidp )( void * );

/*! @brief Function pointer definition */
typedef struct _DMFModuleAttributes *( *pfnDMFModAttr_Void )( void );

/*! @brief Function pointer definition */
typedef struct _DMFModuleOptions *( *pfnDMFModOptions_Void )( void );

/*! @brief Function pointer definition */
/*  typedef struct _L1ContainerIn * (*pfnDMFL1ContainerIn_StrInt)(const DivXString*,int32_t);  */

#endif /* _DMFFUNCTIONPOINTERS_H_
          The following line left intentionally blank */
