// $Id: CAutoMutex.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CAUTOMUTEX_H
#define CAUTOMUTEX_H

#include "CDivXMutex.h"

/*! @brief Automatic mutex manager
 * 
 * Automatic helper object that manages the locking and unlocking of a mutex
 * when the heap allocated instance goes out of scope. In the following example,
 * the mutex will lock at "// A" and unlock at "// B" :
 *
 * <pre>
 * ...
 * {
 *     AutoMutex lock(&mutex); // A
 *     ...
 * } // B
 * ...
 * </pre>
 *
 */

/*lint -esym( 1712, AutoMutex ) don't care about default constructor for
  class AutoMutex */

namespace DivXOS
{
	class CAutoMutex
	{
	public:

        /*! Constructor, requires an initialized mutex instance */
        CAutoMutex(CDivXMutex *pMutex, bool enable = true, int32_t timeout = -1) : pMutex(pMutex), done(!enable) 
        { 
            /*! optionally, you can disable use of this mutex */
            if(!enable) { return; } 

            if(pMutex != NULL) 
            {
                pMutex->SetTimeout(timeout);
                pMutex->Wait(); 
            }
        }

        /*! Deconstructor */
        /*lint -e{1551} not worried by exceptions thrown by "Release()"*/
        virtual ~CAutoMutex() { Release(); pMutex = 0; }

        /*! Prevents double release */
        void EarlyRelease() { Release(); }

    private:

        /*! Release Mutex */
        void Release() 
        { 
            if(done) { return; }

            if(pMutex != NULL) { pMutex->Release(); }

            done = true;
        }

        /*! local cache of mutex instance */
        CDivXMutex *pMutex;

        /*! have we released? */
        bool done;
	};
}

#endif // CAUTOMUTEX_H
