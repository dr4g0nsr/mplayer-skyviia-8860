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

/*! CDivXString.h
*/

#ifndef CDIVXSTRING_H
#define CDIVXSTRING_H

#include <DivXError.h>
#include <DivXTypes.h>

namespace DivXOS
{
	class CDivXMem;

	class CDivXString
	{
	public:
		static DivXError SetMemory(CDivXMem *mem);

		static CDivXString *Alloc(uint32_t iSize);

		DivXError Dealloc();

		static uint32_t Length(const CDivXString *s);
		uint32_t Length();

		static DivXError Cat(CDivXString *dest,const CDivXString *src);
		DivXError Cat(const CDivXString *src);

		static DivXError CopyToChar(char* dest,const CDivXString* src);
		DivXError CopyToChar(char* dest);

		static DivXError CopyFromChar(CDivXString* dest,const char* src);
		DivXError CopyFromChar(const char* src);

		static DivXError Copy(CDivXString *dest,const CDivXString *src);
		DivXError Copy(const CDivXString *src);

		static DivXError NCopy(CDivXString* dest,const CDivXString* src,uint32_t n);
		DivXError NCopy(const CDivXString* src,uint32_t n);

		static DivXError Compare(const CDivXString *dest,const CDivXString *src);
		DivXError Compare(const CDivXString *src);

		static DivXError CompareChar(const char *dest,const char *src);

		static DivXError CopyChar(char *dest,const char *src, uint32_t size);

		static CDivXString *CharStringToDivXString(const char *pstr);

		static CDivXString *CloneString(const CDivXString *pSrc);
		CDivXString *CloneString();

		static int ItoChar(int iVal,char *pDest);

		static int ItoDivXString(int iVal,CDivXString *pDest, uint32_t size);
		int ItoDivXString(int iVal, uint32_t size);

		static char *ToChar(CDivXString *sIn);
		char *ToChar();

		static wchar_t *ToWChar(CDivXString *sIn);
		wchar_t *ToWChar();

		static unsigned char *ToUnsignedChar(CDivXString *sIn);
		unsigned char *ToUnsignedChar();

		static int FindLast(const CDivXString *c,CDivXString *str);
		int FindLast(CDivXString *str);

		static DivXError Sprintf(CDivXString* buffer, int32_t sizeOfBuffer, CDivXString* format, ...);
		DivXError Sprintf(int32_t sizeOfBuffer, CDivXString* format, ...);
	private:
		CDivXString();
		~CDivXString();

		DivXString *buffer;
		static CDivXMem *Pool;
	};
}

#endif // CDIVXSTRING_H
