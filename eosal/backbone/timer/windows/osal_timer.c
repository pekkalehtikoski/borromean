/**

  @file    win32/osal_timer.c
  @brief   System timer functions.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The osal_timer_get() function gets the system timer as 64 bit integer, this is typically number
  of microseconds since the computer booted up. The osal_timer_elapsed() function checks if the
  specified time has elapsed.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/osal.h"
#include <windows.h>


/**
****************************************************************************************************

  @brief Initialize OSAL timers.
  @anchor osal_timer_initialize

  The osal_timer_initialize() function initializes OSAL timer module. This function is called by
  osal_initialize() and should not normally be called by application.

  @return  None.

****************************************************************************************************
*/
void osal_timer_initialize(
    void)
{
	LARGE_INTEGER 
		winfreq;

	/* Query high resolution system timer frequency. If the installed hardware does not 
	   support a high-resolution performance counter, the return value should be zero
	   (but this is not true on all systems). 
	 */

	if (QueryPerformanceFrequency(&winfreq)) /* was  > 1000 */
	{
		osal_int64_set_uint2(&osal_global->sys_timer_param, winfreq.LowPart, winfreq.HighPart);
		osal_int64_divide(&osal_global->sys_timer_param, &osal_int64_1000);
	}
	else
	{
        osal_debug_error("QueryPerformanceFrequency() failed");
	}
}


/**
****************************************************************************************************

  @brief Get system timer.
  @anchor osal_timer_get

  The osal_timer_get() function get current system timer value. System timer counts microseconds,
  typically since computer was booted.

  @param   start_t Pointer to 64 bit integer into which to store current system timer value, us.
  @return  None.

****************************************************************************************************
*/
void osal_timer_get(
    os_int64 *start_t)
{
	LARGE_INTEGER 
		wincounter;

	QueryPerformanceCounter(&wincounter);

#if OSAL_LONG_IS_64_BITS
	*start_t = (1000 * wincounter.QuadPart) / osal_global->sys_timer_param;
#else
	osal_int64_set_uint2(start_t, wincounter.LowPart, wincounter.HighPart);
	osal_int64_multiply(start_t, &osal_int64_1000);
	osal_int64_divide(start_t, &osal_global->sys_timer_param);
#endif
}

	
/**
****************************************************************************************************

  @brief Check if specific time period has elapsed, gets current timer value by osal_timer_get().
  @anchor osal_timer_elapsed

  The osal_timer_elapsed() function checks if time period given as argument has elapsed since
  start time was recorded by osal_timer_get() function.

  @param   start_t Pointer to 64 bit integer which contains start timer value as returned by 
		   the osal_timer_get() function.
  @param   period_ms Period length in milliseconds.
  @return  OS_TRUE (1) if specified time period has elapsed, oe OS_FALSE (0) if not. 

****************************************************************************************************
*/
os_boolean osal_timer_elapsed(
    os_int64 *start_t,
	os_long period_ms)
{
	/* Generic code, should compile on any os.
	 */
	os_int64 
		now_t,
		end_t;

	/* Calculate period end timer value in microseconds.
	 */
	osal_int64_set_long(&end_t, period_ms);
	osal_int64_multiply(&end_t, &osal_int64_1000);
	osal_int64_add(&end_t, start_t);

	/* Get current system timer value.
	 */
	osal_timer_get(&now_t);

	/* If current timer value is past period end, then return OS_TRUE, or
	   OS_FALSE otherwise.
	 */
	return (os_boolean)(osal_int64_compare(&now_t, &end_t) >= 0);
}


/**
****************************************************************************************************

  @brief Check if specific time period has elapsed, current timer value given as argument.
  @anchor osal_timer_elapsed2

  The osal_timer_elapsed2() function checks if time period given as argument has elapsed since
  start time was recorded by osal_timer_get() function.

  @param   start_t Pointer to 64 bit integer which contains start timer value as returned by 
		   the osal_timer_get() function.
  @param   now_t Pointer to 64 bit integer holding current system timer value.
  @param   period_ms Period length in milliseconds.
  @return  OS_TRUE (1) if specified time period has elapsed, oe OS_FALSE (0) if not. 

****************************************************************************************************
*/
os_boolean osal_timer_elapsed2(
    os_int64 *start_t,
    os_int64 *now_t,
	os_long period_ms)
{
	/* Generic code, should compile on any os.
	 */
	os_int64 
		end_t;

	/* Calculate period end timer value in microseconds.
	 */
	osal_int64_set_long(&end_t, period_ms);
	osal_int64_multiply(&end_t, &osal_int64_1000);
	osal_int64_add(&end_t, start_t);

	/* If current timer value is past period end, then return OS_TRUE, or
	   OS_FALSE otherwise.
	 */
	return (os_boolean)(osal_int64_compare(now_t, &end_t) >= 0);
}
