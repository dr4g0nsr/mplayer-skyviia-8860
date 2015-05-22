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

/*! DivXMemObject.h
*/

#ifndef CDIVXMEM_H
#define CDIVXMEM_H

#include <DivXTypes.h>
#include <DivXError.h>
#include <list>

namespace DivXOS
{
	class CDivXMem
	{
	public:
        
		CDivXMem();

        /*! Cleans up and deinitializes all memory */
		virtual ~CDivXMem();

        /*! Initializes a memory pool. */
		DivXError Init(const uint32_t totalBytes);

        /*! DeInitializes a memory pool */
		DivXError DeInit();

        /*! Does a memory allocation from the pool, if pool is NULL uses the heap */
		void *Alloc(const uint32_t size);

        /*! Does a free from the memory pool, if pool is NULL uses the heap */
		DivXError Free(void *ptr);

		static void *Set(void *ptr, uint8_t val, uint32_t size);

		static void *Copy(void *dest, void *src, uint32_t size);

		void *GetPoolID() {return Pool;}

	private:

        /*! list of memory segments */
        std::list<void*> mem;

        /*! Memory pool for pre-allocated memory pools */
		void *Pool;
	};

	void SetClassMemoryPool(CDivXMem *p);
}

/*void * operator new(size_t size);
void * operator new[] (size_t size);

void operator delete(void *p);
void operator delete[] (void *p);*/
	
#endif // CDIVXMEM_H
