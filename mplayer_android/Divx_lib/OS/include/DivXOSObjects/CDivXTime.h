// $Id: CDivXTime.h 49106 2008-02-07 18:42:23Z cdurham $
//
// Copyright (c) 2005 DivX, Inc. All rights reserved.
//
// This software is the confidential and proprietary information of DivX,
// Inc. and may be used only in accordance with the terms of your license from
// DivX, Inc.

#ifndef CDIVXTIME_H
#define CDIVXTIME_H

#include "DivXTypes.h"
#include "DivXError.h"

namespace DivXOS
{
	class CDivXTime
	{
	public:
		CDivXTime();
		virtual ~CDivXTime();

		DivXError Start();
		DivXError Stop();
		DivXError Pause();

		int64_t GetTimeNanoSeconds();
		int64_t GetTimeMilliSeconds();
		int64_t GetTimeSeconds();
		
		void SetTimeNanoSeconds(int64_t nano);
		void SetTimeMilliSeconds(int64_t milli);
		void SetTimeSeconds(int64_t sec);

        void AdjustTimeNanoSeconds(int64_t nano);
        void AdjustTimeMilliSeconds(int64_t milli);
        void AdjustTimeSeconds(int64_t sec);

		DivXBool isRunning();

		static int64_t GetMillisFromNano(int64_t nano);
		static int64_t GetNanoFromMillis(int64_t milli);
		static int64_t GetMillisFromSeconds(int64_t sec);
		static int64_t GetSecondsFromMillis(int64_t milli);
		static int64_t GetNanoFromSeconds(int64_t sec);
		static int64_t GetSecondsFromNano(int64_t nano);
	private:
		enum States
		{
			DIVXTIMER_RUNNING,
			DIVXTIMER_STOPPED,
			DIVXTIMER_PAUSED
		};
		States state;
		int32_t timerResolution;
		int64_t startTime;
		int64_t pauseTime;
		int64_t timeOffset;
		int64_t pauseDuration;
	};
}

#endif // CDIVXTIME_H
