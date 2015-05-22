// $Id: SSDPConn.h 49321 2008-02-05 23:20:10Z lkirkby $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef BIT64SUPPORT_H
#define BIT64SUPPORT_H

#include "DivXInt.h"
#include "DivXBool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Uint64
{
    uint32_t high;
    uint32_t low;
}
Uint64;

Uint64 Uint64Add32(Uint64 left, uint32_t right);

Uint64 Uint64Add(Uint64 left, Uint64 right);

Uint64 Uint64Subtract32(Uint64 left, uint32_t right);

Uint64 Uint64Subtract(Uint64 left, Uint64 right);

DivXBool Uint64IsEqual(Uint64 left, Uint64 right);

DivXBool Uint64IsGreater(Uint64 left, Uint64 right);

DivXBool Uint64IsGreaterEqual(Uint64 left, Uint64 right);

DivXBool Uint64IsLess(Uint64 left, Uint64 right);

DivXBool Uint64IsLessEqual(Uint64 left, Uint64 right);

DivXBool Uint64IsZero(Uint64 left);

Uint64 Uint64Zero();
    
#ifdef __cplusplus
}
#endif

#endif

