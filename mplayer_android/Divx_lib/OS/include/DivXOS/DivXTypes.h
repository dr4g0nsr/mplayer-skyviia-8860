/*!
    @file
@verbatim
$Id: DivXTypes.h 58540 2009-02-20 18:33:41Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXTYPES_H_
#define _DIVXTYPES_H_

/* The following are Primitive defines for basic types like
 * uint32_t ... uint8_t
 */
#include "DivXBool.h"
#include "DivXInt.h"
#include "DivXNULL.h"
#include "DivXStringType.h"

/** This is the DivX error type. */
typedef int DivXError;

/*! Define maximum path lengths
*/
#ifdef _MSC_VER
#include <windows.h> /* for MAX_PATH */
#define DIVX_MAX_PATH MAX_PATH
#elif defined(MACOSX)
#define DIVX_MAX_PATH 256
#elif defined(__GNUC__)
#define DIVX_MAX_PATH 256
#else
#define DIVX_MAX_PATH 256
#endif

/** Define the DivXStringPtr
*/
typedef DivXString* DivXStringPtr;

/*! The following defines the DivX approved inline
 */
#ifdef _MSC_VER
#define DIVX_INLINE __inline
#elif defined(MACOSX)
#define DIVX_INLINE static inline
#elif defined(__GNUC__)
#define DIVX_INLINE static inline
#else
#define DIVX_INLINE
#endif

/*! The following defines a secure sprintf to be used for all DivX
    sprintf type operations.

    Do Not Use sprintf, use DivX_snprintf
 */
#ifdef _MSC_VER
    #ifndef WINCE
	    #if( _MSC_VER >= 1400 )
		    #define DivX_snprintf sprintf_s
	    #else
		    #define DivX_snprintf _snprintf
	    #endif
    #else
        #define DivX_snprintf _snprintf
    #endif
#else
	#define DivX_snprintf snprintf
#endif

/*! The following defines a secure vprintf to be used for all DivX
    vprintf type operations.
 
    Do Not Use vprintf, use DivX_vsnprintf
 */
#ifdef _MSC_VER
    #ifndef WINCE
	    #if( _MSC_VER >= 1400 )
		    #define DivX_vsnprintf vsprintf_s
	    #else
		    #define DivX_vsnprintf _vsnprintf
	    #endif
    #else
        #define DivX_vsnprintf _vsnprintf
    #endif
#else
	#define DivX_vsnprintf vsnprintf
#endif

/*! The following defines a secure itow to be used for all DivX
    itow type operations.
 
    Do Not Use itow, use DivX_itow
 */
#ifdef _MSC_VER
    #ifndef WINCE
	    #if( _MSC_VER >= 1400 )
		    #define DivX_itoa _itoa_s
		    #define DivX_itow _itow_s
	    #else
		    #define DivX_itoa(a,b,c,d) itoa(a,b,d)
		    #define DivX_itow(a,b,c,d) _itow(a,b,d)
	    #endif
    #else
        #define DivX_itoa(a,b,c,d) itoa(a,b,d)
		#define DivX_itow(a,b,c,d) _itow(a,b,d)
    #endif
#elif defined(MACOSX)
	// Added conditionally as part of the Macintosh OS X port. 13 Feburary 2007 GLS
    #define DivX_itoa MacItoa_s
    #define DivX_itow MacItow_s
#elif defined(__GNUC__)
    #define DivX_itoa(a,b,c,d) itoa(a,b,d)
    #define DivX_itow(a,b,c,d) _itow(a,b,d)
#else
	#define DivX_itoa _itoa_s
	#define DivX_itow _itow_s
#endif

/*! The following defines a secure strcpy to be used for all DivX
    strcpy type operations.
 
    Do Not Use strcpy, use DivX_strcpy
 */
#ifdef _MSC_VER
    #ifndef WINCE
	    #if( _MSC_VER >= 1400 )
		    //#define DivX_strcpy(a,b,c) strcpy_s(a,b,c)
        #define DivX_strcpy(a,b,c) strncpy(a,c,b)
	    #else
		    #define DivX_strcpy(a,b,c) strncpy(a,c,b)
	    #endif
    #else
        #define DivX_strcpy(a,b,c) strncpy(a,c,b)
    #endif
#elif defined(MACOSX)
	// Added conditionally as part of the Macintosh OS X port. 13 Feburary 2007 GLS
	#define DivX_strcpy(a,b,c) strncpy(a,c,b)
#elif defined(__GNUC__)
  #define DivX_strcpy(a,b,c) strncpy(a,c,b)
#else
  #define DivX_strcpy(a,b,c) strcpy_s
#endif

/*! The following defines a conversion macro for converting to DivXString* type

    Always use this macro when passing a string literal to a function.
 */
#ifdef _MSC_VER /* prevent next line creating "symbol undefined" warning */
    #if ( _MSC_VER >= 600 )
        #define DIVX_STR(x) L##x
    #endif
#elif defined(MACOSX)
    #define DIVX_STR(x) x
#elif defined(__GNUC__)
    #define DIVX_STR(x) x
#else
    #define DIVX_STR(x) L##x
#endif

/*! The following defines a conversion macro for writing 64 bit integers
 */
#ifdef _MSC_VER
    #define DIVX_I64(x) x##i64
    #define DIVX_UI64(x) x##ui64
#elif defined(MACOSX)
    #define DIVX_I64(x) x##LL
    #define DIVX_UI64(x) x##ULL
#elif defined(__GNUC__)
    #define DIVX_I64(x) x##LL
    #define DIVX_UI64(x) x##ULL
#else
    #define DIVX_I64(x) x##i64
    #define DIVX_UI64(x) x##ui64
#endif

#ifndef __FUNCTION__
#define __FUNCTION__    "__FUNCTION__ MACRO NOT SUPPORTED"
#endif

#endif
