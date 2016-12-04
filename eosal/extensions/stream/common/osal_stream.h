/**

  @file    include/osal_stream.h
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
#define OSAL_STREAM_READ 1

/** Open stream for writing. The OSAL_STREAM_WRITE flag is significant only for osal_stream_open()
    function. To open stream for both reading and writing, use OSAL_STREAM_RW. 
 */
#define OSAL_STREAM_WRITE 2

/** Open stream for both reading and writing. The OSAL_STREAM_RW flag is significant only for 
    osal_stream_open() function. It simply combines OSAL_STREAM_READ and OSAL_STREAM_WRITE flags.
 */
#define OSAL_STREAM_RW (OSAL_STREAM_READ|OSAL_STREAM_WRITE)

/** Open stream for appending. The OSAL_STREAM_APPEND flag is significant only for 
    osal_stream_open() function when opening a file. Current file content is preserved
	and file pointer is set at end of file.
 */
#define OSAL_STREAM_APPEND 4

/** Wait for operation to complete. The OSAL_STREAM_WAIT flag can be given to osal_stream_read(),
    osal_stream_write(), osal_stream_read_value(), osal_stream_write_value() and osal_stream_seek()
	functions. It will cause the stream to wait until operation can be fully completed or the
	stream times out.
 */
#define OSAL_STREAM_WAIT 8

/** Synchronize stream access. The OSAL_STREAM_SYNCHRONIZE flag can be given to osal_stream_open(),
    function. It effects only to a few stream types. 
 */
#define OSAL_STREAM_SYNCHRONIZE 16

/** Peek data stream only. This flag tells read operations not to remove data from input buffer.
    This will not work with all streams.
 */
#define OSAL_STREAM_PEEK 32

/** Write all data or nothing. This flag tells osal_stream_write() and osal_stream_write_value() 
    functions to write all data or noting. This will not work with all streams. This uses same 
	flag value as OSAL_STREAM_PEEK, but used by different functions.
 */
#define OSAL_STREAM_ALL_OR_NOTHING OSAL_STREAM_PEEK

/** Do not write releated control code. The OSAL_STREAM_NO_REPEATED_CTRLS causes 
    osal_queue_write_value() function to check if the same control code is already last item
	of the queue. If so, tre repeated control code is not written.
 */
#define OSAL_STREAM_NO_REPEATED_CTRLS OSAL_STREAM_READ

/** Open a socket to listen for incoming connections. This uses same flag value as OSAL_STREAM_PEEK,
    but used by different functions.
 */
#define OSAL_STREAM_LISTEN OSAL_STREAM_PEEK

/*@}*/


/**
****************************************************************************************************

  @name Stream Control Codes
  @anchor osalStreamFlags

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

/**
****************************************************************************************************

  Enumeration of Reasons for Callback
  @anchor osalStreamCallbackEnum

  The stream callback function get reason for callback as argument. 

****************************************************************************************************
*/
typedef enum
{
	OSAL_STREAM_CALLBACK_ACCEPT,
	OSAL_STREAM_CALLBACK_CONNECT,
	OSAL_STREAM_CALLBACK_CLOSE,
	OSAL_STREAM_CALLBACK_READ,
	OSAL_STREAM_CALLBACK_WRITE,

	OSAL_STREAM_CALLBACK_SHUTDOWN,

	OSAL_STREAM_CALLBACK_INTERRUPT
} 
osalStreamCallbackEnum;


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

  @name Stream Callback Function Type

  Stream callback...

****************************************************************************************************
*/
/*@{*/

/** Stream callback function type. 
    @anchor osal_stream_func

    @param  stream Stream pointer.
	@param  context Callback context.
	@param  reason Reason for callback.
	@return None.
 */
typedef void osal_stream_func(
	osalStream stream,
	void *context,
	osalStreamCallbackEnum reason);

/*@}*/


/** 
****************************************************************************************************

  Stream callback function pointer and context pointer structure.

  Structure to pass callback function pointers and callback context pointers to osal_stream_open(),
  etc, or osal_socket_accept() function. 

****************************************************************************************************
*/
typedef struct
{
	/** Pointer to control function. The callback function is called for control type events
	    on the stream. These are OSAL_STREAM_CALLBACK_CONNECT, OSAL_STREAM_CALLBACK_CLOSE and 
		OSAL_STREAM_CALLBACK_SHUTDOWN.
	 */
	osal_stream_func *control_func;

	/** Context for control_func. The context is application defined pointer to be passed to
	    the callback function.
	 */
	void *control_context;

	/** Pointer to read function. The callback function is called when new data is received 
	    and ready to be read, or a socket connection is accepted. This callback is called for 
		OSAL_STREAM_CALLBACK_READ or OSAL_STREAM_CALLBACK_ACCEPT.
	 */
	osal_stream_func *read_func;

	/** Context for read_func. The context is application defined pointer to be passed to
	    the callback function.
	 */
	void *read_context;

	/** Pointer to read function. The callback function is called when there is space for new
	    data to be written to stream. This callback is called for OSAL_STREAM_CALLBACK_WRITE.
	 */
	osal_stream_func *write_func;

	/** Context for write_func. The context is application defined pointer to be passed to
	    the callback function.
	 */
	void *write_context;
}
osalStreamCallbacks;


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
	/** Ulle. Dulle.
	 */
	osalStream (*stream_open)(
		os_char *parameters,
		osalStreamCallbacks *callbacks,
		void *option,
		osalStatus *status,
		os_short flags);

	void (*stream_close)(
		osalStream stream);

	osalStream (*stream_accept)(
		osalStream stream,
		os_char *parameters,
		osalStreamCallbacks *callbacks,
		osalStatus *status,
		os_short flags);

	osalStatus (*stream_flush)(
		osalStream stream,
		os_short flags);

	osalStatus (*stream_seek)(
		osalStream stream,
		os_long *pos,
		os_short flags);

	osalStatus (*stream_write)(
		osalStream stream,
		const os_uchar *buf,
		os_memsz n,
		os_memsz *n_written,
		os_short flags);

	osalStatus (*stream_read)(
		osalStream stream,
		os_uchar *buf,
		os_memsz n,
		os_memsz *n_read,
		os_short flags);

	osalStatus (*stream_write_value)(
		osalStream stream,
		os_ushort c,
		os_short flags);

	osalStatus (*stream_read_value)(
		osalStream stream,
		os_ushort *c,
		os_short flags);

	os_long (*stream_get_parameter)(
		osalStream stream,
		osalStreamParameterIx parameter_ix);

	void (*stream_set_parameter)(
		osalStream stream,
		osalStreamParameterIx parameter_ix,
		os_long value);
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

	/** Pointers to callback functions and callback context.
	 */
	osalStreamCallbacks callbacks;
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
	osalStreamCallbacks *callbacks,
	osalStatus *status,
	os_short flags);

void osal_stream_close(
	osalStream stream);

osalStream osal_stream_accept(
	osalStream stream,
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	osalStatus *status,
	os_short flags);

osalStatus osal_stream_flush(
	osalStream stream,
	os_short flags);

osalStatus osal_stream_seek(
	osalStream stream,
	os_long *pos,
	os_short flags);

osalStatus osal_stream_write(
	osalStream stream,
	const os_uchar *buf,
	os_memsz n,
	os_memsz *n_written,
	os_short flags);

osalStatus osal_stream_read(
	osalStream stream,
	os_uchar *buf,
	os_memsz n,
	os_memsz *n_read,
	os_short flags);

osalStatus osal_stream_write_value(
	osalStream stream,
	os_ushort c,
	os_short flags);

osalStatus osal_stream_read_value(
	osalStream stream,
	os_ushort *c,
	os_short flags);

os_long osal_stream_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix);

void osal_stream_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value);

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
	os_char *parameters,
	osalStreamCallbacks *callbacks,
	osalStatus *status,
	os_short flags);

osalStatus osal_stream_default_seek(
	osalStream stream,
	os_long *pos,
	os_short flags);

osalStatus osal_stream_default_write_value(
	osalStream stream,
	os_ushort c,
	os_short flags);

osalStatus osal_stream_default_read_value(
	osalStream stream,
	os_ushort *c,
	os_short flags);

os_long osal_stream_default_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix);

void osal_stream_default_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value);

/*@}*/


#endif
