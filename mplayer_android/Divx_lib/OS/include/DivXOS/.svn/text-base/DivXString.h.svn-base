/*!
    @file
@verbatim
$Id: DivXString.h 58615 2009-02-25 18:29:25Z awood $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXSTRING_H_
#define _DIVXSTRING_H_

#include "DivXStringEx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! Sets up a memory handle for the DivXString API, uses malloc as default if this is not set.
    The DivXStringEx.h API should be used for thread safety, this is not a thread safe API, which
    means, yes, you'll have to deal with passing in a DivXMem handle with all of those allocation
    calls.

  @param  hMem       (IN) - DivXMem memory handle

  @return DivXError (OUT) - DivXError (error code)
*/
DivXError DivXStringMemSet(DivXMem hMem);

/*! Converts a wchar ptr into a DivXString*
*/
#define ConvertwcharPtrToDivXStringPtr(wcptr) ((DivXString *)wcptr)

#define FreeConvertedPtr(a) ;

#if defined (__SYMBIAN32__)
int64_t _atoi64( const char *aBuff );
uint64_t _atoui64( const char *aBuff );
#endif

/*! Allocates a string using the memory manager handle

    @note

    @param iSize      (IN)  - Size of string to allocate

    @return void*     (OUT) - Pointer to the allocated memory (NULL == Error)
*/
DivXString *DivXStringAllocate(uint32_t iSize);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringDeallocate(DivXString *pString);

/*! Allocates a string using the memory manager handle

    @note

    @param iSize      (IN)  - Size of string to allocate

    @return void*     (OUT) - Pointer to the allocated memory (NULL == Error)
*/
char *DivXStringCharAllocate(uint32_t iSize);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharDeallocate(char *pString);

/*! Calculates the length of the char string

    @note The string must be null terminated

    @param src        (IN)  - Null terminated const char* to pass

    @return uint32_t  (OUT) - Returns the length of the null terminated string
*/
uint32_t DivXStringCharLen(const char *src);

/*! Trims the spaces on the right

    @note The string must be null terminated

    @param str        (IN)  - Null terminated const char* to pass

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringRTrim(DivXString *str);

/*! Calculates the length of the string

    @note The string must be null terminated

    @param src        (IN)  - Null terminated DivXString* to pass

    @return uint32_t (OUT) - Returns the length of the null terminated string
*/
uint32_t DivXStringLen(const DivXString *src);

/*! Concatenates a DivXString to the end of another DivXString

    @note The string must be null terminated and destination
          string must have enough remaining space to take src

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Destination

    @return DivXError (OUT) - Returns the length of the null terminated string
*/
DivXError DivXStringCat(DivXString *dest, const DivXString *src);

/*! Joins two DivXString's together into a third output string (allocated)

    @note The resulting string is allocated from MemoryPool and must be cleaned up
          Call DivXStringDeallocate() to clean.

    @param dest       (OUT) - Destination string allocated from memory pool (uses Heap by default)
    @param src1       (IN)  - Null terminated DivXString* to pass
    @param src2       (IN)  - Null terminated DivXString* to pass

    @return DivXError (OUT) - Returns the length of the null terminated string
*/
DivXError DivXStringJoin(DivXString **dest, const DivXString *src1, const DivXString *src2);

/*! Copies from wide char to ascii (single char)

    @note

    @param dest       (OUT) - Destination char string
    @param src        (IN)  - Null terminated DivXString* to pass

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCpyDivXStringToChar(char* dest, const DivXString* src);

/*! Copies from ascii (single char) to (DivXString*)

    @note

    @param dest        (OUT) - Destination DivXString
    @param src         (IN)  - Null terminated char string

    @return DivXError  (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCpyCharToDivXString(DivXString* dest, const char* src);

/*! Copies from single byte UTF-8 buffer to DivXString

    @note   This function currently only supports single byte UTF-8 strings. Multi-byte
            characters are represented by underscore characters.

    @param dest        (OUT) - Destination DivXString
    @param src         (IN)  - Null terminated UTF-8 char string

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCpyUTF8ToDivXString(DivXString* dest, const unsigned char* src);

/*! Copies a DivXString to a DivXString

    @note

    @param dest       (OUT) - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCpy(DivXString *dest, const DivXString *src);

/*! Copies a DivXString to a DivXString for n wide char

    @note

    @param dest       (OUT) - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass
    @param n          (IN)  - Number of wide characters to copy

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringNCpy(DivXString* dest, const DivXString* src, uint32_t n);

/*! Compares two DivXStrings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringCmp(const DivXString *dest, const DivXString *src);

/*! Compares two DivXStrings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass
    @param count      (IN)  - Num chars to compare

    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringNCmp(const DivXString *dest,const DivXString *src, int32_t count);

/*! Compares two char strings ignoring case

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringCaseCmp(const DivXString *dest, const DivXString *src);

/*! Compares two char strings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCmpChar(const char *dest, const char *src);

/*! Copy a char strings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass
    @return size     (OUT)  - Number of characters allowed to copy

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCpyChar(char *dest, const char *src, uint32_t size);

/*! Converts a char string to a DivXString

    @note This function allocates memory from the memory manager
          Set to null for allocation from the heap

    @param pstr         (IN)  - Null terminated char* to pass

    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
*/
DivXString *DivXStringCharStringToDivXString(const char *pstr);

/*! Creates a new DivXString from a DivXString

    @note This function allocates memory from the memory manager
          Set to null for allocation from the heap

    @param pstr         (IN)  - Null terminated DivXString* to pass

    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
*/
DivXString *DivXCloneString(const DivXString *pSrc);

/*! Creates a new DivXString from a substring of a DivXString

    @node This function allocates memory from the memory manager
            Set to null for allocation from the heap

    @param pSrc        (IN)  - Null terminated DivXString* to pass

    @param pos       (IN)  - Index of pSrc to begin copy of substring

    @param length         (IN)  - Length of the substring to copy

    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
    */
DivXString *DivXCloneSubstring(const DivXString *pSrc, int32_t pos, int32_t length);

/*! Converts an integer value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - Integer to convert into char*
    @param pDest        (OUT) - Null terminated char*

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringItoChar(int32_t iVal, char *pDest);

/*! Converts an integer value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - Integer to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringItoDivXString(int32_t iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a integer

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringToI(const DivXString *pSrc, int32_t* piDest);

/*! Converts a char into an integer

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharToI(const char *pSrc, int32_t* piDest);

/*! Converts a char into an unsigned integer

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharToUI(const char *pSrc, uint32_t* piDest);

/*! Converts a DivXString into an unsigned integer

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Unsigned integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringToUI(const DivXString *pSrc, uint32_t* piDest);

/*! Converts an int64 value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - int64 to convert into char*
    @param pDest        (OUT) - Null terminated char*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringI64ToChar(int64_t iVal, char *pDest, uint32_t size);

/*! Converts an int64 value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - int64 to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringI64ToDivXString(int64_t iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a int64

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - int64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringToI64(const DivXString *pSrc, int64_t* piDest);

/*! Converts a char into a int64

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - int64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharToI64(const char *pSrc, int64_t* piDest);

/*! Converts a DivXString into a uint64

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - uint64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringToUI64(const DivXString *pSrc, uint64_t* piDest);

/*! Converts a char into a uint64

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - uint64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharToUI64(const char *pSrc, uint64_t* piDest);

/*! Converts an double value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - double to convert into char*
    @param pDest        (OUT) - Null terminated char*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringFToChar(double iVal, char *pDest, uint32_t size);

/*! Converts an double value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - double to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringFToDivXString(double iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a double

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringToF(const DivXString *pSrc, double* piDest);

/*! Converts a char into a double

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringCharToF(const char *pSrc, double* piDest);

/*! Converts a DivString into a char string

    @note

    @param sIn         (IN)  - Null terminated DivXString*

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
char *DivXStringToChar(const DivXString *sIn);

/*! Converts a DivString to an unsigned char*

    @note

    @param sIn              (IN) - Null terminated DivXString*

    @return unsigned char* (OUT) - Returns the unsigned char* or NULL if error
*/
unsigned char *DivXStringToUnsignedChar(const DivXString *sIn);

/*! Finds the first occurence of DivXString sub string within a DivXString

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (OUT) - Null terminated DivXString*

    @return int32_t        (OUT) - Returns the location of the first occurance of c in str
*/
int32_t DivXStringFindFirst(const DivXString *c, const DivXString *str);

/*! Finds the first occurence of DivXString sub string within a DivXString starting at index

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (IN)  - Null terminated DivXString*
    @param index       (IN)  - Index at which to begin search

    @return int32_t        (OUT) - Returns the location of the first occurance of c in str
*/
int32_t DivXStringFindFirstAtIndex(const DivXString *c, const DivXString *str, int32_t index);

/*! Finds the last occurence of DivXString sub string within a DivXString

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (OUT) - Null terminated DivXString*

    @return int32_t        (OUT) - Returns the location of the last occurance of c in str
*/
int32_t DivXStringFindLast(const DivXString *c, const DivXString *str);

/*! Print a formatted string to buffer

    @param buffer     (IN/OUT) - Buffer to print into
    @param uin32_t        (IN) - Buffer size
    @param format         (IN) - Format buffer
    @param ellipsis       (IN) - input arguments printf style

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringSprintf(DivXString* buffer, uint32_t sizeOfBuffer, const DivXString* format, ...);

/*! Returns a new string resulting from replacing all occurrences of oldChar in this "str" with newChar

    @param str  (IN)  - str to base new string on
    @param dest (OUT) - resulting string
    @param oldChar (IN) - characters of "oldchar" will be replaced with "newChar"
    @param newChar (IN) - character to replace "oldChar" with

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringReplace(const DivXString *str, DivXString *dest, DivXString oldChar, DivXString newChar);

/*! Normalizes a string to the correct host type

    @param in  (IN) -  input path changed to correct type for host
    @param out (IN) -  output path

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringConvertPathToHost( const DivXString *in, DivXString *out );

/*! Spans the compliment of a DivXString

    @param str (IN) -  input string to span
    @parame c  (IN) -  the characters to span for

    @return int32_t (OUT) - The number of characters spanned */
int32_t DivXStringSpanCompliment(const DivXString *str, const DivXString *c);

/*! Converts a Unicode (ISO-10646) codepoint to a utf16 character

    @param isoChar (IN) - the codepoint to convert
    @param word (OUT) - the word(s) composing the UTF-16 representation of "isoChar"

    @return uint32)t (OUT) - The number of words (16 bits) needed to represent "isoChar" in UTF-16
*/
uint32_t ISO10646ToUTF16(uint32_t isoChar, uint16_t* word);

/*! Converts a UTF16 character to a Unicode (ISO-10646) codepoint

    @param str      (IN) - the UTF-16 string containting the character at "str[index]" to convert
    @param index    (IN) - the index into "str" where UTF-16 character to convert starts
    @param retVal   (OUT) - pointer to the resulting codepoint

    @return uint32_t (OUT) - Then number of words that were needed to represent "*retVal" in UTF-16
*/
uint32_t UTF16StringToISO10646Char(const uint16_t *str, uint32_t index, uint32_t* retVal);

/*! Converts a UTF-8 string to a Unicode (ISO-10646) codepoint buffer

    @param str  (IN) - the UTF-8 string to convert to Unicode codepoints
    @param iso10646 (OUT) - the output buffer for the codepoints
    @param isoBufferSize (IN) - the maximum number of codepoint elements "iso10646" can contain

    @return uint32_t (OUT) - the number of elements written to "iso10646"
*/
uint32_t UTF8StringToISO10646(const uint8_t *str, uint32_t* iso10646, uint32_t isoBufferSize);

/*! Converts a Unicode (IS0-10646) codepoint to a utf8 character 

    @param isoChar (IN) - Unicode codepoint to convert to UTF8 character
    @param octets (OUT) - UTF-8 conversion of "isoChar"

    @return uint32_t (OUT) - the number of octets (bytes) needed to represent "isoChar"
*/
uint32_t ISO10646ToUTF8(uint32_t isoChar, uint8_t* octets);

/*! Converts a UTF-8 character to a Unicode (ISO-10646) codepoint 

    @param str      (IN) - UTF-8 string to convert to Unicode codepoint
    @param index    (IN) - the index into "str" where UTF-16 character to convert starts
    @param retVal   (OUT) - the Unicode codepoint of "str[index]"

    @return uint32_t (OUT) - the number of octets required to represent "*retVal"
*/
uint32_t UTF8StringToISO646Char(const uint8_t *str, uint32_t index, uint32_t* retVal);

/*! Converts a DivXString to a UTF-8 String 

    @param divxString   (IN) - DivXString to convert to UTF-8 String
    @param utf8String   (OUT) - the UTF-8 conversion of "divxString"
    @param utf8BufferSize (IN) - the maximum number of octets (bytes) "utf8String" can contain

    @return uint32_t (OUT) - then number of octets written to "utf8String"
*/
uint32_t DivXStringToUTF8String(DivXString* divxString, uint8_t* utf8String, uint32_t utf8BufferSize);

/*! Converts a UTF-8 string to a DivXString 

    @param utf8String (IN) - UTF-8 string to convert to a DivXString
    @param divxString (OUT) - the DivXString representation of "utf8String"
    @param divxStringSize (IN) - the maximum number of DivXString characters "divxString" can contain

    @return uint32_t (OUT) - the number of DivXString chars written to "divxString"
*/
uint32_t UTF8StringToDivXString(uint8_t* utf8String, DivXString* divxString, uint32_t divxStringSize);

/*! Converts a UTF-16 string to a Unicode (ISO-10646) codepoint buffer 

    @param str (IN) -  UTF-16 string to convert
    @param iso10646 (OUT) - buffer to hold Unicode codepoints

    @return  uint32_t (OUT) - The number of codepoints written to "iso10646"
*/
uint32_t UTF16StringToISO10646(const uint16_t *str, uint32_t* iso10646, uint32_t isoBufferSize);


#ifdef __cplusplus
}
#endif

#endif

