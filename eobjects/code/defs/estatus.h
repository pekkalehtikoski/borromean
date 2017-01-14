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

	/** Propertie, simpleproperty() did not find property.
	 */
    ESTATUS_NO_SIMPLE_PROPERTY_NR = 300,

    /** Reading float from stream failed
     */
    ESTATUS_STREAM_FLOAT_ERROR = 400

} 
eStatus;

#endif
