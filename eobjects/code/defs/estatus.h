/**

  @file    estatus.h
  @brief   Enumeration of status codes.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Many eobjects library functions return status codes. Enumeration eStatus defines these
  codes.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ESTATUS_INCLUDED
#define ESTATUS_INCLUDED

/**
****************************************************************************************************

  @brief Enumeration of status codes.

  Many eobjects library functions return status codes. Enumeration eStatus defines these
  codes.

****************************************************************************************************
*/
typedef enum
{
	/** Success. The ESTATUS_SUCCESS (0) is returned when function call succeeds.
	 */
	ESTATUS_SUCCESS = 0,

    /** Unidentified failure.
	 */
	ESTATUS_FAILED = 1,

    /** General purpose FALSE.
     */
    ESTATUS_FALSE = 2,

	/** Object properties, properties not supported for the class.
	 */
    ESTATUS_NO_CLASS_PROPERTY_SUPPORT = 90,

	/** Object serialization. Writing object to stream has failed.
	 */
	ESTATUS_WRITING_OBJ_FAILED = 100,

	/** Object serialization. Reading object from stream has failed.
	 */
    ESTATUS_READING_OBJ_FAILED = 101,

	/** Names, mapping name to name space failed.
	 */
    ESTATUS_NAME_MAPPING_FAILED = 200,

	/** Names, name already mapped, skipping remapping.
	 */
    ESTATUS_NAME_ALREADY_MAPPED = 201,

    /** Properties, simpleproperty() did not find property.
	 */
    ESTATUS_NO_SIMPLE_PROPERTY_NR = 300,

    /** Reading float from stream failed
     */
    ESTATUS_STREAM_FLOAT_ERROR = 400,

	/** No new incoming connection. The stream function eStream::accept() returns this 
	    code to indicate that no new connection was accepted. 
	 */
    ESTATUS_NO_NEW_CONNECTION = 401,

    /** Special case, we received invisible flush count character which changed
        the flush count to zero (no more whole objects buffered in stream). 
        Only returned by eEnvelope::reader().
     */
    ESTATUS_NO_WHOLE_MESSAGES_TO_READ = 402,

    /** No more data available from stream. Like end of file or
     *  end of memory buffer or end of file.
     */
    ESTATUS_STREAM_END = 403
} 
eStatus;

#endif
