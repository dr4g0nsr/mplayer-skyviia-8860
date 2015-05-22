/*!

@file
@verbatim
$Header$

Copyright (c) 2008 DivXNetworks, Inc. All rights reserved.

This software is the confidential and proprietary information of DivXNetworks,
Inc. and may be used only in accordance with the terms of your license from
DivXNetworks, Inc.

@endverbatim
 
*/

#ifndef FONTMEMORY_H
#define FONTMEMORY_H

#include "DivXInt.h"
#include "DivXError.h"
#include "DivXMem.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*! This allocates memory using DivX memory manager */
void* FtMemoryAlloc(FT_Memory memory, int32_t size);

/*! This frees memory using DivX memory manager */
void FtMemoryFree(FT_Memory memory, void* block);

/*! This reallocate memory using DivX memory manager */
void* FtMemoryRealloc(FT_Memory memory, int32_t cur_size, int32_t new_size, void* block);

#ifdef __cplusplus
}
#endif

#endif