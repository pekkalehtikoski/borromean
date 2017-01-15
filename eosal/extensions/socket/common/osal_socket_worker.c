/**

  @file    modules/socket/osal_socket_worker.c
  @brief   Thread to run select on multiple sockets.
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
#include "eosal/eosalx.h"


/* Forward referred static functions.
 */
static void osal_socket_worker_thread(
    void *prm,
    osalEvent done);

static void osal_socket_worker_deadlock(
	osalSocketWorkerThreadState *sockworker);

static void osal_socket_worker_exit_thread(
	osalSocketWorkerThreadState *sockworker);

/**
****************************************************************************************************

  @brief Attach socket to worker thread.
  @anchor osal_socket_join_to_worker

  The osal_socket_join_to_worker() function adds a socket to socket list of one of worker threads.
  New worker threads are created as needed.

  @param   s Pointer to socket structure.
  @return  None.

****************************************************************************************************
*/
void osal_socket_join_to_worker(
	osalSocketHeader *s)
{
	osalSocketWorkerThreadState
		*sockworker,
		*w;

	os_boolean
		new_sockworker;

	os_short
		count,
		worker_count;

	/* Synchronize.
	 */
	osal_mutex_system_lock();

	/* Decide wether add this socket to existing socket worker thread or
	   to create new socket worker thread.
	 */
	new_sockworker = OS_TRUE;
	sockworker = OS_NULL;
	count = 10 + 1;
	worker_count = 0;
	for (w = osal_global->sockworker;
		 w;
		 w = w->next)
	{
		if (w->nro_sockets < count)
		{
			count = w->nro_sockets;
			new_sockworker = OS_FALSE;
			sockworker = w;
		}
		worker_count++;
	}

	/* In multi core environments we may want to create at least one worker thread for each core.
	 */
 	if (worker_count < 4) new_sockworker = OS_TRUE;

	/* If new socket worker thread is allocated.
	 */
	if (new_sockworker)
	{
		/* Allocate and clear structure for new socket worker thread.
		 */
		sockworker = osal_memory_allocate(sizeof(osalSocketWorkerThreadState), 
			OS_NULL);
		if (sockworker == OS_NULL)
		{
			osal_mutex_system_unlock();
			return;
		}
		os_memclear(sockworker, sizeof(osalSocketWorkerThreadState));

		sockworker->deadlock = osal_mutex_create();
		sockworker->in_deadlock = osal_event_create();

		/* Allocate operating system dependent resources for the worker thread.
		 */
		osal_socket_worker_ctrl(sockworker, OSAL_SOCKWORKER_SETUP);

		/* Join to list of socket worker threads.
		 */
		sockworker->next = osal_global->sockworker;
		if (osal_global->sockworker) osal_global->sockworker->prev = sockworker;
		osal_global->sockworker = sockworker;
	}

	/* Add socket to list.
	 */
	s->worker_thread = sockworker;
	s->next = sockworker->socket_list;
	if (sockworker->socket_list) sockworker->socket_list->prev = s;
	sockworker->socket_list = s;
	sockworker->nro_sockets++;

	/* End synchronization
	 */
	osal_mutex_system_unlock();

	/* Start socket worker thread, if needed.
	 */
	if (new_sockworker)
	{
		osal_thread_create(osal_socket_worker_thread, sockworker, 
            OSAL_THREAD_DETACHED, 0, "sockwork");
	}
}


/**
****************************************************************************************************

  @brief Detach a socket from worker thread.
  @anchor osal_socket_remove_from_worker

  The osal_socket_remove_from_worker() function detaches a socket from worker thread. After
  this call returns there will be no more callbacks. If this is last socket attached to the 
  the worker thread, worker thread will eventually terminate itself.

  @param   item Pointer to socket list item returned by osal_socket_join_to_worker() function.
		   If this is OS_NULL, then function does nothing.

  @return  None.

****************************************************************************************************
*/
static void osal_socket_remove_from_worker(
	osalSocketHeader *s)
{
	osalSocketWorkerThreadState 
		*sockworker;

	sockworker = s->worker_thread;

	/* Remove the item from list.
	 */
	if (s->next) s->next->prev = s->prev;
	if (s->prev) s->prev->next = s->next;
	else sockworker->socket_list = s->next;
	sockworker->nro_sockets--;

	/* Cleanup.
	 */
	osal_socket_cleanup(s);
}


/**
****************************************************************************************************

  @brief Socket worker thread function.

  The osal_socket_worker_thread() function is called to start the socket worker thread. The
  thread will run repeated osal_socket_select() calls on sockets which are serverd by this
  worker thread. osal_socket_select() will call application's callback functions. The
  worker thread exits when there are no sockets attached to it.

  @param   prm Pointer to parameters for new thread, here pointer to socker worker thread state 
		   structure is parameter for the new thread. Cast it.. This pointer must can be used 
		   only before "done" event is set. This can be OS_NULL if no parameters are needed.
  @param   done Event to set when thread which creted this one may proceed.

  @return  None.

****************************************************************************************************
*/
static void osal_socket_worker_thread(
    void *prm,
    osalEvent done)
{
	osalSocketWorkerThreadState
		*sockworker;

	osalSocketHeader
		*s,
		*next_s;

	/* Pointer to socker worker thread state structure is parameter for the new thread. Cast it.
	 */
	sockworker = (osalSocketWorkerThreadState*)prm;

    /* Let the thread which started this one to proceed.
     */
    osal_event_set(done);

	/* Do forever (termination condition inside).
	 */
	while (1)
	{
		/* If socket functions (like write, read, accept, close) are called by other threads 
		   than the worker thread, the worker thread will be halted into 
		   osal_socket_worker_deadlock() function, until operation is complete.
		 */
		osal_socket_worker_deadlock(sockworker);

		/* Synchronize.
		 */
		osal_mutex_system_lock();

		/* Release socket structures which need to be removed.
		 */
		for (s = sockworker->socket_list;
			 s;
			 s = next_s)
		{
			next_s = s->next;
			if (s->remove) 
			{
				osal_socket_remove_from_worker(s);
			}
		}

		/* If this socket worker thread is unnecessary, release resources and
		   exit the thread.
		 */
		if (sockworker->socket_list == OS_NULL)
		{
			osal_socket_worker_exit_thread(sockworker);
		}

		/* End synchronization.
		 */
		osal_mutex_system_unlock();

		/* Call select(), etc on sockets. The osal_socket_select() will call
		   the callback functions of sockets, as needed.
		 */
		osal_socket_worker_ctrl(sockworker, OSAL_SOCKWORKER_SELECT);
	}
}


/* If socket functions (like write, read, accept, close) are called by other threads 
   than the worker thread, the worker thread will be halted into 
   osal_socket_worker_deadlock() function, until operation is complete.
   Goal is to make sure that socket functions on same socket will never be called
   concurrently by multiple threads.
 */
static void osal_socket_worker_deadlock(
	osalSocketWorkerThreadState *sockworker)
{
	if (osal_mutex_try_lock(sockworker->deadlock))
	{
		osal_event_set(sockworker->in_deadlock);
		osal_mutex_lock(sockworker->deadlock);
		osal_event_wait(sockworker->in_deadlock, 0);
	}
	osal_mutex_unlock(sockworker->deadlock);
}



/* If this socket worker thread is unnecessary, release resources and
   exit the thread.
 */
static void osal_socket_worker_exit_thread(
	osalSocketWorkerThreadState *sockworker)
{
	/* Remove from list.
	 */
	if (sockworker->next) sockworker->next->prev = sockworker->prev;
	if (sockworker->prev) sockworker->prev->next = sockworker->next;
	else osal_global->sockworker = sockworker->next;

	/* End synchronization.
	 */
	osal_mutex_system_unlock();

	/* Release operating system specific resources from socket worker
	   thread.
	 */
	osal_socket_worker_ctrl(sockworker, OSAL_SOCKWORKER_CLEANUP);

	/* Delete mutex and event.
	 */
	osal_mutex_delete(sockworker->deadlock);
	osal_event_delete(sockworker->in_deadlock);

	/* Free memory
	 */
	osal_memory_free(sockworker, sizeof(osalSocketWorkerThreadState));

	/* Exit the thread.
	 */
//	osal_thread_exit();
}
