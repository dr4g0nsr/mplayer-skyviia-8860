/*!
@file
@verbatim
$Id: CDivXSemaphore.h 50181 2008-03-03 17:01:57Z cdurham $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/
#ifndef _CDIVXSEMAPHORE_H
#define _CDIVXSEMAPHORE_H

#include "DivXTypes.h"
#define CDIVX_SEM_INFINITE  0xFFFFFFFF 

namespace DivXOS
{
    class CDivXSemaphore
    {
    public:
        CDivXSemaphore( uint32_t initCount = 1, 
                uint32_t maxCount = 1 );
        /*!
            @param timeout (IN) time to wait in milliseconds
        */
        void Wait( uint32_t timeout = CDIVX_SEM_INFINITE);
        void Release();

    private:

        /* cached semaphore handle */
        void *pSemaphore;
    };
}

#endif //_CDIVXSEMAPHORE_H
