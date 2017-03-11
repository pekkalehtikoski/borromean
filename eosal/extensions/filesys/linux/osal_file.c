/**

  @file    filesys/linux/osal_file.c
  @brief   Basic file IO.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  File IO for linux.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/eosalx.h"



/**
****************************************************************************************************

  @name Socket data structure for Windows file encapsulation.

  The osalStream type is pointer to a stream, like stream handle. It is defined as pointer to 
  dummy structure to provide compiler type checking. This sturcture is never really allocated,
  and OSAL functions cast their own stream pointers to osalStream pointers.

****************************************************************************************************
*/
typedef struct osalSocket
{
    /** The file structure must start with file header structure. This includes generic
	    stream header, which contains parameters common to every stream. 
	 */
	osalStreamHeader hdr;

    /** Operating system's file handle.
	 */
    int handle;

    /** Stream open flags. Flags which were given to osal_file_open().
        function. 
	 */
	os_int open_flags;
}
osalFile;



/**
****************************************************************************************************

  @brief Open a file.
  @anchor osal_file_open

  The osal_file_open() function opens a file. The file can be either listening TCP
  file, connecting TCP file or UDP multicast file.

  @param  parameters file parameters, a list string or direct value.
		  Address and port to connect to, or interface and port to listen for.
          file IP address and port can be specified either as value of "addr" item
          or directly in parameter sstring. For example "192.168.1.55:20" or "localhost:12345"
          specify IPv4 addressed. If only port number is specified, which is often 
          useful for listening file, for example ":12345".
          IPv4 address is automatically recognized from numeric address like
          "2001:0db8:85a3:0000:0000:8a2e:0370:7334", but not when address is specified as string
          nor for empty IP specifying only port to listen. Use brackets around IP address
          to mark IPv6 address, for example "[localhost]:12345", or "[]:12345" for empty IP.

  @param  option Not used for files, set OS_NULL.

  @param  status Pointer to integer into which to store the function status code. Value
		  OSAL_SUCCESS (0) indicates success and all nonzero values indicate an error.
          See @ref osalStatus "OSAL function return codes" for full list.
		  This parameter can be OS_NULL, if no status code is needed. 

  @param  flags Flags for creating the file. Bit fields, combination of:
          - OSAL_STREAM_CONNECT: Connect to specified file port at specified IP address.
          - OSAL_STREAM_LISTEN: Open a file to listen for incoming connections.
          - OSAL_STREAM_UDP_MULTICAST: Open a UDP multicast file.
          - OSAL_STREAM_NO_SELECT: Open file without select functionality.
          - OSAL_STREAM_TCP_NODELAY: Disable Nagle's algorithm on TCP file.
          - OSAL_STREAM_NO_REUSEADDR: Disable reusability of the file descriptor.
          - OSAL_STREAM_BLOCKING: Open file in blocking mode.

		  See @ref osalStreamFlags "Flags for Stream Functions" for full list of stream flags.

  @return Stream pointer representing the file, or OS_NULL if the function failed.

****************************************************************************************************
*/
osalStream osal_file_open(
	os_char *parameters,
	void *option,
	osalStatus *status,
	os_int flags)
{
    osalFile *myfile = OS_NULL;
	os_memsz host_sz, sz1, sz2;
	os_int port_nr;
	osalStatus rval;
    int handle = -1;


	/* Get host name or numeric IP address and TCP port number from parameters.
       The host buffer must be released by calling os_free() function,
       unless if host is OS_NULL (unpecified).



    /* Create file.
     */
    /* handle = file(af, hints.ai_socktype, hints.ai_protocol);
    if (handle == -1)
	{
		rval = OSAL_STATUS_FAILED;
		goto getout;
    } */

    /* Allocate and clear file structure.
	 */
    myfile = (osalFile*)os_malloc(sizeof(osalFile), OS_NULL);
    if (myfile == OS_NULL)
	{
		rval = OSAL_STATUS_MEMORY_ALLOCATION_FAILED;
		goto getout;
	}
    os_memclear(myfile, sizeof(osalFile));

    /* Save file handle and open flags.
	 */
    myfile->handle = handle;
    myfile->open_flags = flags;

	/* Save interface pointer.
	 */
    myfile->hdr.iface = &osal_file_iface;

	/* Set 0 timeouts.
	 */
    myfile->hdr.write_timeout_ms = myfile->hdr.read_timeout_ms = 0;

    /* Success set status code and cast file structure pointer to stream pointer and return it.
	 */
	if (status) *status = OSAL_SUCCESS;
    return (osalStream)myfile;

getout:
    /* If we got far enough to allocate the file structure.
       Close the event handle (if any) and free memory allocated
       for the file structure.
     */
    if (myfile)
    {
        os_free(myfile, sizeof(osalFile));
    }

    /* Close file
     */    
/*     if (handle != -1)
	{
        close(handle);
    } */

	/* Set status code and return NULL pointer.
	 */
	if (status) *status = rval;
	return OS_NULL;
}


/**
****************************************************************************************************

  @brief Close file.
  @anchor osal_file_close

  The osal_file_close() function clsoes a file, which was creted by osal_file_open()
  function. All resource related to the file are freed. Any attemp to use the file after
  this call may result crash.

  @param   stream Stream pointer representing the file. After this call stream pointer will
		   point to invalid memory location.
  @return  None.

****************************************************************************************************
*/
void osal_file_close(
	osalStream stream)
{
    osalFile *myfile;
    int handle;
    char buf[64];
	int n, rval;

	/* If called with NULL argument, do nothing.
	 */
	if (stream == OS_NULL) return;

    /* Cast stream pointer to file structure pointer, lock file and get OS file handle.
	 */
    myfile = (osalFile*)stream;
    handle = myfile->handle;

    /* If file operating system file is not already closed, close now.
	 */
    if (handle != -1)
	{
        /* Mark file closed
		 */
        myfile->handle = -1;

        /* Close the file.
		 */
        if (close(handle))
		{
            osal_debug_error("closefile failed");
		}
	}

    /* Free memory allocated for file structure.
     */
    os_free(myfile, sizeof(osalFile));
}



/**
****************************************************************************************************

  @brief Flush the file.
  @anchor osal_file_flush

  The osal_file_flush() function flushes data to be written to stream.

  @param   stream Stream pointer representing the file.
  @param   flags See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.
  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_file_flush(
	osalStream stream,
	os_int flags)
{
	return OSAL_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to file.
  @anchor osal_file_write

  The osal_file_write() function writes up to n bytes of data from buffer to file.

  @param   stream Stream pointer representing the file.
  @param   buf Pointer to the beginning of data to place into the file.
  @param   n Maximum number of bytes to write. 
  @param   n_written Pointer to integer into which the function stores the number of bytes 
           actually written to file,  which may be less than n if there is not enough space
           left in the file. If the function fails n_written is set to zero.
  @param   flags Flags for the function.
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.
  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_file_write(
	osalStream stream,
	const os_uchar *buf,
	os_memsz n,
	os_memsz *n_written,
	os_int flags)
{
	int rval;
    osalFile *myfile;
    int handle;

	if (stream)
	{
        /* Cast stream pointer to file structure pointer.
		 */
        myfile = (osalFile*)stream;

        /* If nothing to write.
		 */
		if (n == 0)
		{
			*n_written = 0;
			return OSAL_SUCCESS;
		}

        /* Lock file and get OS file handle.
		 */
        handle = myfile->handle;

        /* If operating system file is already closed.
		 */
        if (handle == -1)
		{
			goto getout;
		}

        /* Limit number of bytes to write at one to 2^31
         */
        if (n > 0x7FFFFFFFFFFFFFFE) n = 0x7FFFFFFFFFFFFFFF;
        
//        rval = send(handle, buf, (int)n, MSG_NOSIGNAL);


		*n_written = rval;
        return OSAL_SUCCESS;
	}

getout:
	*n_written = 0;
    return OSAL_STATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Read data from file.
  @anchor osal_file_read

  The osal_file_read() function reads up to n bytes of data from file into buffer.

  @param   stream Stream pointer representing the file.
  @param   buf Pointer to buffer to read into.
  @param   n Maximum number of bytes to read. The data buffer must large enough to hold
		   at least this namy bytes. 
  @param   n_read Pointer to integer into which the function stores the number of bytes read, 
           which may be less than n if there are fewer bytes available. If the function fails 
		   n_read is set to zero.
  @param   flags Flags for the function, use OSAL_STREAM_DEFAULT (0) for default operation. 
           The OSAL_STREAM_PEEK flag causes the function to return data in file, but nothing
           will be removed from the file.
		   See @ref osalStreamFlags "Flags for Stream Functions" for full list of flags.

  @return  Function status code. Value OSAL_SUCCESS (0) indicates success and all nonzero values
		   indicate an error. See @ref osalStatus "OSAL function return codes" for full list.

****************************************************************************************************
*/
osalStatus osal_file_read(
	osalStream stream,
	os_uchar *buf,
	os_memsz n,
	os_memsz *n_read,
	os_int flags)
{
    osalFile *myfile;
    int handle, rval;

	if (stream)
	{
        /* Cast stream pointer to file structure pointer, lock file and get OS file handle.
		 */
        myfile = (osalFile*)stream;
        handle = myfile->handle;

        /* If operating system file is already closed.
		 */
        if (handle == -1)
		{
			goto getout;
		}

        /* Limit number of bytes to read at one to 2^31.
         */
        if (n > 0x7FFFFFFFFFFFFFFE) n = 0x7FFFFFFFFFFFFFFF;

//        rval = recv(handle, buf, (int)n, MSG_NOSIGNAL);

        /* If other end has gracefylly closed.
         */
/*        if (rval == 0)
        {
            *n_read = 0;
            return OSAL_STATUS_file_CLOSED;
        }
*/

		*n_read = rval;
		return OSAL_SUCCESS;
	}

getout:
	*n_read = 0;
    return OSAL_STATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Get file parameter.
  @anchor osal_file_get_parameter

  The osal_file_get_parameter() function gets a parameter value.

  @param   stream Stream pointer representing the file.
  @param   parameter_ix Index of parameter to get.
		   See @ref osalStreamParameterIx "stream parameter enumeration" for the list.
  @return  Parameter value.

****************************************************************************************************
*/
os_long osal_file_get_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix)
{
	/* Call the default implementation
	 */
	return osal_stream_default_get_parameter(stream, parameter_ix);
}


/**
****************************************************************************************************

  @brief Set file parameter.
  @anchor osal_file_set_parameter

  The osal_file_set_parameter() function gets a parameter value.

  @param   stream Stream pointer representing the file.
  @param   parameter_ix Index of parameter to get.
		   See @ref osalStreamParameterIx "stream parameter enumeration" for the list.
  @param   value Parameter value to set.
  @return  None.

****************************************************************************************************
*/
void osal_file_set_parameter(
	osalStream stream,
	osalStreamParameterIx parameter_ix,
	os_long value)
{
	/* Call the default implementation
	 */
	osal_stream_default_set_parameter(stream, parameter_ix, value);
}


#if OSAL_FUNCTION_POINTER_SUPPORT

/** Stream interface for OSAL files. This is structure osalStreamInterface filled with
    function pointers to OSAL files implementation.
 */
osalStreamInterface osal_file_iface
 = {osal_file_open,
    osal_file_close,
    osal_stream_default_accept,
    osal_file_flush,
	osal_stream_default_seek,
    osal_file_write,
    osal_file_read,
	osal_stream_default_write_value,
	osal_stream_default_read_value,
    osal_file_get_parameter,
    osal_file_set_parameter,
    osal_stream_default_select};

#endif

