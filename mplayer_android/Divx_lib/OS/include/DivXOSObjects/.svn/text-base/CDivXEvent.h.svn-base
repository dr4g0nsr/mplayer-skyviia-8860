// $Id: CDivXEvent.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CDIVXEVENT_H
#define CDIVXEVENT_H

#include "DivXTypes.h"

namespace DivXOS
{
	/*! @brief Simple event object */
	class CDivXEvent
	{
	public:

		CDivXEvent(const DivXString *name = NULL, bool manualReset = false, int32_t timeout = -1);
		virtual ~CDivXEvent();

        /*! Wait for event */
        DivXError Wait(int32_t timeout = -1);

        /*! Trigger event */
        void Set();

        /*! Reset event */
        void Reset();

        /*! Set the timeout period */
        void SetTimeout(int32_t timeout){eventTimeout = timeout;};

    private:

        /*! Cached event handle */
        void *pEvent;

        /*! Cached global flag */
        bool global;

        /*! Timeout flag */
        int32_t eventTimeout;

	};
}

#endif // CDIVXEVENT_H
