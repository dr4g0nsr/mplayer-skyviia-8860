/*!
    @file
@verbatim
$Id: MKVFastParse.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVFASTPARSE_H_
#define _MKVFASTPARSE_H_

#include "DivXInt.h"
#include "DivXError.h"
#include "DF3/DF3Common/DMFInputStream.h"

/*! Takes an input stream and retrieves the requested information 
    for the title referred to by the index

    @param hInputStream     (IN) - A pointer to an input stream
    @param uiTitle          (IN) - Indicates which title's SegmentUID to get
    @param pUIDBuffer      (OUT) - 16 byte buffer where UID will be stored

    @return DivXError       (OUT)   - Returns errors:
                                        DIVX_ERR_SUCCESS
*/
DivXError mkvFastParse_GetSegmentUID(DMFInputStreamHandle hInputStream, uint32_t uiTitle, unsigned char * pUIDBuffer);


#endif
/* _MKVFASTPARSE_H_ */
