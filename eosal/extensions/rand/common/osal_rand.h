/**

  @file    clock/common/osal_clock.h
  @brief   Get and set system time.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file contains functions prototypes for getting and setting system time (GMT) as 
  long integer.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_CLOCK_INCLUDED
#define OSAL_CLOCK_INCLUDED

/** 
****************************************************************************************************

  @name Clock functions

  The osal_clock_get() function gets the time stamp as 64 bit integer, microseconds since epoc 
  1970 and the osal_clock_set() function sets the system clock. 

****************************************************************************************************
 */
/*@{*/

/* Get system time (GMT).
 */
void osal_clock_get(
    os_int64 *t);

/* Set system time (GMT).
 */
osalStatus osal_clock_set(
    const os_int64 *t);

/*@}*/

#endif
