/*!
    @file
@verbatim
$Id: DivXStringEx.h 56464 2008-10-13 21:01:13Z jbraness $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXSTRINGEX_H_
#define _DIVXSTRINGEX_H_

#include "DivXMem.h"
#include "DivXInt.h"

#define DIVX_MAX_STRING_LEN 102400
#define DIVX_MAX_FILEPATH_SIZE (DIVX_MAX_STRING_LEN*2+1)

#ifdef __cplusplus
extern "C"
{
#endif

/*! Converts a wchar ptr into a DivXString*
*/
#define ConvertwcharPtrToDivXStringPtr(wcptr) ((DivXString *)wcptr)

#define FreeConvertedPtr(a) ;

/*! Allocates a string using the memory manager handle

    @note

    @param iSize      (IN)  - Size of string to allocate
    @param handle     (IN)  - Handle to the memory block
    @return void*     (OUT) - Pointer to the allocated memory (NULL == Error)
*/
DivXString *DivXStringExAllocate(uint32_t iSize, DivXMem handle);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate
    @param handle     (IN)  - Memory block to deallocate from
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExDeallocate(DivXString *pString, DivXMem handle);

/*! Allocates a string using the memory manager handle

    @note

    @param iSize      (IN)  - Size of string to allocate
    @param handle     (IN)  - Handle to the memory block
    @return void*     (OUT) - Pointer to the allocated memory (NULL == Error)
*/
char *DivXStringExCharAllocate(uint32_t iSize, DivXMem handle);

/*! Deallocates a string using the memory manager handle

    @note

    @param pString    (IN)  - Ptr to string to deallocate
    @param handle     (IN)  - Memory block to deallocate from
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharDeallocate(char *pString, DivXMem handle);

/*! Calculates the length of the char string

    @note The string must be null terminated

    @param src        (IN)  - Null terminated const char* to pass
    @return length    (OUT) - Returns the length of the null terminated string
*/
uint32_t DivXStringExCharLen(const char *src);

/*! Calculates the length of the string

    @note The string must be null terminated

    @param src        (IN)  - Null terminated DivXString* to pass
    @return length    (OUT) - Returns the length of the null terminated string
*/
uint32_t DivXStringExLen(const DivXString *src);

/*! Concatenates a DivXString to the end of another DivXString

    @note The string must be null terminated and destination
          string must have enough remaining space to take src

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Destination
    @return length    (OUT) - Returns the length of the null terminated string
*/
DivXError DivXStringExCat(DivXString *dest, const DivXString *src);

/*! Joins two DivXString's together into a third output string (allocated)

    @note The resulting string is allocated from MemoryPool and must be cleaned up
          Call DivXStringDeallocate() to clean.

    @param src1       (IN)  - Null terminated DivXString* to pass
    @param src2       (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Destination string allocated from memory pool (uses Heap by default)
    @param hMem       (IN)  - Memory handle
    @return length    (OUT) - Returns the length of the null terminated string
*/
DivXError DivXStringExJoin(DivXString **dest, const DivXString *src1, const DivXString *src2, DivXMem hMem);

/*! Copies from wide char to ascii (single char)

    @note

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Destination char string
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCpyDivXStringToChar(char* dest, const DivXString* src);

/*! Copies from ascii (single char) to wide char string (DivXString*)

    @note

    @param dest        (IN)  - Null terminated DivXString* to pass
    @param src         (OUT) - Destination char string
    @return DivXError  (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCpyCharToDivXString(DivXString* dest, const char* src);

/*! Copies from single byte UTF-8 buffer to DivXString

    @note   This function currently only supports single byte UTF-8 strings. Multi-byte
            characters are represented by underscore characters.

    @param dest        (OUT) - Destination DivXString
    @param src         (IN)  - Null terminated UTF-8 char string

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCpyUTF8ToDivXString(DivXString* dest, const unsigned char* src);

/*! Copies a DivXString to a DivXString

    @note

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Null terminated DivXString* to pass
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCpy(DivXString *dest, const DivXString *src);

/*! Copies a DivXString to a DivXString for n wide char

    @note

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (OUT) - Null terminated DivXString* to pass
    @param n          (IN)  - Number of wide characters to copy
    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExNCpy(DivXString* dest, const DivXString* src,uint32_t n);

/*! Compares two DivXStrings

    @note

    @param src        (IN)  - Null terminated DivXString* to pass
    @param dest       (IN)  - Null terminated DivXString* to pass
    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringExCmp(const DivXString *dest, const DivXString *src);

/*! Compares two DivXStrings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass
    @param count      (IN)  - Num chars to compare

    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringExNCmp(const DivXString *dest,const DivXString *src, int32_t count);

/*! Compares two char strings ignoring case

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return int32_t (OUT) - If int32_t == 0 equal. 1 if dest > src, -1 if dst < src
*/
int32_t DivXStringExCaseCmp(const DivXString *dest, const DivXString *src);

/*! Compares two char strings

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCmpChar(const char *dest, const char *src);

/*! Copy a char string

    @note

    @param dest       (IN)  - Null terminated DivXString* to pass
    @param src        (IN)  - Null terminated DivXString* to pass
    @param size       (IN)  - Size of output buffer

    @return DivXError (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCpyChar(char *dest, const char *src, uint32_t size);

/*! Converts a char string to a DivXString

    @note This function allocates memory from the memory manager
          Set to null for allocation from the heap

    @param pstr         (IN)  - Null terminated char* to pass
    @param dest         (OUT) - Null terminated DivXString*
    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
*/
DivXString *DivXStringExCharStringToDivXString(const char *pstr, DivXMem hMem);

/*! Creates a new DivXString from a DivXString

    @note This function allocates memory from the memory manager
          Set to null for allocation from the heap

    @param pSrc         (IN)  - Null terminated DivXString* to pass
    @param hMem         (IN)  - Memory handle

    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
*/
DivXString *DivXStringExClone( const DivXString *pSrc, DivXMem hMem);

/*! Creates a new DivXString from a substring of a DivXString

    @node This function allocates memory from the memory manager
            Set to null for allocation from the heap

    @param pSrc         (IN)  - Null terminated DivXString* to pass

    @param pos          (IN)  - Index of pSrc to begin copy of substring

    @param length       (IN)  - Length of the substring to copy

    @return DivXString* (OUT) - Returns the newly created DivXString or NULL for error
    */
DivXString *DivXStringExCloneSubstring(const DivXString *pSrc, int32_t pos, int32_t length, DivXMem handle);

/*! Converts an integer value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - Integer to convert into char*
    @param pDest        (OUT) - Null terminated char*

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExItoChar(int32_t iVal, char *pDest);

/*! Converts an integer value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - Integer to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExItoDivXString(int32_t iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a integer

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExToI(const DivXString *pSrc, int32_t* piDest);

/*! Converts a DivXString into an unsigned integer

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExToUI(const DivXString *pSrc, uint32_t* piDest);

DivXError DivXHexStringToUI(const DivXString *pSrc, uint64_t* piDest);

/*! Converts a char into a integer

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharToI(const char *pSrc, int32_t* piDest);

/*! Converts a char into an unsigned integer

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharToUI(const char *pSrc, uint32_t* piDest);



DivXError DivXHexStringExToUI(const DivXString *pSrc, uint64_t* piDest, DivXMem hMem);

/*! Converts an int64 value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - int64 to convert into char*
    @param pDest        (OUT) - Null terminated char*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExI64ToChar(int64_t iVal, char *pDest, uint32_t size);

/*! Converts an int64 value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - int64 to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExI64ToDivXString(int64_t iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a int64

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - int64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExToI64(const DivXString *pSrc, int64_t* piDest);

/*! Converts a char into a int64

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - int64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharToI64(const char *pSrc, int64_t* piDest);

/*! Converts a DivXString into a uint64

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - uint64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExToUI64(const DivXString *pSrc, uint64_t* piDest);

/*! Converts a char into a uint64

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - uint64 output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharToUI64(const char *pSrc, uint64_t* piDest);

/*! Converts an double value into a char string

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - double to convert into char*
    @param pDest        (OUT) - Null terminated char*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExFToChar(double iVal, char *pDest, uint32_t size);

/*! Converts an double value into a DivXString

    @note pDest string must have enough allocated characters to fit the string

    @param iVal         (IN)  - double to convert
    @param pDest        (OUT) - Null terminated DivXString*
    @param size         (IN)  - size of buffer to copy into

    @return int32_t     (OUT) - Returns the number of characters written
*/
int32_t DivXStringExFToDivXString(double iVal, DivXString *pDest, uint32_t size);

/*! Converts a DivXString into a double

    @note

    @param pSrc         (IN)  - Null terminated DivXString*
    @param piDest       (OUT) - Integer output

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExToF(const DivXString *pSrc, double* piDest);

/*! Converts a char into a double

    @note

    @param pSrc         (IN)  - Null terminated char*
    @param piDest       (OUT) - Destination value

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExCharToF(const char *pSrc, double* piDest);

/*! Converts a DivString into a char string

    @note

    @param sIn          (IN)  - Null terminated DivXString*
    @param hMem         (IN)  - Memory handle

    @return char*       (OUT) - Returns the char* or NULL if error
*/
char *DivXStringExToChar(const DivXString *sIn, DivXMem hMem);

/*! Converts a DivString to an unsigned char*

    @note

    @param sIn             (IN)  - Null terminated DivXString*
    @param hMem            (IN)  - Memory handle

    @return unsigned char* (OUT) - Returns the unsigned char* or NULL if error
*/
unsigned char *DivXStringExToUnsignedChar(const DivXString *sIn, DivXMem hMem);

/*! Finds the first occurence of DivXString sub string within a DivXString

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (OUT) - Null terminated DivXString*

    @return int32_t        (OUT) - Returns the location of the first occurance of c in str
*/
int32_t DivXStringExFindFirst(const DivXString *c, const DivXString *str);

/*! Finds the first occurence of DivXString sub string within a DivXString starting at index

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (IN)  - Null terminated DivXString*
    @param index       (IN)  - Index at which to begin search

    @return int32_t        (OUT) - Returns the location of the first occurance of c in str
*/
int32_t DivXStringExFindFirstAtIndex(const DivXString *c, const DivXString *str, int32_t index);

/*! Finds the last occurence of DivXString sub string within a DivXString

    @note

    @param c           (IN)  - Null terminated DivXString*
    @param str         (IN)  - Null terminated DivXString*

    @return int32_t        (OUT) - Returns the location of the last occurance of c in str
*/
int32_t DivXStringExFindLast(const DivXString *c, const DivXString *str);


/*! Print a formatted string to buffer

    @param buffer       (OUT) - Buffer of size sizeOfBuffer
    @param sizeOfBuffer (IN)  - Determines the max size of the buffer
    @param format       (IN)  - Sets the formatting for the printed string
    @param ...          (IN)  - Argument list for the formatted string (printf style formatting).

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExSprintf(DivXString* buffer, uint32_t sizeOfBuffer, const DivXString* format, ...);

/*! Returns a new string resulting from replacing all occurrences of oldChar in this "str" with newChar

    @param str     (IN) - str to base new string on
    @param dest   (OUT) - str destination
    @param oldChar (IN) - characters of "oldchar" will be replaced with "newChar"
    @param newChar (IN) - character to replace "oldChar" with
    @param hMem    (IN) - memory handle to mem pool -- used to alloc space for new string

    @return DivXError   (OUT) - Error code (see DivXError.h)
*/
DivXError DivXStringExReplace(const DivXString *str, DivXString *dest, DivXString oldChar, DivXString newChar, DivXMem hMem);


#ifdef __cplusplus
}
#endif

#endif

