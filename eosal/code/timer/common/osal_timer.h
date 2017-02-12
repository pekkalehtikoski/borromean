/**

  @file    timer/windows/osal_timer.h
  @brief   System timer functions.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file contains functions prototypes for reading system timer and checking if 
  specified time interval has elapsed.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_TIMER_INCLUDED
#define OSAL_TIMER_INCLUDED


/** 
****************************************************************************************************

  @name System Timer Functions

  The os_timer() function gets the system timer as 64 bit integer, this is typically number
  of microseconds since the computer booted up. The os_elapsed() and os_elapsed2() 
  functions check if the specified time interval has elapsed.

****************************************************************************************************
 */
/*@{*/

/* Initialize OSAL timers.
 */
void osal_timer_initialize(
    void);

/* Get system timer, microseconds, typically time elapsed since last boot.
 */
void os_timer(
    os_int64 *start_t);

/* Check if specific time period (milliseconds) has elapsed, gets current timer value by os_timer().
 */
os_boolean os_elapsed(
    os_int64 *start_t,
	os_long period_ms);

/* Check if specific time period (milliseconds) has elapsed, current timer value given as argument.
 */
os_boolean os_elapsed2(
    os_int64 *start_t,
    os_int64 *now_t,
	os_long period_ms);

/*@}*/

#endif
