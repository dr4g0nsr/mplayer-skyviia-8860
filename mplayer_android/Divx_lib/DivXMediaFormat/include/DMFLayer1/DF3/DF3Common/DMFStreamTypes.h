/*!
    @file
@verbatim
$Id: DMFStreamTypes.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DMFSTREAMTYPES_H_
#define _DMFSTREAMTYPES_H_

typedef enum
{
    StreamType_HardDisk,
    StreamType_OpticalDisk,
    StreamType_Memory,
#ifdef _DMFNETSUPPORT
    StreamType_Network
#endif

} DMFStreamType_e;

#endif /*_DMFSTREAMTYPES_H_*/
