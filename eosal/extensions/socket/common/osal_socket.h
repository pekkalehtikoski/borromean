/**

  @file    modules/socket/osal_socket.h
  @brief   OSAL Sockets API.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file contains function prototypes and definitions for OSAL sockets API. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_SOCKET_INCLUDED
#define OSAL_SOCKET_INCLUDED

/** Stream interface structure for sockets.
 */
#if OSAL_FUNCTION_POINTER_SUPPORT
extern osalStreamInterface osal_socket_iface;
#endif

/** Define to get socket interface pointer. The define is used so that this can 
    be converted to function call.
 */
#define OSAL_SOCKET_IFACE &osal_socket_iface





/** 
****************************************************************************************************

  @name Socket Functions.

  These functions implement sockets as OSAL stream. These functions can either be called 
  directly or through stream interface.

****************************************************************************************************
 */
/*@{*/

/* Open socket.
 */
osalStream osal_socket_open(
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	void *option,
	osalStatus *status,
	os_short flags);

/* Close socket.
 */
void osal_socket_close(
	osalStream stream);

/* Accept connection from listening socket.
 */
/* osalStream osal_socket_select(
	osalStream *stream,
    os_int n); */

/* Accept connection from listening socket.
 */
osalStream osal_socket_accept(
	osalStream stream,
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	osalStatus *status,
	os_short flags);


/* Flush written data to socket.
 */
osalStatus osal_socket_flush(
	osalStream stream,
	os_short flags);

/* Write data to socket.
 */
osalStatus osal_socket_write(
	osalStream stream,
	const os_uchar *buf,
	os_memsz n,
	os_memsz *n_written,
	os_short flags);

/* Read data from socket.
 */
osalStatus osal_socket_read(
	osalStream stream,
	os_uchar *buf,
	os_memsz n,
	os_memsz *n_read,
	os_short flags);

/* Get socket parameter.
 */
os_long osal_socket_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix);

/* Set socket parameter.
 */
void osal_socket_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value);

void osal_socket_worker_ctrl(
	osalSocketWorkerThreadState *sockworker,
	osalSockWorkerAction action);

/* Initialize sockets.
 */
void osal_socket_initialize(
	void);

/* Shut down sockets.
 */
void osal_socket_shutdown(
	void);



/* Get host and port from network address string.
 */
os_char *osal_socket_get_host_name_and_port(
	os_char *parameters,
	os_int  *port,
	os_memsz *buf_sz);


void osal_socket_cleanup(
	osalSocketHeader *s);


/*@}*/


#endif
