/*!
    @file
@verbatim
$Id: DivXStringType.h 56462 2008-10-13 20:48:09Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXSTRINGTYPE_H_
#define _DIVXSTRINGTYPE_H_

/** Defines the DivXString as wide character type
*/
#ifdef _MSC_VER /* prevent next line creating "symbol undefined" warning */
    #if ( _MSC_VER >= 600 )
        #include "DivXWchar.h"
        typedef wchar_t DivXString;
    #endif
#elif defined(MACOSX)
    typedef char DivXString;
#elif defined(__GNUC__)
    typedef char DivXString;
#else
#error "Undefined compiler type for DivXStringType.h"
#endif

#endif
