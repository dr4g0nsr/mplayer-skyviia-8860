// $Id: CDivXMutexRW.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CDIVXMUTEXRW_H
#define CDIVXMUTEXRW_H

#include <DivXTypes.h>

#include "CDivXMutex.h"
#include "CDivXEvent.h"
#include "CFastList.h"

namespace DivXOS
{
	/*! @brief Mutex object that allows multiple readers and a single writer */
	class CDivXMutexRW
	{
    public:

        CDivXMutexRW(const DivXString *name = NULL, int32_t timeout = -1);

        /*! Wait for writer mutex ownership */
        void WaitW(int32_t timeout = -1);

        /*! Wait for reader mutex ownership */
        void WaitR(int32_t timeout = -1);

        /*! Release writer mutex ownership */
        void ReleaseW();

        /*! Release reader mutex ownership */
        void ReleaseR();

        /*! Switch from reader to writer @note this operation is not atomic! */
        void SwitchRtoW() { ReleaseR(); WaitW(); }
        
        /*! Switch from writer to reader @note this operation is not atomic! */
        void SwitchWtoR() { ReleaseW(); WaitR(); }

        /*! Sets the timeout period, use -1 for infinite */
        void SetTimeout(int32_t timeout);

    private:

        /*! list of current readers */
        CFastList readerList;

        /*! number of current readers */
        int readerCount;

        /*! mutex for writer */
        CDivXMutex mutexW;

        /*! mutex for reader */
        CDivXMutex mutexR;

        /*! event for when readerCount := 0 */
        CDivXEvent noReaderEvent;

        /*! timeout flag */
        int32_t mutexTimeout;
	};
}

#endif // CDIVXMUTEXRW_H
