// $Id: CDivXThread.h 50102 2008-02-29 01:31:16Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CDIVXTHREAD_H
#define CDIVXTHREAD_H

#include "DivXMem.h"

namespace DivXOS
{
	/*lint -esym(1536,Thread::*) suppress the warning that we are
	  exposing the low level members */

	/*! generic thread function */
	typedef void (*ThreadFunc)(bool *pStopFlag, void *pContext, DivXMem hMem);
	
    /*! supported thread priority levels */
    enum DIVX_THREAD_PRIORITY
    {
        DIVXTHREAD_BACKGROUND = 0,
        DIVXTHREAD_LOW = 1,
        DIVXTHREAD_NORMAL = 2,
        DIVXTHREAD_HIGH = 3,
        DIVXTHREAD_CRITICAL = 4
    };

	/*! @brief Simple thread object
	 *
	 *  This thread object provides a platform independant way to
	 *  Start/Stop a thread of execution. To use this thread object,
	 *  the provided ThreadFunc must periodically check the stop
	 *  flag to know when to stop executing (return).
	 *
	 */
	
	class CDivXThread
	{
    public:

        CDivXThread();
        CDivXThread(ThreadFunc pfFunc, void *pContext, DivXMem hMem = NULL, DIVX_THREAD_PRIORITY nPriority = DIVXTHREAD_NORMAL);
        virtual ~CDivXThread();

        /*! Start thread */
        void Start();

        /*! Start with speific parameter info */
        void Start(ThreadFunc pFunc, void *hContext);

        /*! Stop thread */
        void Stop(bool async = false);

        /*! Access thread function */
        ThreadFunc GetFunc() { return pfFunc; }

        /*! Access thread context */
        void *GetThreadContext() { return pContext; }

        /*! Access the stop flag */
        bool GetStopFlag() { return bStop; }

        /*! Access the address of stop flag */
        bool *GetStopFlagRef() { return &bStop; }

        /*! Access stop event */
        void *GetStoppedEvent() { return pStoppedEvent; }

        /*! Is this thread stopped? */
        bool IsStopped();

        /*! sets the thread priority to one of 5 enumerated levels */
        bool SetPriority(DIVX_THREAD_PRIORITY nPriority);

        /*! Sleep the current thread for specified number of milliseconds */
        static void Sleep(int ms);

        /*! Retrieve unique thread identifier */
        static int GetCurrentThreadID();

        /*! thread stopped flag */
		bool stopped;

        /*! Memory handle, use NULL for default */
        DivXMem hMem;
    private:

        /*! Cached thread function */
        ThreadFunc pfFunc;

        /*! Cached arbitrary user context */
        void *pContext;

        /*! Cached thread handle */
        void *pHandle;

        /*! used to signal stopping the thread */
        bool bStop;

        /*! event triggered when thread is dead */
        void *pStoppedEvent;

        /*! thread priority */
        DIVX_THREAD_PRIORITY priority;
	};

	/*! Utility macro for 'this' call function forwarding */
	#define THREAD_FUNC_FWD(ClassName, FuncNameFwd, FuncName)           \
	static void FuncNameFwd(bool *pStopFlag, void *pContext, DivXMem hMem)            \
	{                                                                   \
	    ClassName *pThis = (ClassName*)pContext;                        \
	                                                                    \
	    if(pThis == NULL) { return; }                                   \
								                                        \
	    pThis->FuncName(pStopFlag, pContext, pThis->hMem);              \
	    return;                                                         \
	}
}

#endif // CDIVXTHREAD_H
