/*! 
	@file 
@verbatim 
$Id: DivXError.h 60192 2009-05-17 21:42:39Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXERROR_H_
#define _DIVXERROR_H_

#include "DivXTypes.h"

#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*!
    Summary:

	Macros
	  To print use DIVX_PRINT() w/ normal printf style arguments
	  To print an error use DIVX_ERR()  ""
	  To print a warning use DIVX_WARN()  ""
	  To print an info msg use DIVX_INFO() ""

    Outputs
	  DIVX_PRINT -> prints exactly like printf if not conditionally compiled out
	  DIVX_ERR -> prepends an "ERR:" postpends a "FILE.cpp:<line number>\n" and assumes a return character
	  DIVX_WARN -> same as DIVX_ERR, but can be conditionally compiled out by #undef 
*/

/*! Helper macro to check to see if success.
    If any warning or errors will return false. */
#define DIVX_IS_SUCCESS( errorCode ) ((DivXError)(errorCode) == 0)

/*! Helper macro to check to see if an error is fatal 
    (does not include warnings) */
#define DIVX_IS_FATAL_ERROR( errorCode ) ((DivXError)(errorCode) < 0)

/*! Helper macro to check to see if an error is non fatal 
    (does not include success) */
#define DIVX_IS_NONFATAL_ERROR( errorCode ) ((DivXError)(errorCode) > 0)

/*! Common Success code */
#define DIVX_ERR_SUCCESS          ((DivXError) 0)

/*! Common Non-fatal errors/warnings. */
#define DIVX_WARN_NONFATAL         1  ///< General nonfatal warning
#define DIVX_WARN_INVALID_HANDLE   2  ///< Nonfatal warning for invalid handle

/*! Common Error codes. */
/*! DIVX_ERR_OUT_OF_RANGE is a special error code for returning an invalid range check on error code creation.
    Make sure your error code is not out of range.
*/
#define DIVX_ERR_FAILURE          -1   ///< General undefined error
#define DIVX_ERR_INVALID_ARG      -2   ///< Supplied argument invalid
#define DIVX_ERR_OPEN_FAILURE     -3   ///< Open file or container failed
#define DIVX_ERR_READ_FAILURE     -4   ///< General read failure
#define DIVX_ERR_WRITE_FAILURE    -5   ///< General write failure
#define DIVX_ERR_INVALID_STATE    -6   ///< Invalid state
#define DIVX_ERR_OUT_OF_RANGE     -7   ///< Error code out of range
#define DIVX_ERR_INVALID_REQUEST  -8   ///< Invalid request
#define DIVX_ERR_EOF              -9   ///< End of file reached while reading
#define DIVX_ERR_INSUFFICIENT_MEM -10  ///< Insufficient memory 

/*! Set the following define to 1 to shorten the path that is printed on error or 0 for full path */
#define DIVX_DEBUG_SHORTENED_FILE_PATH_PRINT 1

/*! Set the following define to 1 for warn and info filename, line number information to be printed */
#define DIVX_DEBUG_WARNINFO_EXTRA_PRINT_INFO 1

/* The following macros must be defined for their type of printout to function.
   Set the following defines to allow printing of error, warning, info and prints, respectively
   Note:  If any of the following macros are not defined here they can be defined
          in just the file they are being used in by defining them before the 
		      include of DivxError.h
*/
#define DIVX_DEBUG_PRINT

#ifdef DIVX_DEBUG_ALL_ON
  #define DIVX_DEBUG_ERR
  #define DIVX_DEBUG_WARN
  #define DIVX_DEBUG_INFO
#endif


/*! Defines the maximum length of the error string, this cannot be exceeded w/o crashing the app 
*/
#define MAX_ERROR_STR_LEN 1024

/*! DivXErrorString takes error code and returns a string (if available) associated with that error 
    @param code - Error code to look up

    @return const DivXString* - Error string associated with the error code
    */
const DivXString* DivXErrorString(DivXError code);

/*! DivXErrorPrint takes filename, func and line number as argument for determining exactly where the 
    error occured.  Useful for debugging. 
    @param filename - __FILE__
    @param func - __FUNCTION__
    @param linenum - __LINE__
    @param fmt - printf style formatting string
    @param ... - printf style argument list

    @return void
*/
void DivXErrorPrint(const char* filename, const char *func, int linenum, const char* fmt, ...);

/*! DivXWarnPrint takes filename, func and line number as argument for determining exactly where the 
    warn occured.  Useful for debugging. 
    @param filename - __FILE__
    @param func - __FUNCTION__
    @param linenum - __LINE__
    @param fmt - printf style formatting string
    @param ... - printf style argument list

    @return void    
    */
void DivXWarnPrint(const char* filename, const char *func, int linenum, const char* fmt, ...);

/*! DivXInfoPrint takes filename, func and line number as argument for determining exactly where the 
    info occured.  Useful for debugging. 
    @param filename - __FILE__
    @param func - __FUNCTION__
    @param linenum - __LINE__
    @param fmt - printf style formatting string
    @param ... - printf style argument list

    @return void    
    */
void DivXInfoPrint(const char* filename, const char *func, int linenum, const char* fmt, ...);

/*! This macro redefines printf and should be used instead of printf in order to conditionally
    compile in/out prints to STDOUT or, alternatively, to redirect the printed output to another
	function.
*/
#ifdef DIVX_DEBUG_PRINT
  #define DIVX_PRINT printf
#else
  #define DIVX_PRINT
#endif

#ifdef DIVX_DEBUG_ERR
/*! This macro defines an ERR printout.  An ERR printout takes the following form:
      ERROR: <string passed to ERR + printf style arguments> __FILE__:__LINE__\n
   
    This data can go directly to STDOUT or be passed into a logger or terminal screen as necessary.
*/
  #define DIVX_ERR(fmt) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt)
  #define DIVX_ERR1(fmt,a) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a)
  #define DIVX_ERR2(fmt,a,b) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b)
  #define DIVX_ERR3(fmt,a,b,c) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c)
  #define DIVX_ERR4(fmt,a,b,c,d) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d)
  #define DIVX_ERR5(fmt,a,b,c,d,e) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e)
  #define DIVX_ERR6(fmt,a,b,c,d,e,f) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f)
  #define DIVX_ERR7(fmt,a,b,c,d,e,f,g) DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f, g)

  #define DIVX_ERR_INST(a) a
#else
  #define DIVX_ERR(fmt) 
  #define DIVX_ERR1(fmt,a) 
  #define DIVX_ERR2(fmt,a,b) 
  #define DIVX_ERR3(fmt,a,b,c) 
  #define DIVX_ERR4(fmt,a,b,c,d) 
  #define DIVX_ERR5(fmt,a,b,c,d,e) 
  #define DIVX_ERR6(fmt,a,b,c,d,e,f) 
  #define DIVX_ERR7(fmt,a,b,c,d,e,f,g) 

  #define DIVX_ERR_INST(a)
#endif

#ifdef DIVX_DEBUG_WARN
/*! This macro defines a WARN printout.  A WARN printout takes the following form:
      WARN: <string passed to WARN + printf style arguments> __FILE__:__LINE__\n
   
    This data can go directly to STDOUT or be passed into a logger or terminal screen as necessary.
*/
  #define DIVX_WARN(fmt) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt)
  #define DIVX_WARN1(fmt,a) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a)
  #define DIVX_WARN2(fmt,a,b) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b)
  #define DIVX_WARN3(fmt,a,b,c) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c)
  #define DIVX_WARN4(fmt,a,b,c,d) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d)
  #define DIVX_WARN5(fmt,a,b,c,d,e) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e)
  #define DIVX_WARN6(fmt,a,b,c,d,e,f) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f)
  #define DIVX_WARN7(fmt,a,b,c,d,e,f,g) DivXWarnPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f, g)
#else
  #define DIVX_WARN(fmt) 
  #define DIVX_WARN1(fmt,a) 
  #define DIVX_WARN2(fmt,a,b) 
  #define DIVX_WARN3(fmt,a,b,c) 
  #define DIVX_WARN4(fmt,a,b,c,d) 
  #define DIVX_WARN5(fmt,a,b,c,d,e) 
  #define DIVX_WARN6(fmt,a,b,c,d,e,f) 
  #define DIVX_WARN7(fmt,a,b,c,d,e,f,g)
#endif

#ifdef DIVX_DEBUG_INFO
/*! This macro defines an INFO printout.  An INFO printout takes the following form:
      INFO: <string passed to INFO + printf style arguments> __FILE__:__LINE__\n
   
    This data can go directly to STDOUT or be passed into a logger or terminal screen as necessary.
*/
  #define DIVX_INFO(fmt) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt)
  #define DIVX_INFO1(fmt,a) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a)
  #define DIVX_INFO2(fmt,a,b) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b)
  #define DIVX_INFO3(fmt,a,b,c) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c)
  #define DIVX_INFO4(fmt,a,b,c,d) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d)
  #define DIVX_INFO5(fmt,a,b,c,d,e) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e)
  #define DIVX_INFO6(fmt,a,b,c,d,e,f) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f)
  #define DIVX_INFO7(fmt,a,b,c,d,e,f,g) DivXInfoPrint(__FILE__,__FUNCTION__,__LINE__, fmt, a, b, c, d, e, f, g)
#else
  #define DIVX_INFO(fmt) 
  #define DIVX_INFO1(fmt,a) 
  #define DIVX_INFO2(fmt,a,b) 
  #define DIVX_INFO3(fmt,a,b,c) 
  #define DIVX_INFO4(fmt,a,b,c,d) 
  #define DIVX_INFO5(fmt,a,b,c,d,e) 
  #define DIVX_INFO6(fmt,a,b,c,d,e,f) 
  #define DIVX_INFO7(fmt,a,b,c,d,e,f,g) 
#endif

#ifdef DEBUG
/*! This macro checks to see if the error code is fatal
    and subsequently returns the error
    printing out error to stderr.
*/
#define DIVX_RETURN_ERR(err) {if((DivXError)err < DIVX_ERR_SUCCESS) { DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, "Returned with error code: %d\n",err); return (DivXError)err;} }
#else
/*! This macro checks to see if the error code is fatal
    and subsequently returns the error.
*/
#define DIVX_RETURN_ERR(err) {if((DivXError)err < DIVX_ERR_SUCCESS) return (DivXError)err;}
#endif

#ifdef DEBUG
/*! This macro checks to see if the error code is fatal
    and subsequently returns the rval
    printing out error to stderr.
*/
#define DIVX_RETURN_ERR_ON_ERR(err,rval) {if((DivXError)err < DIVX_ERR_SUCCESS) { DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, "Returned with error code: %d\n",rval); return (DivXError)rval;} }
#else
/*! This macro checks to see if the error code is fatal
    and subsequently returns the error.
*/
#define DIVX_RETURN_ERR_ON_ERR(err,rval) {if((DivXError)err < DIVX_ERR_SUCCESS) return (DivXError)rval;}
#endif

#ifdef DEBUG
/*! This macro checks to see if the error code is not DIVX_ERR_SUCCESS
    and subsequently returns the error specified by errVal
    printing out error to stderr.
*/
#define DIVX_RETURN_ON_NOTSUCCESS(err) {if((DivXError)err != DIVX_ERR_SUCCESS) { DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, "Returned without success: %d\n",err); return (DivXError)err;} }
#else
/*! This macro checks to see if the error code is not DIVX_ERR_SUCCESS
    and subsequently returns the error specified by errVal.
*/
#define DIVX_RETURN_ON_NOTSUCCESS(err) {if((DivXError)err != DIVX_ERR_SUCCESS) return (DivXError)err;}
#endif

#ifdef DEBUG
/*! This macro checks to see if a pointer is null. It returns 
    the value passed as the rVal if the pointer is null
    printing out error to stderr.
*/
#define DIVX_CHECKPOINTER(ptr,rVal) {if((ptr)==NULL) { DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, "Pointer check failed returned: %d\n",rVal); return (DivXError)rVal;} }
#else
/*! This macro checks to see if a pointer is null. It returns 
    the value passed as the rVal if the pointer is null.
*/
#define DIVX_CHECKPOINTER(ptr,rVal) {if((ptr)==NULL) return (DivXError)(rVal);}
#endif

#ifdef DEBUG
/*! This macro checks to see if a pointer is null following a memory allocation. It returns 
    the value passed as the rVal if the pointer is null
    printing out error to stderr.
*/
#define DIVX_CHECK_MEMALLOC(ptr,rVal) {assert (ptr != NULL); if((ptr)==NULL) { DivXErrorPrint(__FILE__,__FUNCTION__,__LINE__, "Allocation failed returned: %d\n",rVal); return (DivXError)rVal;} }
#else
/*! This macro checks to see if a pointer is null following a memory allocation. It returns 
    the value passed as the rVal if the pointer is null.
*/
#define DIVX_CHECK_MEMALLOC(ptr,rVal) {assert (ptr != NULL); if((ptr)==NULL) return (DivXError)(rVal);}
#endif

/*! This macro checks to see if ret is an warning and set pointer if so. Can be used
    to catch the warnings and returned.
*/
#define DIVX_SET_WARNING(ret,ptr) {if((*ptr == DIVX_ERR_SUCCESS) && (ret >DIVX_ERR_SUCCESS)) *ptr=ret;}

#define DIVX_CHECKCAST(ptr,type,expected) {if( ((type)(ptr))->myType != expected ) return -1;}

/* DMF error code masks */
#define DMF_ERR_MASK_VALUE  0xC0000000
#define DMF_WARN_MASK_VALUE 0x40000000

#ifdef __cplusplus
}
#endif

#endif

