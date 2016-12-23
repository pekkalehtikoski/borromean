/**

  @file    modules/socket/win32/osal_socket.c
  @brief   OSAL sockets API windows implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Implementation of OSAL sockets for Windows.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/eosalx.h"
#if 0
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>


/** Windows socket library version information.
 */
static WSADATA 
	osal_wsadata;


/**
****************************************************************************************************

  @name Socket data structure.

  The osalStream type is pointer to a stream, like stream handle. It is defined as pointer to 
  dummy structure to provide compiler type checking. This sturcture is never really allocated,
  and OSAL functions cast their own stream pointers to osalStream pointers.


****************************************************************************************************
*/
typedef struct osalSocket
{
	/** The socket structure must start with socket header structure. This includes generic
	    stream header, which contains parameters common to every stream. 
	 */
	osalSocketHeader hdr;

	/** Operating system's socket handle.
	 */
	SOCKET handle;

	/** Stream open flags. Flags which were given to osal_socket_open() function. 
	 */
	os_short open_flags;

	/** Even to be set when new data has been received, can be sent, new connection has been 
		created, accepted or closed socket.
	 */
	WSAEVENT event;

	osalMutex mutex;
} 
osalSocket;


typedef struct 
{
	osalSocket *stream;
	osalMutex mutex;
} 
osalWindowsEventInfo;

/* Forward referred static functions.
 */
static void osal_socket_select(
	osalSocketWorkerThreadState *sockworker);

static void osal_socket_set_callbacks(
	osalSocket *mysocket,
	osalStreamCallbacks *callbacks);

static os_boolean osal_socket_lock(
	osalSocket *mysocket);

static void osal_socket_unlock(
	osalSocket *mysocket,
	os_boolean select_halted);


/**
****************************************************************************************************

  @brief Open a socket.
  @anchor osal_socket_open

  The osal_socket_open() function opens a socket. The socket can be either listening TCP 
  socket, connecting TCP socket. 

  @param   parameters Socket parameters, a list string. "addr=192.168.1.128:35565" sets IP 
		   address and port to connect to, or interface and port to listen for.

  @param   callbacks Callback functions.

  @param   option Not used for sockets, set OS_NULL.

  @param   status Pointer to integer into which to store the function status code. Value
		   OSAL_SUCCESS (0) indicates success and all nonzero values indicate an error.
           See @ref osalStatus "OSAL function return codes" for full list.
		   This parameter can be OS_NULL, if no status code is needed. 

  @param   flags Flags for creating the socket. 
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.

  @return  Stream pointer representing the socket, or OS_NULL if the function failed.

****************************************************************************************************
*/
osalStream osal_socket_open(
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	void *option,
	osalStatus *status,
	os_short flags)
{
	osalSocket 
		*mysocket = OS_NULL;

	os_memsz
		host_sz;

	os_int
		port_nr;

	os_char
		*host;

	osalStatus
		rval;

	unsigned long 
		addr,
		no_blocking;

	SOCKET 
		handle = INVALID_SOCKET;

	struct sockaddr_in 
		sin;

	struct hostent
		*he;

	/* Get host name or numeric IP address and TCP port number from parameters.
	 */
	host = osal_socket_get_host_name_and_port(parameters, &port_nr,
		&host_sz);

	/* If no host name.
	 */
	if (host == OS_NULL)
	{
		host = "127.0.0.1";
	}

    addr = inet_addr(host);
    if (addr == INADDR_NONE) 
	{
/* Set non blocking mode
 */
// no_blocking = 1;
// ioctlsocket (handle, FIONBIO, &no_blocking);

        he = gethostbyname(host);
        if (he == NULL) 
		{
			rval = OSAL_STATUS_FAILED;
			goto getout;
        }
        addr = *((u_long*)he->h_addr_list[0]);
	}


    handle = socket(AF_INET, SOCK_STREAM, 0);
    if (handle == INVALID_SOCKET) 
	{
		rval = OSAL_STATUS_FAILED;
		goto getout;
	}

	os_memclear(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = addr;
    sin.sin_port = htons((unsigned short)port_nr);

	if (flags & OSAL_STREAM_LISTEN)
	{
		if (bind(handle, (struct sockaddr*)&sin, sizeof(sin))) 
		{
			rval = OSAL_STATUS_FAILED;
			goto getout;
		}
	}

	/* Set non blocking mode
	 */
	no_blocking = 1;
	ioctlsocket (handle, FIONBIO, &no_blocking);

	/* Allocate and clear socket structure.
	 */
	mysocket = osal_memory_allocate(sizeof(osalSocket), OS_NULL);
	if (mysocket == OS_NULL) 
	{
		rval = OSAL_STATUS_MEMORY_ALLOCATION_FAILED;
		goto getout;
	}
	os_memclear(mysocket, sizeof(osalSocket));

	/* Save socket handle and open flags.
	 */
	mysocket->handle = handle;
	mysocket->open_flags = flags;

	/* Save interface pointer.
	 */
	mysocket->hdr.hdr.iface = &osal_socket_iface;

	/* Set infinite timeout
	 */
	mysocket->hdr.hdr.write_timeout_ms = mysocket->hdr.hdr.read_timeout_ms = -1;

	/* Create mutex to synchronize socket access and start synchronization.
	 */
	mysocket->mutex = osal_mutex_create();
//	osal_mutex_lock(mysocket->mutex);

	if (callbacks)
	{
		osal_socket_set_callbacks(mysocket, callbacks);
	}

	if (flags & OSAL_STREAM_LISTEN)
	{
		if (listen(handle, 1))
		{
			/* Detach this socket from worker thread. 
			 */
			// osal_socket_remove_from_worker((osalSocketHeader*)mysocket);

			rval = OSAL_STATUS_FAILED;
			goto getout;
		}
	}
	else
	{
		if (connect(handle, (struct sockaddr*)&sin, sizeof(sin)))
		{
			/* Detach this socket from worker thread. 
			 */
			// osal_socket_remove_from_worker((osalSocketHeader*)mysocket);

rval = WSAGetLastError();
if (rval == 10035)
{
	Sleep(1000);
	goto try_again;
}

			rval = OSAL_STATUS_FAILED;
			goto getout;
		}
	}
try_again:

	/* Join the socket worker thread's linked list.
	 */
	if (callbacks)
	{
		osal_socket_join_to_worker((osalSocketHeader*)mysocket);
	}

//	if (mysocket->mutex)
//	{
//		osal_mutex_unlock(mysocket->mutex);
//	}

	/* Release memory allocated for the host name or address.
	 */
	if (host_sz) osal_memory_free(host, host_sz);

	/* Success set status code and cast socket structure pointer to stream pointer and return it.
	 */
	if (status) *status = OSAL_SUCCESS;
	return (osalStream)mysocket;

getout:
	/* Release memory allocated for the host name or address.
	 */
	if (host_sz) osal_memory_free(host, host_sz);

	if (handle != INVALID_SOCKET) 
	{
		closesocket(handle);
	}

	if (mysocket) 
	{
		osal_socket_cleanup((osalSocketHeader*)mysocket);
	}

	/* Memory allocation failed. Set status code and return null pointer.
	 */
	if (status) *status = rval;
	return OS_NULL;
}




/**
****************************************************************************************************

  @brief Close socket.
  @anchor osal_socket_close

  The osal_socket_close() function clsoes a socket, which was creted by osal_socket_open() 
  function. All resource related to the socket are freed. Any attemp to use the socket after
  this call may result crash.

  @param   stream Stream pointer representing the socket. After this call stream pointer will
		   point to invalid memory location.
  @return  None.

****************************************************************************************************
*/
void osal_socket_close(
	osalStream stream)
{
	osalSocket 
		*mysocket;

	SOCKET 
		handle;

    char 
		buf[64];

	int 
		n;

	os_boolean
		select_halted,
		cleanup_now = OS_FALSE;

	/* If called with NULL argument, do nothing.
	 */
	if (stream == OS_NULL) return;

	/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
	 */
	mysocket = (osalSocket*)stream;
	select_halted = osal_socket_lock(mysocket);
	handle = mysocket->handle;

	/* If socket operating system socket is not already closed, close now.
	 */
	if (handle != INVALID_SOCKET)
	{
		/* Mark socket closed
		 */
		mysocket->handle = INVALID_SOCKET;
		mysocket->hdr.remove = OS_TRUE;

		/* Disable sending data. This informs other the end of socket that it is going down now.
		 */
		if (shutdown(handle, SD_SEND)) 
		{
			osal_debug_error("shutdown() failed");
		}

		/* Read data to be received until receive buffer is empty.
		 */
		do
		{
			n = recv(handle, buf, sizeof(buf), 0);
			if (n == SOCKET_ERROR) 
			{
	#if OSAL_DEBUG
				if (WSAGetLastError() != WSAEWOULDBLOCK) 
				{
					osal_debug_error("reading end failed");
				}
	#endif
				break;
			}
		} 
		while(n);

		/* Close the socket.
		 */
		if (closesocket(handle)) 
		{
			osal_debug_error("closesocket failed");
		}

		cleanup_now = (os_boolean)(mysocket->event == OS_NULL);
	}

	/* Unlock the socket
	 */
	osal_socket_unlock(mysocket, select_halted);

	/* If this socket is run by worker thread, just clean up resources now.
	 */
	if (cleanup_now)
	{
		osal_socket_cleanup((osalSocketHeader*)mysocket);
	}
}




/**
****************************************************************************************************

  @brief Accept connection from listening socket.
  @anchor osal_socket_open

  The osal_socket_accept() function accepts an incoming connection from listening socket.

  @param   stream Stream pointer representing the listening socket.

  @param   parameters Socket parameters, a list string. 

  @param   callbacks Callback functions.

  @param   status Pointer to integer into which to store the function status code. Value
		   OSAL_SUCCESS (0) indicates that new connection was successfully accepted.
		   The value OSAL_STATUS_NO_NEW_CONNECTION indicates that no new incoming 
		   connection, was accepted.  All other nonzero values indicate an error,
           See @ref osalStatus "OSAL function return codes" for full list.
		   This parameter can be OS_NULL, if no status code is needed. 

  @param   flags Flags for creating the socket. Define OSAL_STREAM_DEFAULT for normal operation.
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.

  @return  Stream pointer representing the socket, or OS_NULL if the function failed.

****************************************************************************************************
*/
osalStream osal_socket_accept(
	osalStream stream,
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	osalStatus *status,
	os_short flags)
{
	osalSocket 
		*mysocket,
		*newsocket;

	SOCKET 
		handle,
		new_handle;

	int 
		addr_size;

	struct sockaddr_in 
		sin_remote;

	os_boolean
		select_halted;

	if (stream)
	{
	/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
	 */
		mysocket = (osalSocket*)stream;
		select_halted = osal_socket_lock(mysocket);
		handle = mysocket->handle;

		/* If socket operating system socket is not already closed.
		 */
		if (handle != INVALID_SOCKET)
		{
			addr_size = sizeof(sin_remote);
			new_handle = accept(handle, (struct sockaddr*)&sin_remote, &addr_size);
		}
		else
		{
			new_handle = INVALID_SOCKET;
		}

		/* Unlock the listening socket, no longer needed by accept.
		 */
		osal_socket_unlock(mysocket, select_halted);

		/* If no new connection, do nothing more.
		 */
        if (new_handle == INVALID_SOCKET) 
		{
			if (status) *status = OSAL_STATUS_NO_NEW_CONNECTION;
			return OS_NULL;
		}

		/* cout << "Accepted connection from " <<
           inet_ntoa(sinRemote.sin_addr) << ":" <<
           ntohs(sinRemote.sin_port) << "." << endl;
		 */

		/* Allocate and clear socket structure.
		 */
		newsocket = osal_memory_allocate(sizeof(osalSocket), OS_NULL);
		if (newsocket == OS_NULL) 
		{
			closesocket(new_handle);
			if (status) *status = OSAL_STATUS_MEMORY_ALLOCATION_FAILED;
			return OS_NULL;
		}
		os_memclear(newsocket, sizeof(osalSocket));

		/* Save socket handle and open flags.
		 */
		newsocket->handle = new_handle;
		newsocket->open_flags = flags;

		/* Save interface pointer.
		 */
	#if OSAL_FUNCTION_POINTER_SUPPORT
		newsocket->hdr.hdr.iface = &osal_socket_iface;
	#endif

		/* Set timeouts.
		 */
		newsocket->hdr.hdr.write_timeout_ms = newsocket->hdr.hdr.read_timeout_ms = -1;

		/* Create mutex to synchronize socket access and start synchronization.
		 */
		newsocket->mutex = osal_mutex_create();

		if (callbacks)
		{
			osal_socket_set_callbacks(newsocket, callbacks);
			osal_socket_join_to_worker((osalSocketHeader*)newsocket);
		}

		/* Set event. It is at least theoretically possible that at very beginning an event
		   belonging to the accepted socket may go to the listening socket. Creating one time
		   unnecessary event is much better than any possibility of missing an event.
		 */
		if (newsocket->event) WSASetEvent(newsocket->event);

		/* Success set status code and cast socket structure pointer to stream pointer 
		   and return it.
		 */
		if (status) *status = OSAL_SUCCESS;
		return (osalStream)newsocket;
	}

	if (status) *status = OSAL_STATUS_FAILED;
	return OS_NULL;
}





/**
****************************************************************************************************

  @brief Flush the socket.
  @anchor osal_socket_flush

  The osal_socket_flush() function flushes data to be written to stream.

  @param   stream Stream pointer representing the socket.
  @param   flags See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.
  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_socket_flush(
	osalStream stream,
	os_short flags)
{
	return OSAL_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to socket.
  @anchor osal_socket_write

  The osal_socket_write() function writes up to n bytes of data from buffer to socket.

  @param   stream Stream pointer representing the socket.
  @param   buf Pointer to the beginning of data to place into the socket.
  @param   n Maximum number of bytes to write. 
  @param   n_written Pointer to integer into which the function stores the number of bytes 
		   actually written to socket,  which may be less than n if there is not enough space
		   left in the socket. If the function fails n_written is set to zero.
  @param   flags Flags for the function.
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.
  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_socket_write(
	osalStream stream,
	const os_uchar *buf,
	os_memsz n,
	os_memsz *n_written,
	os_short flags)
{
	int 
		rval;

	os_boolean 
		select_halted;

	osalSocket 
		*mysocket;

	SOCKET 
		handle;

	if (stream)
	{
		/* Cast stream pointer to socket structure pointer.
		 */
		mysocket = (osalSocket*)stream;

		/* Special case. Writing 0 bytes will trigger write callback by worker thread.
		 */
		if (n == 0)
		{
			if (mysocket->hdr.hdr.callbacks.write_func)
			{
				mysocket->hdr.send_now = OS_TRUE;
				osal_socket_worker_ctrl(mysocket->hdr.worker_thread, OSAL_SOCKWORKER_INTERRUPT);
			}
			*n_written = 0;
			return OSAL_SUCCESS;
		}

		/* Lock socket and get OS socket handle.
		 */
		select_halted = osal_socket_lock(mysocket);
		handle = mysocket->handle;

		/* If operating system socket is already closed.
		 */
		if (handle == INVALID_SOCKET)
		{
			osal_socket_unlock(mysocket, select_halted);
			goto getout;
		}

		rval = send(handle, buf, n, 0);

		if (rval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) 
			{
				osal_socket_unlock(mysocket, select_halted);
				goto getout;
			}
			rval = 0;
		}

		osal_socket_unlock(mysocket, select_halted);
		*n_written = rval;
		return OSAL_SUCCESS;
	}

getout:
	*n_written = 0;
	return OSAL_STATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Read data from socket.
  @anchor osal_socket_read

  The osal_socket_read() function reads up to n bytes of data from socket into buffer. 

  @param   stream Stream pointer representing the socket.
  @param   buf Pointer to buffer to read into.
  @param   n Maximum number of bytes to read. The data buffer must large enough to hold
		   at least this namy bytes. 
  @param   n_read Pointer to integer into which the function stores the number of bytes read, 
           which may be less than n if there are fewer bytes available. If the function fails 
		   n_read is set to zero.
  @param   flags Flags for the function, use OSAL_STREAM_DEFAULT (0) for default operation. 
		   The OSAL_STREAM_PEEK flag causes the function to return data in socket, but nothing
		   will be removed from the socket.
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.

  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_socket_read(
	osalStream stream,
	os_uchar *buf,
	os_memsz n,
	os_memsz *n_read,
	os_short flags)
{
	int 
		rval;

	os_boolean 
		select_halted;

	osalSocket 
		*mysocket;

	SOCKET 
		handle;

	if (stream)
	{
		/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
		 */
		mysocket = (osalSocket*)stream;
		select_halted = osal_socket_lock(mysocket);
		handle = mysocket->handle;

		/* If operating system socket is already closed.
		 */
		if (handle == INVALID_SOCKET)
		{
			osal_socket_unlock(mysocket, select_halted);
			goto getout;
		}

		rval = recv(handle, buf, n, 0);

		if (rval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) 
			{
				osal_socket_unlock(mysocket, select_halted);
				goto getout;
			}
			rval = 0;
		}

		osal_socket_unlock(mysocket, select_halted);
		*n_read = rval;
		return OSAL_SUCCESS;
	}

getout:
	*n_read = 0;
	return OSAL_STATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Get socket parameter.
  @anchor osal_socket_get_parameter

  The osal_socket_get_parameter() function gets a parameter value.

  @param   stream Stream pointer representing the socket.
  @param   parameter_ix Index of parameter to get.
		   See @ref osalStreamParameterIx "stream parameter enumeration" for the list.
  @return  Parameter value.

****************************************************************************************************
*/
os_long osal_socket_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix)
{
	/* Call the default implementation
	 */
	return osal_stream_default_get_parameter(stream, parameter_ix);
}


/**
****************************************************************************************************

  @brief Set socket parameter.
  @anchor osal_socket_set_parameter

  The osal_socket_set_parameter() function gets a parameter value.

  @param   stream Stream pointer representing the socket.
  @param   parameter_ix Index of parameter to get.
		   See @ref osalStreamParameterIx "stream parameter enumeration" for the list.
  @param   value Parameter value to set.
  @return  None.

****************************************************************************************************
*/
void osal_socket_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value)
{
	/* Call the default implementation
	 */
	osal_stream_default_set_parameter(stream, parameter_ix, value);
}


/**
****************************************************************************************************

  @brief Initialize sockets.
  @anchor osal_socket_initialize

  The osal_socket_initialize() initializes the underlying sockets library.

  @return  None.

****************************************************************************************************
*/
void osal_socket_initialize(
	void)
{
	/* Make sure that OSAL is initailized
	 */
	osal_initialize();

	/* If socket library is already initialized, do nothing.
	 */
	if (osal_global->sockets_initialized) return;

	/* Lock the system mutex to syncronize.
	 */
	osal_mutex_system_lock();

	/* If socket library is already initialized, do nothing. Double checked here
	   for thread synchronization.
	 */
	if (!osal_global->sockets_initialized) 
	{
		/* Initialize winsock.
		 */
		if (WSAStartup(MAKEWORD(2,2), &osal_wsadata))
		{
			osal_debug_error("WSAStartup() failed");
			return;
		}

		/* ADD SOCKET SHUTDOWN TO ATEXIT FUNCTION
		 */
		
		/* Mark that socket library has been initialized.
		 */
		osal_global->sockets_initialized = OS_TRUE;
	}

	/* End synchronization.
	 */
	osal_mutex_system_unlock();
}


/**
****************************************************************************************************

  @brief Shut down sockets.
  @anchor osal_socket_shutdown

  The osal_socket_shutdown() shuts down the underlying sockets library.

  @return  None.

****************************************************************************************************
*/
void osal_socket_shutdown(
	void)
{
	/* If socket library is not initialized, do nothing.
	 */
	if (!osal_global->sockets_initialized) return;

	/* Initialize winsock.
	 */
	if (WSACleanup())
	{
		osal_debug_error("WSACleanup() failed");
		return;
	}

	/* Mark that socket library is no longer initialized.
	 */
	osal_global->sockets_initialized = OS_FALSE;
}



/**
****************************************************************************************************

  @brief Wait for an event from one of sockets.
  @anchor osal_socket_select

  The osal_socket_select() function blocks execution of the calling thread until something
  happens with listed sockets, or interrupt select is called.

  @param   streams Array of stream pointers to wait for.
  @param   n_streams Number of stream pointers in array.

  @return  None.

****************************************************************************************************
*/
static void osal_socket_select(
	osalSocketWorkerThreadState *sockworker)
{
	WSAEVENT
		*event;

	WSANETWORKEVENTS 
		network_events;

	osalWindowsEventInfo
		*event_info;

	os_memsz
		n_events,
		event_sz,
		event_info_sz,
		i;

	osalSocket
		*s;

	/* Synchronize, this is needed to loop trough socket list.
	 */
	osal_mutex_system_lock();

	/* Number of events needed.
	 */
	n_events = sockworker->nro_sockets + 1;

	event_sz = sizeof(WSAEVENT) * n_events;
	event = osal_memory_allocate(event_sz, OS_NULL);

	event_info_sz = sizeof(osalWindowsEventInfo) * n_events;
	event_info = osal_memory_allocate(event_info_sz, OS_NULL);
	if (event == OS_NULL || event_info == OS_NULL) goto getout;

	/* Add interrupt event
	 */
	event[0] = sockworker->interrupt_event;
	event_info[0].mutex = OS_NULL;
	event_info[0].stream = OS_NULL;

	/* Release socket structures which need to be removed.
	 */
	for (s = (osalSocket*)sockworker->socket_list, i = 1;
		 s;
		 s = (osalSocket*)((osalSocketHeader*)s)->next, i++)
	{
		event[i] = s->event;
		event_info[i].mutex = s->mutex;
		event_info[i].stream = s;
	}

	/* End synchronization.
	 */
	osal_mutex_system_unlock();
	
	/* Lock all sockets. It is important to do this outside system lock.
	 */
	for (i = 1; i < n_events; i++)
	{
		osal_mutex_lock(event_info[i].mutex);
	}

	i = WSAWaitForMultipleEvents(n_events,
		event, FALSE, -1, FALSE);

	/* Interrupt event
	 */
	if (!i)
	{
		ResetEvent(event[0]);

		for (i = 1; i < n_events; i++)
		{
			s = event_info[i].stream;
			if (s->hdr.send_now)
			{
				s->hdr.send_now = OS_FALSE;
				s->hdr.hdr.callbacks.write_func((osalStream)s, 
					s->hdr.hdr.callbacks.write_context, 
					OSAL_STREAM_CALLBACK_WRITE);
			}
		}
	}

	else if (i >= 0 && i < n_events)
	{	

		s = event_info[i].stream;

		/* Resets the event
		 */
		WSAEnumNetworkEvents(
			s->handle,                           
			event[i],
			&network_events);

		if (network_events.lNetworkEvents & FD_ACCEPT)
		{
			s->hdr.hdr.callbacks.read_func((osalStream)s, 
				s->hdr.hdr.callbacks.read_context, 
					OSAL_STREAM_CALLBACK_ACCEPT);
		}

		if (network_events.lNetworkEvents & FD_CONNECT)
		{
			s->hdr.hdr.callbacks.control_func((osalStream)s, 
				s->hdr.hdr.callbacks.control_context, 
				OSAL_STREAM_CALLBACK_CONNECT);

		}

		if (network_events.lNetworkEvents & FD_READ)
		{
			s->hdr.hdr.callbacks.read_func((osalStream)s, 
				s->hdr.hdr.callbacks.read_context, 
				OSAL_STREAM_CALLBACK_READ);
		}

		if (network_events.lNetworkEvents & FD_WRITE)
		{
			s->hdr.hdr.callbacks.write_func((osalStream)s, 
				s->hdr.hdr.callbacks.write_context, 
				OSAL_STREAM_CALLBACK_WRITE);
		}


		if (network_events.lNetworkEvents & FD_CLOSE)
		{
			s->hdr.hdr.callbacks.control_func((osalStream)s, 
				s->hdr.hdr.callbacks.control_context, 
				OSAL_STREAM_CALLBACK_CLOSE);
		}
	}

	for (i = 1; i < n_events; i++)
	{
		osal_mutex_unlock(event_info[i].mutex);
	}

getout:
	osal_memory_free(event, event_sz);
	osal_memory_free(event_info, event_info_sz);

#if 0
    struct sockaddr_in 
		sin_remote;

	fd_set 
		ReadFDs, WriteFDs, ExceptFDs;

	while (1)
	{
		/* 
		osal_socket_select(
	osalStream stream[],
	os_int n_streams)

    int nAddrSize = sizeof(sinRemote);

        SetupFDSets(&ReadFDs, &WriteFDs, &ExceptFDs, 0);

        if (select(0, &ReadFDs, &WriteFDs, &ExceptFDs, 0) > 0) 
		{
            // Something happened on one of the sockets.
            // Was it the listener socket?...
            if (FD_ISSET(ListeningSocket, &ReadFDs)) 
			{
                SOCKET handle = accept(ListeningSocket, 
                        (sockaddr*)&sinRemote, &nAddrSize);
                if (handle != INVALID_SOCKET) {
                    // Tell user we accepted the socket, and add it to
                    // our connecition list.
                    cout << "Accepted connection from " <<
                            inet_ntoa(sinRemote.sin_addr) << ":" <<
                            ntohs(sinRemote.sin_port) << 
                            ", socket " << handle << "." << endl;
                    gConnections.push_back(Connection(handle));
					if ((gConnections.size() + 1) > 64) {
						// For the background on this check, see
						// www.tangentsoft.net/wskfaq/advanced.html#64sockets
						// The +1 is to account for the listener socket.
						cout << "WARNING: More than 63 client "
								"connections accepted.  This will not "
								"work reliably on some Winsock "
								"stacks!" << endl;
					}

                    // Mark the socket as non-blocking, for safety.
                    u_long nNoBlock = 1;
                    ioctlsocket(handle, FIONBIO, &nNoBlock);
                }
                else {
                    cerr << WSAGetLastErrorMessage("accept() failed") << 
                            endl;
                    return;
                }
            }
/*             else if (FD_ISSET(ListeningSocket, &ExceptFDs)) {
                int err;
                int errlen = sizeof(err);
                getsockopt(ListeningSocket, SOL_SOCKET, SO_ERROR,
                        (char*)&err, &errlen);
                cerr << WSAGetLastErrorMessage(
                        "Exception on listening socket: ", err) << endl;
                return;
            }
*/
		}



	}

#endif

}

#if 0
void SetupFDSets(fd_set *ReadFDs, fd_set *WriteFDs, 
        fd_set *ExceptFDs, SOCKET ListeningSocket) 
{
    FD_ZERO(ReadFDs);
    FD_ZERO(WriteFDs);
    FD_ZERO(ExceptFDs);

    // Add the listener socket to the read and except FD sets, if there
    // is one.
    if (ListeningSocket != INVALID_SOCKET) {
        FD_SET(ListeningSocket, ReadFDs);
        FD_SET(ListeningSocket, ExceptFDs);
    }

    // Add client connections
    ConnectionList::iterator it = gConnections.begin();

    while (it != gConnections.end()) {
		if (it->nCharsInBuffer < kBufferSize) {
			// There's space in the read buffer, so pay attention to
			// incoming data.
			FD_SET(it->handle, &ReadFDs);
		}

        if (it->nCharsInBuffer > 0) {
            // There's data still to be sent on this socket, so we need
            // to be signalled when it becomes writable.
            FD_SET(it->handle, &WriteFDs);
        }

        FD_SET(it->handle, &ExceptFDs);

        ++it;
    }
}
#endif



void osal_socket_cleanup(
	osalSocketHeader *s)
{
	osalSocket 
		*mysocket;
	
	mysocket = (osalSocket*)s;

	/* Delete event.
	 */
	if (mysocket->event) 
	{
		WSACloseEvent(mysocket->event);
	}

	if (mysocket->mutex)
	{
		osal_mutex_delete(mysocket->mutex);
	}

	/* Free socket structure.
	 */
	osal_memory_free(mysocket, sizeof(osalSocket));
}



/**
****************************************************************************************************

  @brief Wait for event from one of sockets.
  @anchor osal_socket_signal_select

  The osal_socket_signal_select() function blocks execution of the calling thread until something
  happens with listed sockets, or osal_socket_signal_select() is called.

  @param   stream Socket stream to signal.

  @return  None.

****************************************************************************************************
*/
void osal_socket_worker_ctrl(
	osalSocketWorkerThreadState *sockworker,
	osalSockWorkerAction action)
{
	if (sockworker) switch (action)
	{
		case OSAL_SOCKWORKER_SELECT:
			osal_socket_select(sockworker);
			break;

		case OSAL_SOCKWORKER_INTERRUPT:
			if (sockworker->interrupt_event)
			{
				WSASetEvent(sockworker->interrupt_event);
			}
			break;

		case OSAL_SOCKWORKER_SETUP:
			sockworker->interrupt_event = WSACreateEvent();
			break;

		case OSAL_SOCKWORKER_CLEANUP:
			if (sockworker->interrupt_event)
			{
				WSACloseEvent(sockworker->interrupt_event);
			}
			break;
	}
}



static void osal_socket_set_callbacks(
	osalSocket *mysocket,
	osalStreamCallbacks *callbacks)
{
	SOCKET
		handle;

	long 
		network_events;

	/* Save callback function pointers and callback context
	 */
	os_memcpy(&mysocket->hdr.hdr.callbacks, callbacks, sizeof(osalStreamCallbacks));

	handle = mysocket->handle;

	network_events = 0; 
	if (callbacks->read_func)
	{
		network_events |= FD_READ|FD_ACCEPT|FD_OOB;
	}
	if (callbacks->write_func)
	{
		network_events |= FD_WRITE;
	}
	if (callbacks->control_func)
	{
		network_events |= FD_CONNECT|FD_CLOSE;
	}

	mysocket->event = WSACreateEvent();
	WSAEventSelect(handle, mysocket->event, network_events);
}


/**
****************************************************************************************************

  @brief Move select to wait state
  @anchor osal_socket_lock

  The osal_socket_lock() function...

  @return  None.

****************************************************************************************************
*/
static os_boolean osal_socket_lock(
	osalSocket *mysocket)
{
	osalSocketWorkerThreadState 
		*sockworker;

	if (!osal_mutex_try_lock(mysocket->mutex))
		return OS_FALSE;

	sockworker = mysocket->hdr.worker_thread;

	if (sockworker)
	{
		osal_mutex_lock(sockworker->deadlock);
		osal_socket_worker_ctrl(sockworker, OSAL_SOCKWORKER_INTERRUPT);
		osal_event_wait(sockworker->in_deadlock, OSAL_EVENT_INFINITE);
	}

	osal_mutex_lock(mysocket->mutex);

	return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Shut down sockets.
  @anchor osal_socket_unlock

  The osal_socket_unlock() shuts down the underlying sockets library.

  @return  None.

****************************************************************************************************
*/
static void osal_socket_unlock(
	osalSocket *mysocket,
	os_boolean select_halted)
{
	osalSocketWorkerThreadState 
		*sockworker;

	if (select_halted)
	{
		sockworker = mysocket->hdr.worker_thread;

		if (sockworker)
		{
			osal_mutex_unlock(sockworker->deadlock);
		}
	}
	osal_mutex_unlock(mysocket->mutex);
}



#if OSAL_FUNCTION_POINTER_SUPPORT

osalStreamInterface osal_socket_iface
 = {osal_socket_open,
	osal_socket_close,
	osal_socket_accept,
	osal_socket_flush,
	osal_stream_default_seek,
	osal_socket_write,
	osal_socket_read,
	osal_stream_default_write_value,
	osal_stream_default_read_value,
	osal_socket_get_parameter,
	osal_socket_set_parameter};

#endif

#endif
