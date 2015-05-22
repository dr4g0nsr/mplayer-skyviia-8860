/*!
    @file
   @verbatim
   $Id: DMFContainerType.h 57188 2008-11-12 13:01:32Z bbeyeler $

   Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

   This software is the confidential and proprietary information of
   DivX, Inc. and may be used only in accordance with the terms of
   your license from DivX, Inc.
   @endverbatim
 **/

#ifndef _DMFCONTAINERTYPE_H_
#define _DMFCONTAINERTYPE_H_

enum DMFContainerType
{
    AVI1_CONTAINER = 0,
    AVI2_CONTAINER,
    MKV_CONTAINER,
#ifdef MP4_SUPPORT
	MP4_CONTAINER
#endif
};


#endif /* _DMFCONTAINERTYPE_ */
