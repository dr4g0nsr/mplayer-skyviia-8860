/*!

@file
@verbatim
$Header$

Copyright (c) 2003-2004 DivXNetworks, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

 */

#ifndef DIVXDBG_H
#define DIVXDBG_H

/*
    Debug Trace API:

    DbgError   := Displayed on trace level <= 3
    DbgWarning := Displayed on trace level <= 2
    DbgVerbose := Displayed on trace level <= 1
    DbgPrintf  := Displayed on all trace levels

    All calls take the same parameters as printf. Define one of the
    following in your project settings or makefile(s):

    DIVX_DEBUG_TRACE   [DbgPrintf]
    DIVX_DEBUG_TRACE=3 [DbgPrintf, DbgError]
    DIVX_DEBUG_TRACE=2 [DbgPrintf, DbgError, DbgWarning]
    DIVX_DEBUG_TRACE=1 [DbgPrintf, DbgError, DbgWarning, DbgVerbose]
*/

/* DbgEmpty compiles away to nothing, for zero overhead */

#define DIVXDBG_COMPILER_C89 (0)
#define DIVXDBG_COMPILER_C99 (1)
#define DIVXDBG_COMPILER_CPP (2)

/*! Figure out the type of compiler, and work out the DbgEmpty macro/function accordingly:

  1. If it's C++ then you can use an empty in-line function
  2. If it's a C99 compatible compiler, then variadic macros reduce the compiler warnings by
     compiling out the argument list to nothing
  3. If it's an older C compiler then the argument lists are left in the code, with accompanying
     compiler warnings

*/
#ifdef __cplusplus

#  define DIVXDBG_COMPILER DIVXDBG_COMPILER_CPP

#elif defined(__GNUC__) /*! __cplusplus */

#  if ( ( __GNUC__ > 2 ) || ( ( __GNUC__ == 2 ) && ( __GNUC_MINOR__ >= 95 ) ) ) && !defined(__SYMBIAN32__)
#    define DIVXDBG_COMPILER DIVXDBG_COMPILER_C99
#  else
#    define DIVXDBG_COMPILER DIVXDBG_COMPILER_C89
#  endif

#elif defined(_MSC_VER) /*! __cplusplus */

#  if ( _MSC_VER >= 1300 )
#    define DIVXDBG_COMPILER DIVXDBG_COMPILER_C99
#  else
#    define DIVXDBG_COMPILER DIVXDBG_COMPILER_C89
#  endif

#else
   /*! Unknown compiler, assume the worst (no variadic support) */
#  define DIVXDBG_COMPILER DIVXDBG_COMPILER_C89
#endif /*! __cplusplus */

/*! Figure out what to use for the empty macros */
#if (DIVXDBG_COMPILER == DIVXDBG_COMPILER_CPP)

inline void null_func(const char *format, ...) { }
#  define DbgEmpty null_func

#elif (DIVXDBG_COMPILER == DIVXDBG_COMPILER_C99)

/*! It's a C99 compiler supporting variadic macros */
#  define DbgEmpty(...)

#else

   /*! Unknown compiler, assume the worst (no variadic support) */
#  define DbgEmpty

#endif /*! __cplusplus */

/* Debug Trace Enabled */
#ifdef  DIVX_DEBUG_TRACE
#  include <stdio.h>
#  define DbgPrintf printf
#else
/* Debug Trace Disabled */
#  define DbgPrintf DbgEmpty
#endif

/*! If trace details are requested then output file and line information */
#ifdef DIVX_DEBUG_TRACE_DETAILS
#  define DIVXDBG_INFO DbgPrintf("File: %s\nLine: %d\n\t", __FILE__, __LINE__);
#else
#  define DIVXDBG_INFO
#endif

/*! If Debug Enabled, Set Level */
#ifdef DIVX_DEBUG_TRACE

/* Debug Trace - Verbose */
#  if DIVX_DEBUG_TRACE <= 1
#    define DbgVerbose DIVXDBG_INFO DbgPrintf("VERBOSE: "); DbgPrintf
#  else
#    define DbgVerbose DbgEmpty
#  endif

/* Debug Trace - Warning */
#  if DIVX_DEBUG_TRACE <= 2
#    define DbgWarning DIVXDBG_INFO DbgPrintf("WARNING: "); DbgPrintf
#  else
#    define DbgWarning DbgEmpty
#  endif

/*! Debug Trace - Error */
#  if DIVX_DEBUG_TRACE <= 3
#    define DbgError DIVXDBG_INFO DbgPrintf("ERROR: "); DbgPrintf
#  else
#    define DbgError DbgEmpty
#  endif

/*! Debug Trace Disabled */
#else

#  define DbgVerbose DbgEmpty
#  define DbgWarning DbgEmpty
#  define DbgError DbgEmpty

#endif

#endif /* DIVXDBG_H */
