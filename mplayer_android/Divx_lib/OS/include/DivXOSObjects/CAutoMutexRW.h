    // $Id: CAutoMutexRW.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CAUTOMUTEXRW_H
#define CAUTOMUTEXRW_H

#include "CDivXMutexRW.h"

/*! @brief Automatic mutex manager for multiple readers and a single writer
 * 
 * Automatic helper object that manages the locking and unlocking of a mutex
 * when the heap allocated instance goes out of scope. In the following example,
 * the mutex will lock at "// A" and unlock at "// B" :
 *
 * <pre>
 * ...
 * {
 *     AutoMutexRW lock(&mutex, true); // A
 *     ...
 * } // B
 * ...
 * </pre>
 *
 */

namespace DivXOS
{
	class CAutoMutexRW
	{
    public:

        /*! Constructor, requires an initialized mutex instance */
        CAutoMutexRW(CDivXMutexRW *pMutexRW, bool writer = false, bool enable = true, int32_t timeout = -1) : pMutexRW(pMutexRW), writer(writer), done(!enable)
        {
            /*! optionally, you can disable use of this mutex */
            if(!enable) { return; }

            if(pMutexRW != NULL)
            {
                pMutexRW->SetTimeout(timeout);
                if(writer) { pMutexRW->WaitW(); }
                else { pMutexRW->WaitR(); }
            }
        }

        /*! Deconstructor */
        virtual ~CAutoMutexRW() { Release(); pMutexRW = NULL; }

        /*! Prevents double release */
        void EarlyRelease() { Release(); }

    private:

        /*! Release MutexRW */
        void Release()
        {
            if(done) { return; }

            if(pMutexRW != NULL)
            {
                if(writer) { pMutexRW->ReleaseW(); }
                else { pMutexRW->ReleaseR(); }
            }

            done = true;
        }

        /*! local cache of mutex instance */
        CDivXMutexRW *pMutexRW;

        /*! local cache of read/write configuration */
        bool writer;

        /*! have we released? */
        bool done;
	};
}

#endif
