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

	/** Object serialization. Writing object to stream has failed.
	 */
	ESTATUS_WRITING_OBJ_FAILED = 100,

	/** Object serialization. Reading object from stream has failed.
	 */
    ESTATUS_READING_OBJ_FAILED = 101,

} 
eStatus;

#endif
