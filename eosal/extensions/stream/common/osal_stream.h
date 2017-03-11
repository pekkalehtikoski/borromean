/**

  @file    stream/common/osal_stream.h
  @brief   Stream interface.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file contains functions type definitions and stream interface structure 
  definition.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_STREAM_INCLUDED
#define OSAL_STREAM_INCLUDED


/**
****************************************************************************************************

  @name Stream Pointer Type

  The osalStream type is pointer to a stream, like stream handle. It is pointer to stream's 
  internal data structure, which holds the state of the stream. The stream's structure will 
  start with the stream header structure osalStreamHeader, which contains information common
  to all streams. OSAL functions cast their own stream pointers to osalStream pointers and back.

****************************************************************************************************
*/
/*@{*/

/* Declare stream header structure, defined later in this file.
 */
struct osalStreamHeader;

/** Stream pointer returned by osal_stream_open() function.
 */
typedef struct osalStreamHeader *osalStream;

/*@}*/


/**
****************************************************************************************************

  @name Flags for Stream Functions.
  @anchor osalStreamFlags

  These flags nodify how stream functions behave. Some flags are appropriate for many functions,
  and some flags are effecr only one.

****************************************************************************************************
*/
/*@{*/

/** No special flags. The OSAL_STREAM_DEFAULT (0) can be given as flag for any stream function,
    which takes flags. It simply specifies no special flags.
 */
#define OSAL_STREAM_DEFAULT 0

/** Open stream for reading. The OSAL_STREAM_READ flag is significant only for osal_stream_open()
    function. To open stream for both reading and writing, use OSAL_STREAM_RW.
 */
#define OSAL_STREAM_READ 0x0001

/** Open stream for writing. The OSAL_STREAM_WRITE flag is significant only for osal_stream_open()
    function. To open stream for both reading and writing, use OSAL_STREAM_RW. 
 */
#define OSAL_STREAM_WRITE 0x0002

/** Open stream for both reading and writing. The OSAL_STREAM_RW flag is significant only for 
    osal_stream_open() function. It simply combines OSAL_STREAM_READ and OSAL_STREAM_WRITE flags.
 */
#define OSAL_STREAM_RW (OSAL_STREAM_READ|OSAL_STREAM_WRITE)

/** Open stream for appending. The OSAL_STREAM_APPEND flag is significant only for 
    osal_stream_open() function when opening a file. Current file content is preserved
	and file pointer is set at end of file.
 */
#define OSAL_STREAM_APPEND 0x0004

/** Wait for operation to complete. The OSAL_STREAM_WAIT flag can be given to osal_stream_read(),
    osal_stream_write(), osal_stream_read_value(), osal_stream_write_value() and osal_stream_seek()
	functions. It will cause the stream to wait until operation can be fully completed or the
	stream times out.
 */
#define OSAL_STREAM_WAIT 0x0008

/** Synchronize stream access. The OSAL_STREAM_SYNCHRONIZE flag can be given to osal_stream_open(),
    function. It effects only to a few stream types. 
 */
// #define OSAL_STREAM_SYNCHRONIZE 0x0010

/** Peek data stream only. This flag tells read operations not to remove data from input buffer.
    This will not work with all streams.
 */
// #define OSAL_STREAM_PEEK 0x0020

/** Write all data or nothing. This flag tells osal_stream_write() and osal_stream_write_value() 
    functions to write all data or noting. This will not work with all streams. 
 */
// #define OSAL_STREAM_ALL_OR_NOTHING 0x0040

/** Do not write releated control code. The OSAL_STREAM_NO_REPEATED_CTRLS causes 
    osal_queue_write_value() function to check if the same control code is already last item
	of the queue. If so, tre repeated control code is not written.
 */
// #define OSAL_STREAM_NO_REPEATED_CTRLS OSAL_STREAM_READ 

/** Open a socket to to connect. Connect is default socket operation, OSAL_STREAM_CONNECT
    is zero 
 */
#define OSAL_STREAM_CONNECT 0

/** Open a socket to listen for incoming connections. 
 */
#define OSAL_STREAM_LISTEN 0x0100

/** Open a UDP multicast socket. 
 */
#define OSAL_STREAM_UDP_MULTICAST 0x0400

/** Open socket without select functionality.
 */
#define OSAL_STREAM_NO_SELECT 0x0800

/** Disable Nagle's algorithm on TCP socket.
 */
#define OSAL_STREAM_TCP_NODELAY 0x1000

/** Disable reusability of the socket descriptor.
 */
#define OSAL_STREAM_NO_REUSEADDR 0x2000

/** Open socket in blocking mode. Another name OSAL_STREAM_SYNCHRONIZE.
 */
#define OSAL_STREAM_BLOCKING 0x4000

/* Nore: bit 0x0100000 and larger are reserved to eStream
 */


/*@}*/


#if 0
/**
****************************************************************************************************

  @name Stream Control Codes
  @anchor osalStreamCtrlCodes

  These flags nodify how stream functions behave. Some flags are appropriate for many functions,
  and some flags are effecr only one.

****************************************************************************************************
*/
typedef enum
{
	/** Reserved code to mark control character itself. This is not used as control code.
	 */
	OSAL_STREAM_CTRL_CHAR = 0x101,

	/** Buffer overflow has occurred and data is lost.
	 */
	OSAL_STREAM_CTRL_BUF_OVERFLOW = 0x102,

	/** New connection has been established.
	 */
	OSAL_STREAM_CTRL_CONNECT = 0x103,

	/** A connection has been disconnected. 
	 */
	OSAL_STREAM_CTRL_DISCONNECT = 0x104
}
osalStreamCtrlCode;


/** If control code is converted to UTF32, this number is added to it to make it above any
    possible Unicode point.
 */
#define OSAL_STREAM_CTRL_UTF32_BASE 0x200000
#endif



/**
****************************************************************************************************

  @name Stream Parameter Enumeration
  @anchor osalStreamParameterIx

  Streams may have parameters osal_stream_get_parameter() and osal_stream_set_parameter().

****************************************************************************************************
*/
/*@{*/

/** Stream parameter enumeration. Indexes of all stream parameters.
    @anchor osalStreamParameterIx
 */
typedef enum
{
    /** Number of bytes which can be read immediately.
     */
	OSAL_STREAM_IN_BYTES,

    /** Number of bytes which can be written immediately.
     */
	OSAL_STREAM_OUT_SPACE,

    /** Timeout for writing data, milliseconds.
     */
	OSAL_STREAM_WRITE_TIMEOUT_MS,

    /** Timeout for reading data, milliseconds.
     */
	OSAL_STREAM_READ_TIMEOUT_MS,
}
osalStreamParameterIx;

/*@}*/




/** 
****************************************************************************************************

  Data returned by osal_stream_select()

****************************************************************************************************
*/

/* Bit fields for eventflags.
 */
#define OSAL_STREAM_ACCEPT_EVENT  0x0001
#define OSAL_STREAM_CONNECT_EVENT 0x0002
#define OSAL_STREAM_CLOSE_EVENT   0x0004
#define OSAL_STREAM_READ_EVENT    0x0008
#define OSAL_STREAM_WRITE_EVENT   0x0010

/* Custom event
 */
#define OSAL_STREAM_NR_CUSTOM_EVENT -1
#define OSAL_STREAM_CUSTOM_EVENT  0x0100

/* Information back from select function
 */
typedef struct osalSelectData
{
    os_int stream_nr;  /* zero based stream number */
    os_int eventflags; /* which events have occurred, like read possible, write possible */
    os_int errorcode; /* Error code, 0 = all fine */
}
osalSelectData;


#if OSAL_FUNCTION_POINTER_SUPPORT

/** 
****************************************************************************************************

  Stream Interface structure.

  The interface structure contains set of function pointers. These function pointers point 
  generally to functions which do implemen a specific stream. The functions pointer can also 
  point to default implementations in osal_stream.c. This structure exists only if the compiler 
  and the operating system support function pointers, see define OSAL_FUNCTION_POINTER_SUPPORT.

****************************************************************************************************
 */
typedef struct osalStreamInterface
{
	osalStream (*stream_open)(
		os_char *parameters,
		void *option,
		osalStatus *status,
		os_int flags);

	void (*stream_close)(
		osalStream stream);

	osalStream (*stream_accept)(
		osalStream stream,
		osalStatus *status,
		os_int flags);

	osalStatus (*stream_flush)(
		osalStream stream,
		os_int flags);

	osalStatus (*stream_seek)(
		osalStream stream,
		os_long *pos,
		os_int flags);

	osalStatus (*stream_write)(
		osalStream stream,
		const os_uchar *buf,
		os_memsz n,
		os_memsz *n_written,
		os_int flags);

	osalStatus (*stream_read)(
		osalStream stream,
		os_uchar *buf,
		os_memsz n,
		os_memsz *n_read,
		os_int flags);

	osalStatus (*stream_write_value)(
		osalStream stream,
		os_ushort c,
		os_int flags);

	osalStatus (*stream_read_value)(
		osalStream stream,
		os_ushort *c,
		os_int flags);

	os_long (*stream_get_parameter)(
		osalStream stream,
		osalStreamParameterIx parameter_ix);

	void (*stream_set_parameter)(
		osalStream stream,
		osalStreamParameterIx parameter_ix,
		os_long value);

	osalStatus (*stream_select)(
		osalStream *streams,
        os_int nstreams,
		osalEvent evnt,
		osalSelectData *selectdata,
		os_int flags);
}
osalStreamInterface;

#endif


/**
****************************************************************************************************

  Stream header structure.
  @anchor osalStreamHeader

  Stream pointer can be casted to osalStreamHeader to access information what is common to
  cll streams.

****************************************************************************************************
*/
typedef struct osalStreamHeader
{
#if OSAL_FUNCTION_POINTER_SUPPORT
	/** Pointer to stream interface is always first item of the handle
	 */
	osalStreamInterface *iface;
#endif

    /** Timeout for writing data, milliseconds. Value -1 indicates infinite timeout.
     */
	os_int write_timeout_ms;

    /** Timeout for reading data, milliseconds. Value -1 indicates infinite timeout.
     */
	os_int read_timeout_ms;
}
osalStreamHeader;


#if OSAL_FUNCTION_POINTER_SUPPORT

/** 
****************************************************************************************************

  @name Stream Functions

  These functions access the underlying stream implementation trough iface pointer in 
  stream data structure's osalStreamHeader member. The functions add extra capabilities 
  to streams, most importantly support for OSAL_STREAM_WAIT flag. These functions can be 
  used only if the compiler and the platform support function pointers, see define
  OSAL_FUNCTION_POINTER_SUPPORT.

****************************************************************************************************
 */
/*@{*/

osalStream osal_stream_open(
	osalStreamInterface *iface,
	os_char *parameters,
	void *option,
	osalStatus *status,
	os_int flags);

void osal_stream_close(
	osalStream stream);

osalStream osal_stream_accept(
	osalStream stream,
	osalStatus *status,
	os_int flags);

osalStatus osal_stream_flush(
	osalStream stream,
	os_int flags);

osalStatus osal_stream_seek(
	osalStream stream,
	os_long *pos,
	os_int flags);

osalStatus osal_stream_write(
	osalStream stream,
	const os_uchar *buf,
	os_memsz n,
	os_memsz *n_written,
	os_int flags);

osalStatus osal_stream_read(
	osalStream stream,
	os_uchar *buf,
	os_memsz n,
	os_memsz *n_read,
	os_int flags);

osalStatus osal_stream_write_value(
	osalStream stream,
	os_ushort c,
	os_int flags);

osalStatus osal_stream_read_value(
	osalStream stream,
	os_ushort *c,
	os_int flags);

os_long osal_stream_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix);

void osal_stream_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value);

osalStatus osal_stream_select(
	osalStream *streams,
    os_int nstreams,
	osalEvent evnt,
	osalSelectData *selectdata,
	os_int flags);

/*@}*/

#endif

/** 
****************************************************************************************************

  @name Default Function Implementations

  A stream implementation may not need to implement all stream functions. The default 
  implementations here can be used for to fill in those places in stream interface structure,
  or called from stream's own function to handle general part of the job.

****************************************************************************************************
 */
/*@{*/

osalStream osal_stream_default_accept(
	osalStream stream,
	osalStatus *status,
	os_int flags);

osalStatus osal_stream_default_seek(
	osalStream stream,
	os_long *pos,
	os_int flags);

osalStatus osal_stream_default_write_value(
	osalStream stream,
	os_ushort c,
	os_int flags);

osalStatus osal_stream_default_read_value(
	osalStream stream,
	os_ushort *c,
	os_int flags);

os_long osal_stream_default_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix);

void osal_stream_default_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value);

osalStatus osal_stream_default_select(
	osalStream *streams,
    os_int nstreams,
	osalEvent evnt,
	osalSelectData *selectdata,
	os_int flags);


/*@}*/


#endif
