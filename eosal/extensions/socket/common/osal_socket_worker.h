/**

  @file    modules/socket/osal_socket_worker.h
  @brief   Socket worker threads.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Socket worker threads use select one select() function, etc. is to monitor events on multiple 
  sockets simultaneously. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_SOCKET_SELECT_INCLUDED
#define OSAL_SOCKET_SELECT_INCLUDED

struct osalSocketWorkerThreadState;


/**
****************************************************************************************************

  Socket header structure contaings the stream header structure, plus information for
  linking all sockets to worker thread.

****************************************************************************************************
*/
typedef struct osalSocketHeader
{
	/** The stream structure must start with stream header structure. The stream header
	    contains parameters common to every stream. 
	 */
	osalStreamHeader hdr;

	/** Flag to set initiate removing this item from list. This flag is set when socket
	    is closed, so that socket worker thread may do the cleanup.
	 */
	os_boolean remove;

	/** Flag set by write with argument zero. This flag indicates that write callback should
	    be called when select is interrupted.
	 */
	os_boolean send_now;

	/** Next item in socket list. Socket list is two directional linked list which lists
	    all socket belonging to the same worker thread.
	 */
	struct osalSocketHeader *next;

	/** Previous item in socket list. Socket list is two directional linked list which lists
	    all socket belonging to the same worker thread.
	 */
	struct osalSocketHeader *prev;

	/** Pointer to socket worker thread's state structure.
	 */
	struct osalSocketWorkerThreadState *worker_thread;
}
osalSocketHeader;


/**
****************************************************************************************************

  Socket worker thread thread state structure.

  X...

****************************************************************************************************
*/
typedef struct osalSocketWorkerThreadState
{
	/** List of sockets run by this worker thread.
	 */
	struct osalSocketHeader *socket_list;

	/** Number of sockets handled by this worker thread. This is incremented every time
	    a socket is attached to worker thread and decremented when socket is detached
		from worker thread.
	 */
	os_int nro_sockets;

	/** Mutex used to halt socket worker thread.
	 */
	osalMutex deadlock;

	/** Event used to signal caller once the worker thread is halted.
	 */
	osalEvent in_deadlock;

	/** Windows only: Windows event to interrupt select wait on the worker thread.
	 */
#ifdef OSAL_WINDOWS
	void *interrupt_event;
#endif

	/** Next worker thread structure. Worker thread list is two directional linked list.
	 */
	struct osalSocketWorkerThreadState *next;

	/** Previous worker thread structure. Worker thread list is two directional linked list.
	 */
	struct osalSocketWorkerThreadState *prev;
}
osalSocketWorkerThreadState;



typedef enum
{
	OSAL_SOCKWORKER_SELECT,
	OSAL_SOCKWORKER_INTERRUPT,
	OSAL_SOCKWORKER_SETUP,
	OSAL_SOCKWORKER_CLEANUP
}
osalSockWorkerAction;


/**
****************************************************************************************************

  @name Socket Worker Thread Functions.

  The osal_socket_join_to_worker() function attaches a socket to worker thread and 
  osal_socket_remove_from_worker() function detaches a socker from worker thread. Worker
  threads are started and terminated as needed.

****************************************************************************************************
*/
/*@{*/

void osal_socket_join_to_worker(
	osalSocketHeader *s);

void osal_socket_remove_from_worker(
	osalSocketHeader *s);

/*@}*/

#endif
