/*!

@file
@verbatim
$Id:

Copyright (c) 2008-2009 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim

*/

#ifndef _ACTIVETRACKS_H_
#define _ACTIVETRACKS_H_

typedef struct _ActiveTracks_t
{
    int16_t vidTrackId;
    int32_t vidStartBlock;
    int16_t audTrackId;
    int32_t audStartBlock;
    int16_t subTrackId;
    int32_t subStartBlock;
}ActiveTracks_t;

#endif //ACTIVETRACKS_H_