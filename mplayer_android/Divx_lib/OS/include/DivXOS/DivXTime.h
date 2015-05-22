/*!
    @file
@verbatim
$Id: DivXTime.h 58875 2009-03-11 17:54:05Z ashivadas $

Copyright (c) 2006-2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _DIVXTIME_H_
#define _DIVXTIME_H_

#include "DivXTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*!
    DivXTime uses units of 100 nanoseconds or 10^-7 seconds.
**/

typedef int64_t DivXTime;


/** Predefined DivXTime values */
#define DIVX_TIME_HOUR        36000000000LL /** One second in DivXTime units */
#define DIVX_TIME_MINUTE      600000000     /** One second in DivXTime units */
#define DIVX_TIME_SECOND      10000000      /** One second in DivXTime units */
#define DIVX_TIME_MILLISECOND 10000         /** One millisecond in DivXTime units */
#define DIVX_TIME_MICROSECOND 10            /** One microsecond in DivXTime units */

/** Defines used by the DivXTime functions */
#define  MILLISECONDS     1000        /** 10^-3 */
#define  MICROSECONDS     1000000     /** 10^-6 */
#define  NANOSECONDS      1000000000  /** 10^-9 */
#define  DIVX_TIME_UNITS  10000000    /** 10^-7 */

/*! Retrieve the number of milliseconds in absolute time
    from the system

  @return uint64_t  (OUT) - Output time in milliseconds
 */
uint64_t DivXTimeGet(void);

/*! Retrieve the date in YYYY-MM-DD format 
   
  @return DivXError (OUT) - SUCCESS in setting time
                            FAILURE Error occured
 */
DivXError DivXGetYMD(DivXString *pString, uint32_t *pnSize);

/*! Retrieve the time in HH:MM:SS format 
   
  @return DivXError (OUT) - SUCCESS in setting time
                            FAILURE Error occured
 */
DivXError DivXGetHMS(DivXString *pString, uint32_t *pnSize);


/*! Sleep for the number of milliseconds

  @return void  (OUT) - no value returned
 */
void     DivXSleep( uint64_t msTime );

/*! Convert DivXTime to number of milliseconds 
    
  @param divXTime   (IN) - Input time in DivXTime

  @return int64_t  (OUT) - Output time in milliseconds
 */
int64_t DivXTimeToMilliSeconds( DivXTime divxTime);

/*! Convert milliseconds to DivXTime
    
  @param nMilliSeconds   (IN) - Input time in milliseconds

  @return int64_t  (OUT) - Output time in DivXTime
 */
DivXTime MilliSecondsToDivXTime(int64_t nMilliSeconds);

/*! Convert DivXTime to seconds
    
  @param divXTime   (IN) - Input time in DivXTime

  @return int64_t  (OUT) - Output time in seconds
 */
int64_t DivXTimeToSeconds(DivXTime divxTime);

/*! Convert seconds to DivXTime
    
  @param nSeconds   (IN) - Input time in seconds

  @return DivXTime  (OUT) - Output time in seconds
 */
DivXTime SecondsToDivXTime(int64_t nSeconds);

DivXTime MinutesToDivXTime(int64_t nMinutes);

DivXTime HoursToDivXTime(int64_t nHours);

#ifdef __cplusplus
}
#endif

#endif

