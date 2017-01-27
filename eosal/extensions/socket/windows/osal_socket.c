/**

  @file    eosal/extensions/socket/windows/osal_socket.c
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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>

/**
****************************************************************************************************

  @name Socket data structure for Windows socket encapsulation.

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
	osalStreamHeader hdr;

	/** Operating system's socket handle.
	 */
	SOCKET handle;

	/** Even to be set when new data has been received, can be sent, new connection has been 
		created, accepted or closed socket.
	 */
	WSAEVENT event;

	/** Stream open flags. Flags which were given to osal_socket_open() or osal_socket_accept()
        function. 
	 */
	os_int open_flags;

    os_boolean is_ipv6;
} 
osalSocket;


/**
****************************************************************************************************

  @brief Open a socket.
  @anchor osal_socket_open

  The osal_socket_open() function opens a socket. The socket can be either listening TCP 
  socket, connecting TCP socket or UDP multicast socket. 

  @param  parameters Socket parameters, a list string or direct value.
		  Address and port to connect to, or interface and port to listen for.
          Socket IP address and port can be specified either as value of "addr" item
          or directly in parameter sstring. For example "192.168.1.55:20" or "localhost:12345"
          specify IPv4 addressed. If only port number is specified, which is often 
          useful for listening socket, for example ":12345".
          IPv4 address is automatically recognized from numeric address like
          "2001:0db8:85a3:0000:0000:8a2e:0370:7334", but not when address is specified as string
          nor for empty IP specifying only port to listen. Use brackets around IP address
          to mark IPv6 address, for example "[localhost]:12345", or "[]:12345" for empty IP.

  @param  callbacks Callback functions.

  @param  option Not used for sockets, set OS_NULL.

  @param  status Pointer to integer into which to store the function status code. Value
		  OSAL_SUCCESS (0) indicates success and all nonzero values indicate an error.
          See @ref osalStatus "OSAL function return codes" for full list.
		  This parameter can be OS_NULL, if no status code is needed. 

  @param  flags Flags for creating the socket. Bit fields, combination of:
          - OSAL_STREAM_CONNECT: Connect to specified socket port at specified IP address. 
          - OSAL_STREAM_LISTEN: Open a socket to listen for incoming connections. 
          - OSAL_STREAM_UDP_MULTICAST: Open a UDP multicast socket. 
          - OSAL_STREAM_NO_SELECT: Open socket without select functionality.
          - OSAL_STREAM_TCP_NODELAY: Disable Nagle's algorithm on TCP socket.
          - OSAL_STREAM_NO_REUSEADDR: Disable reusability of the socket descriptor.
          - OSAL_STREAM_BLOCKING: Open socket in blocking mode.

		  See @ref osalStreamFlags "Flags for Stream Functions" for full list of stream flags.

  @return Stream pointer representing the socket, or OS_NULL if the function failed.

****************************************************************************************************
*/
osalStream osal_socket_open(
	os_char *parameters,
	void *option,
	osalStatus *status,
	os_int flags)
{
	osalSocket *mysocket = OS_NULL;
	os_memsz host_sz, sz1, sz2;
	os_int port_nr;
	os_char *host, *hostbuf, nbuf[OSAL_NBUF_SZ];
    os_ushort *host_utf16, *port_utf16;
    ADDRINFOW *addrinfo = NULL;
    ADDRINFOW *ptr = NULL;
    ADDRINFOW hints;
	osalStatus rval;
	SOCKET handle = INVALID_SOCKET;
	struct sockaddr_in saddr;
    struct sockaddr_in6 saddr6;
    struct sockaddr *sa;
    os_boolean is_ipv6;
    int af, udp, on = 1, s, sa_sz;

    /* Initialize sockets library, if not already initialized. 
     */
    osal_socket_initialize();

	/* Get host name or numeric IP address and TCP port number from parameters.
       The host buffer must be released by calling osal_memory_free() function,
       unless if host is OS_NULL (unpecified).
	 */
    port_nr = OSAL_DEFAULT_SOCKET_PORT;
	host = hostbuf = osal_socket_get_host_name_and_port(parameters, &port_nr, &host_sz, &is_ipv6);
    udp = (flags & OSAL_STREAM_UDP_MULTICAST) ? OS_TRUE : OS_FALSE;

    af = is_ipv6 ? AF_INET6 : AF_INET;
    (struct sockaddr*)

    os_memclear(&hints, sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = udp ? IPPROTO_UDP : IPPROTO_TCP;
    sa = is_ipv6 ? (struct sockaddr *)&saddr6 : (struct sockaddr *)&saddr;
    sa_sz = is_ipv6 ? sizeof(saddr6) : sizeof(saddr);
    os_memclear(sa, sa_sz);
    
    if (host)
    {
        if (InetPton(af, host, sa) <= 0)
        {

            host_utf16 = osal_string_utf8_to_utf16_malloc(host, &sz1);
            osal_int_to_string(nbuf, sizeof(nbuf), port_nr);
            port_utf16 = osal_string_utf8_to_utf16_malloc(nbuf, &sz2);

            s = GetAddrInfoW(host_utf16, port_utf16,
                &hints, &addrinfo);

            osal_memory_free(host_utf16, sz1);
            osal_memory_free(port_utf16, sz2);

            if (s || addrinfo == NULL) 
		    {
                if (addrinfo) FreeAddrInfoW(addrinfo);
			    rval = OSAL_STATUS_FAILED;
			    goto getout;
            }

            for (ptr = addrinfo; ptr != NULL; ptr = ptr->ai_next) 
            {
                if (ptr->ai_family == af) 
                {
                    os_memcpy(sa,  ptr->ai_addr, sa_sz);
                    break;
                }
            }

            FreeAddrInfoW(addrinfo);

            /* If no match found
             */
            if (ptr == NULL)
            {
			    rval = OSAL_STATUS_FAILED;
			    goto getout;
            }
	    }
    }

    /* Create socket.
     */
    handle = socket(af, hints.ai_socktype, hints.ai_protocol);
    if (handle == INVALID_SOCKET) 
	{
		rval = OSAL_STATUS_FAILED;
		goto getout;
	}

    /* Set socket reuse flag.
     */
    if ((flags & OSAL_STREAM_NO_REUSEADDR) == 0)
    {
        if (setsockopt(handle, SOL_SOCKET,  SO_REUSEADDR,
            (char *)&on, sizeof(on)) < 0)
        {
		    rval = OSAL_STATUS_FAILED;
		    goto getout;
        }
    }

	/* Set non blocking mode.
	 */
    if ((flags & OSAL_STREAM_BLOCKING) == 0)
    {
    	ioctlsocket(handle, FIONBIO, &on);
    }

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
    mysocket->is_ipv6 = is_ipv6;

	/* Save interface pointer.
	 */
	mysocket->hdr.iface = &osal_socket_iface;

	/* Set 0 timeouts.
	 */
	mysocket->hdr.write_timeout_ms = mysocket->hdr.read_timeout_ms = 0;

    /* If we are preparing to use this with select function.
     */
    if ((flags & OSAL_STREAM_NO_SELECT) == 0)
    {   
        /* Create event
         */
        mysocket->event = WSACreateEvent();
        if (mysocket->event == WSA_INVALID_EVENT)
        {
		    rval = OSAL_STATUS_MEMORY_ALLOCATION_FAILED;
		    goto getout;
        }

        if (WSAEventSelect(handle, mysocket->event, FD_ACCEPT|FD_CONNECT|FD_CLOSE|FD_READ|FD_WRITE) == SOCKET_ERROR)
        {
		    rval = OSAL_STATUS_FAILED;
		    goto getout;
        }           
    }

    if (is_ipv6)
    {
        saddr6.sin6_family = AF_INET6;
        if (host == OS_NULL) memcpy(&saddr6.sin6_addr, &in6addr_any, sizeof(in6addr_any));
        saddr6.sin6_port = htons(port_nr);
    }
    else
    {
        saddr.sin_family = AF_INET;
        if (host == OS_NULL) saddr.sin_addr.s_addr = htonl(INADDR_ANY); 
        saddr.sin_port = htons(port_nr);
    } 

	if (flags & (OSAL_STREAM_LISTEN | OSAL_STREAM_UDP_MULTICAST))
	{
		if (bind(handle, sa, sa_sz)) 
		{
			rval = OSAL_STATUS_FAILED;
			goto getout;
		}

        /* Set the listen back log
         */
	    if (flags & OSAL_STREAM_LISTEN)
            if (listen(handle, 32) , 0)
        {
		    rval = OSAL_STATUS_FAILED;
		    goto getout;
        }
	}

	else 
	{
		if (connect(handle, sa, sa_sz))
		{
            rval = WSAGetLastError();
            if (rval != WSAEWOULDBLOCK )
            {
			    rval = OSAL_STATUS_FAILED;
			    goto getout;
            }
		}
	}

	/* Release memory allocated for the host name or address.
	 */
	osal_memory_free(hostbuf, host_sz);

	/* Success set status code and cast socket structure pointer to stream pointer and return it.
	 */
	if (status) *status = OSAL_SUCCESS;
	return (osalStream)mysocket;

getout:
	/* Opt out on error. First Release memory allocated for the host name or address.
	 */
	osal_memory_free(hostbuf, host_sz);

    /* If we got far enough to allocate the socket structure.
       Close the event handle (if any) and free memory allocated
       for the socket structure.
     */
    if (mysocket)
    {
        if (mysocket->event) 
	    {
		    WSACloseEvent(mysocket->event);
	    }

        osal_memory_free(mysocket, sizeof(osalSocket));
    }

    /* Close socket
     */    
	if (handle != INVALID_SOCKET) 
	{
		closesocket(handle);
	}

	/* Set status code and return NULL pointer.
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
	osalSocket *mysocket;
	SOCKET handle;
    char buf[64];
	int n, rval;

	/* If called with NULL argument, do nothing.
	 */
	if (stream == OS_NULL) return;

	/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
	 */
	mysocket = (osalSocket*)stream;
	handle = mysocket->handle;

	/* If socket operating system socket is not already closed, close now.
	 */
	if (handle != INVALID_SOCKET)
	{
		/* Mark socket closed
		 */
		mysocket->handle = INVALID_SOCKET;

	    if (mysocket->event) 
	    {
		    WSACloseEvent(mysocket->event);
	    }

		/* Disable sending data. This informs other the end of socket that it is going down now.
		 */
		if (shutdown(handle, SD_SEND)) 
		{
            rval = WSAGetLastError();
            if (rval != WSAENOTCONN)
            {
			    osal_debug_error("shutdown() failed");
            }
		}

		/* Read data to be received until receive buffer is empty.
		 */
		do
		{
			n = recv(handle, buf, sizeof(buf), 0);
			if (n == SOCKET_ERROR) 
			{
	#if OSAL_DEBUG
                rval = WSAGetLastError();
				if (rval != WSAEWOULDBLOCK && rval != WSAENOTCONN) 
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
	}
}


/**
****************************************************************************************************

  @brief Accept connection from listening socket.
  @anchor osal_socket_open

  The osal_socket_accept() function accepts an incoming connection from listening socket.

  @param   stream Stream pointer representing the listening socket.

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
	osalStatus *status,
	os_int flags)
{
	osalSocket *mysocket, *newsocket = OS_NULL;
	SOCKET handle, new_handle;
	int addr_size, on = 1;
	struct sockaddr_in sin_remote;
	struct sockaddr_in6 sin_remote6;
	osalStatus rval;

	if (stream)
	{
	/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
	 */
		mysocket = (osalSocket*)stream;
		handle = mysocket->handle;

		/* If socket operating system socket is not already closed.
		 */
		if (handle != INVALID_SOCKET)
		{
            if (mysocket->is_ipv6) 
            {
			    addr_size = sizeof(sin_remote6);
			    new_handle = accept(handle, (struct sockaddr*)&sin_remote6, &addr_size);
            }
            else
            {
			    addr_size = sizeof(sin_remote);
			    new_handle = accept(handle, (struct sockaddr*)&sin_remote, &addr_size);
            }

		}
		else
		{
			new_handle = INVALID_SOCKET;
		}

		/* If no new connection, do nothing more.
		 */
        if (new_handle == INVALID_SOCKET) 
		{
			if (status) *status = OSAL_STATUS_NO_NEW_CONNECTION;
			return OS_NULL;
		}

        /* Set socket reuse flag.
         */
        if ((flags & OSAL_STREAM_NO_REUSEADDR) == 0)
        {
            if (setsockopt(new_handle, SOL_SOCKET,  SO_REUSEADDR,
                (char *)&on, sizeof(on)) < 0)
            {
		        rval = OSAL_STATUS_FAILED;
		        goto getout;
            }
        }

	    /* Set non blocking mode.
	     */
        if ((flags & OSAL_STREAM_BLOCKING) == 0)
        {
    	    if (ioctlsocket(new_handle, FIONBIO, &on) == SOCKET_ERROR)
            {
		        rval = OSAL_STATUS_FAILED;
		        goto getout;
            }
        }

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
        newsocket->is_ipv6 = mysocket->is_ipv6;

		/* Save interface pointer.
		 */
	#if OSAL_FUNCTION_POINTER_SUPPORT
		newsocket->hdr.iface = &osal_socket_iface;
	#endif

	    /* Set 0 timeouts.
	     */
		newsocket->hdr.write_timeout_ms = newsocket->hdr.read_timeout_ms = 0;

        /* If we are preparing to use this with select function.
         */
        if ((flags & OSAL_STREAM_NO_SELECT) == 0)
        {   
            /* Create event
             */
            newsocket->event = WSACreateEvent();
            if (newsocket->event == WSA_INVALID_EVENT)
            {
		        rval = OSAL_STATUS_MEMORY_ALLOCATION_FAILED;
		        goto getout;
            }

            if (WSAEventSelect(new_handle, newsocket->event,
                FD_ACCEPT|FD_CONNECT|FD_CLOSE|FD_READ|FD_WRITE) == SOCKET_ERROR)
            {
		        rval = OSAL_STATUS_FAILED;
		        goto getout;
            }           
        }

		/* Success set status code and cast socket structure pointer to stream pointer 
		   and return it.
		 */
		if (status) *status = OSAL_SUCCESS;
		return (osalStream)newsocket;
	}

getout:
	/* Opt out on error. If we got far enough to allocate the socket structure.
       Close the event handle (if any) and free memory allocated  for the socket structure.
     */
    if (newsocket)
    {
        if (newsocket->event) 
	    {
		    WSACloseEvent(newsocket->event);
	    }

        osal_memory_free(newsocket, sizeof(osalSocket));
    }

    /* Close socket
     */    
	if (new_handle != INVALID_SOCKET) 
	{
		closesocket(new_handle);
	}

	/* Set status code and return NULL pointer.
	 */
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
	os_int flags)
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
	os_int flags)
{
	int rval;
	osalSocket *mysocket;
	SOCKET handle;

	if (stream)
	{
		/* Cast stream pointer to socket structure pointer.
		 */
		mysocket = (osalSocket*)stream;

		/* Special case. Writing 0 bytes will trigger write callback by worker thread.
		 */
		if (n == 0)
		{
			*n_written = 0;
			return OSAL_SUCCESS;
		}

		/* Lock socket and get OS socket handle.
		 */
		handle = mysocket->handle;

		/* If operating system socket is already closed.
		 */
		if (handle == INVALID_SOCKET)
		{
			goto getout;
		}

        /* Limit number of bytes to write at one to 2^31
         */
        if (n > 0x7FFFFFFFFFFFFFFE) n = 0x7FFFFFFFFFFFFFFF;
        
		rval = send(handle, buf, (int)n, 0);

		if (rval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) 
			{
				goto getout;
			}
			rval = 0;
		}

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
	os_int flags)
{
	int rval;
	osalSocket *mysocket;
	SOCKET handle;

	if (stream)
	{
		/* Cast stream pointer to socket structure pointer, lock socket and get OS socket handle.
		 */
		mysocket = (osalSocket*)stream;
		handle = mysocket->handle;

		/* If operating system socket is already closed.
		 */
		if (handle == INVALID_SOCKET)
		{
			goto getout;
		}

        /* Limit number of bytes to read at one to 2^31.
         */
        if (n > 0x7FFFFFFFFFFFFFFE) n = 0x7FFFFFFFFFFFFFFF;

		rval = recv(handle, buf, (int)n, 0);

		if (rval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) 
			{
				goto getout;
			}
			rval = 0;
		}

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

  @brief Wait for an event from one of sockets.
  @anchor osal_socket_select

  The osal_socket_select() function blocks execution of the calling thread until something
  happens with listed sockets, or interrupt select is called.

  @param   streams Array of stream pointers to wait for.
  @param   n_streams Number of stream pointers in array.

  @return  None.

****************************************************************************************************
*/
osalStatus osal_socket_select(
	osalStream *streams,
    os_int nstreams,
	osalEvent evnt,
	osalSelectData *selectdata,
	os_int flags)
{
	osalSocket *mysocket;
    osalSocket *sockets[OSAL_SOCKET_SELECT_MAX+1];
	WSAEVENT events[OSAL_SOCKET_SELECT_MAX+1];
	os_int ixtable[OSAL_SOCKET_SELECT_MAX+1];
	WSANETWORKEVENTS network_events;
    os_int i, n_sockets, n_events, event_nr, eventflags, errorcode;
    DWORD rval;

    if (nstreams < 1 || nstreams > OSAL_SOCKET_SELECT_MAX)
        return OSAL_STATUS_FAILED;

    n_sockets = 0;
    for (i = 0; i < nstreams; i++)
    {
        mysocket = (osalSocket*)streams[i];
        if (mysocket)
        {
            sockets[n_sockets] = mysocket; 
            events[n_sockets] = mysocket->event;
            ixtable[n_sockets++] = i;
        }
    }
    n_events = n_sockets;

    /* If we have event, add it to wait.
     */
    if (evnt)
    {
        events[n_events++] = evnt;
    }

    rval = WSAWaitForMultipleEvents(n_events,
		events, FALSE, WSA_INFINITE, FALSE);

    event_nr = (os_int)(rval - WSA_WAIT_EVENT_0);

    if (evnt && event_nr == n_sockets)
    {
        selectdata->eventflags = OSAL_STREAM_CUSTOM_EVENT;
        selectdata->stream_nr = OSAL_STREAM_NR_CUSTOM_EVENT;
		return OSAL_SUCCESS;
    }

    if (event_nr < 0 || event_nr >= n_sockets)
    {
		return OSAL_STATUS_FAILED;
    }

    if (WSAEnumNetworkEvents(sockets[event_nr]->handle,
        events[event_nr], &network_events) == SOCKET_ERROR)
    {
		return OSAL_STATUS_FAILED;
    }

    eventflags = 0;
    errorcode = OSAL_SUCCESS;
	if (network_events.lNetworkEvents & FD_ACCEPT)
	{
        eventflags |= OSAL_STREAM_ACCEPT_EVENT;
        if (network_events.iErrorCode[FD_ACCEPT_BIT])
        {
            errorcode = OSAL_STATUS_FAILED;
        }
	}

	if (network_events.lNetworkEvents & FD_CONNECT)
	{
        eventflags |= OSAL_STREAM_CONNECT_EVENT;
        if (network_events.iErrorCode[FD_CONNECT_BIT])
        {
            errorcode = OSAL_STATUS_FAILED;
        }
	}

	if (network_events.lNetworkEvents & FD_CLOSE)
	{
        eventflags |= OSAL_STREAM_CLOSE_EVENT;
        if (network_events.iErrorCode[FD_CLOSE_BIT])
        {
            errorcode = OSAL_STATUS_FAILED;
        }
	}

	if (network_events.lNetworkEvents & FD_READ)
	{
        eventflags |= OSAL_STREAM_READ_EVENT;
        if (network_events.iErrorCode[FD_READ_BIT])
        {
            errorcode = OSAL_STATUS_FAILED;
        }
	}

	if (network_events.lNetworkEvents & FD_WRITE)
	{
        eventflags |= OSAL_STREAM_WRITE_EVENT;
	}

    selectdata->eventflags = eventflags;
    selectdata->errorcode = errorcode;
    selectdata->stream_nr = ixtable[event_nr];

    // ResetEvent(event[event_nr]);  ??????

    return OSAL_SUCCESS;
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
    /** Windows socket library version information.
     */
    WSADATA osal_wsadata;

	/* If socket library is already initialized, do nothing.
	 */
	if (osal_global->sockets_shutdown_func) return;

	/* Lock the system mutex to syncronize.
	 */
	osal_mutex_system_lock();

	/* If socket library is already initialized, do nothing. Double checked here
	   for thread synchronization.
	 */
	if (!osal_global->sockets_shutdown_func) 
	{
		/* Initialize winsock.
		 */
		if (WSAStartup(MAKEWORD(2,2), &osal_wsadata))
		{
			osal_debug_error("WSAStartup() failed");
			return;
		}

		/* Mark that socket library has been initialized by setting shutdown function pointer.
           Now the pointer is shared on windows by main program and DLL. If this needs to
           be separated, move sockets_shutdown_func pointer from global structure to
           plain global variable.
		 */
		osal_global->sockets_shutdown_func = osal_socket_shutdown;
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
	if (!osal_global->sockets_shutdown_func) return;

	/* Initialize winsock.
	 */
	if (WSACleanup())
	{
		osal_debug_error("WSACleanup() failed");
		return;
	}

	/* Mark that socket library is no longer initialized.
	 */
    osal_global->sockets_shutdown_func = OS_NULL;
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
	osal_socket_set_parameter,
    osal_socket_select};

#endif

