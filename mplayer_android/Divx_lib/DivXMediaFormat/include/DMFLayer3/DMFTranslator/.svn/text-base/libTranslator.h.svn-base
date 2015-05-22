/*!
    @file
@verbatim
$Id: libTranslator.h 56354 2008-10-06 01:02:30Z sbramwell $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _LIBTRANSLATOR_H_
#define _LIBTRANSLATOR_H_

#include "DivXInt.h"
#include "DivXMem.h"
#include "DivXError.h"
#include "DivXString.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TranslatorInstanceType *TranslatorHandle;

/*! Initializes String Translator
    @param (IN) memory pool for allocating strings
    @param (OUT) handle to session instance
    @return DivX Error Code */
DivXError dstInit( TranslatorHandle *instance,
                   DivXMem           Pool );

/*! Adds a blank entry to the translation table
    @param (IN) Handle to session instance
    @param (OUT) String Id for looking up this string
    @return DivX Error Code */
DivXError dstCreateEntry( TranslatorHandle instance,
                          uint32_t         stringId );

/*! Deletes an existing string entry
    @param (IN) Handle to session instance
    @param (IN) String entry ID
    @return DivX Error Code */
DivXError dstDeleteEntry( TranslatorHandle instance,
                          uint32_t         stringId );

/*!	Adds a translated string to an existing table entry
    @param (IN) Handle to session instance
    @param (IN) String reference Id
    @param (IN) language code for this string
    @param (IN) translated string
    @return DivX Error Code */
DivXError dstAddString( TranslatorHandle  instance,
                        uint32_t          stringId,
                        const DivXString *language,
                        const DivXString *string );

/*! Retrieves a translated string from the table
    @param (IN) Handle to session instance
    @param (IN) string reference ID
    @param (IN) language code for the string
    @param (OUT) buffer to contain translated string
    @return DivX Error Code */
DivXError dstGetString( TranslatorHandle  instance,
                        uint32_t          stringId,
                        const DivXString *language,
                        DivXString      **string );

/*!
    @param (IN) Handle to session instance
    @param (IN) string reference ID
    @return number of languages */
int8_t dstGetNumLanguages( TranslatorHandle instance,
                           uint32_t         stringId );

/*!
    @param (IN) Handle to session instance
    @param (IN) string reference ID
    @param (OUT) two-char language code
    @return DivX Error code */
DivXError dstGetStringLanguage( TranslatorHandle instance,
                                uint32_t         stringId,
                                int8_t           index,
                                DivXString      *language );

/*! Shuts down the translator
    @param (IN) Handle to session instance
    @return DivX Error Code */
DivXError dstClose( TranslatorHandle instance );

#ifdef __cplusplus
}
#endif

#endif /* _LIBTRANSLATOR_H_ */
