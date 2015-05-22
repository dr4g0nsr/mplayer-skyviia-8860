/*!
    @file
@verbatim
$Id: libTranslatorInternal.h 52604 2008-04-23 05:33:29Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBTRANSLATORINTERNAL_H_
#define _LIBTRANSLATORINTERNAL_H_

#include "./DMFTranslator/libTranslator.h"

typedef struct _LanguageStringType LanguageString;
typedef struct _LanguageStringType
{
    DivXString       languageCode[2];
    DivXString      *languageString;
    LanguageString  *nextLanguage;
} LanguageStringType;

typedef struct _StringEntryType StringEntry;
typedef struct _StringEntryType
{
    uint32_t         stringID;
    int8_t           stringCount;
    LanguageString  *stringList;
    StringEntry     *nextString;
} StringEntryType;

typedef struct _TranslatorInstanceType TranslatorInstance;
typedef struct _TranslatorInstanceType
{
    StringEntry     *stringTable;
    DivXMem          PoolHandle;
} TranslatorInstanceType;

#endif /* _LIBTRANSLATORINTERNAL_H_ */
