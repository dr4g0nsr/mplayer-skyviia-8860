// $Id: CDivXMutex.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CDIVXMUTEX_H
#define CDIVXMUTEX_H

#include <DivXTypes.h>

namespace DivXOS
{
	/*! @brief Simple mutex object */
	class CDivXMutex
	{
	    public:
	
	        CDivXMutex(const DivXString *name = NULL, int32_t timeout = -1);
	        virtual ~CDivXMutex();
	
	        /*! Wait for mutex ownership */
	        DivXError Wait(int32_t timeout = -1);
	
	        /*! Release mutex ownership */
	        DivXError Release();

            /*! Set the timeout, use -1 for infinite wait*/
            void SetTimeout(int32_t timeout);
	
	    private:
	
	        /*! Cached mutex handle */
	        void *pMutex;
	
	        /*! Cached global flag */
	        bool global;

            /*! timeout flag */
            int32_t mutexTimeout;
	};
}

#endif // CDIVXMUTEX_H
