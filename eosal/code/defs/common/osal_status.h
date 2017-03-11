/**

  @file    defs/common/osal_status.h
  @brief   OSAL function return codes.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Many OSAL functions return status codes, which are enumerated here.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_STATUS_INCLUDED
#define OSAL_STATUS_INCLUDED


/**
****************************************************************************************************

  @name Function return codes
  @anchor osalStatus

  Many OSAL function returns status code. Zero is always success and other values identify
  an error or an exception condition.

****************************************************************************************************
*/
/*@{*/

/** Status codes
 */
typedef enum
{
    /** Success.
     */
    OSAL_SUCCESS = 0,

    /** General failed.
     */
    OSAL_STATUS_FAILED,

    /** Creating thread failed.
     */
    OSAL_STATUS_THREAD_CREATE_FAILED,

    /** Setting thread priority failed.
     */
    OSAL_STATUS_THREAD_SET_PRIORITY_FAILED,

    /** Creating an event failed.
     */
    OSAL_STATUS_EVENT_CREATE_EVENT_FAILED,

    /** Timeout in osal_event_wait() function. If event doesn't get signaled before timeout
        interval given as argument, this code is returned.
     */
    OSAL_STATUS_EVENT_TIMEOUT,

    /** General failure code for osal_event.c. This indicates a programming error.
     */
    OSAL_STATUS_EVENT_FAILED,

    /** Mutex already locked, code returned by osal_mutex_try_lock().
     */
    /* OSAL_STATUS_MUTEX_ALREADY_LOCKED, */

    /** Memory allocation from operating system has failed.
     */
    OSAL_STATUS_MEMORY_ALLOCATION_FAILED,

    /** Setting computer's clock failed.
     */
    OSAL_STATUS_CLOCK_SET_FAILED,

	/** Call would block. The stream functions osal_stream_read_value() and 
	    osal_stream_write_value() return this code to indicate that no data was received
		or sent, because it would otherwise block the calling thread. 
	 */
	OSAL_STATUS_STREAM_WOULD_BLOCK,

	/** No new incoming connection. The stream function osal_stream_accept() return this 
	    code to indicate that no new connection was accepted. 
	 */
    OSAL_STATUS_NO_NEW_CONNECTION,

    /** Socket has been closed.
     */
    OSAL_STATUS_SOCKET_CLOSED,

    /** Process does not have access right to object.
     */
    OSAL_STATUS_NO_ACCESS_RIGHT,

    /** Device is out of free space.
     */
    OSAL_STATUS_DISC_FULL,

    /** Attempt to open file which doesn't exist.
     */
    OSAL_FILE_DOES_NOT_EXIST
}
osalStatus;

/*@}*/



#endif
