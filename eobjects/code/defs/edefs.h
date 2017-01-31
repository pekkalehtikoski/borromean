/**

  @file    edefs.h
  @brief   eobject general defines.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EDEFS_INCLUDED
#define EDEFS_INCLUDED

/**
****************************************************************************************************

  @name eobjects specific types.

  The ...


****************************************************************************************************
*/
/*@{*/

/** Object index.
	*/
typedef os_uint e_oix;

/** Object identifier.
	*/
typedef os_int e_oid;


/**
****************************************************************************************************

  @name Default build defines.

  This file contains platform defaults for defines of optional eobject features. These can
  be changed for yupir build in global makefile, etc. For initial cmake build this place would
  be build/cmake/e-build-defs.txt, for other build tools the file to edit will be different.

  These defines must be the same for compiling whole eobjects library and any software which 
  makes use of the eobjects library.

****************************************************************************************************
*/
/*@{*/

/** Sopport JSON format object serialization. Enables reading and writing object trees as JSON.
 */
#ifndef E_SUPPROT_JSON
#define E_SUPPROT_JSON 0
#if OSAL_WINDOWS
#undef E_SUPPROT_JSON
#define E_SUPPROT_JSON 1
#endif
#if OSAL_LINUX
#undef E_SUPPROT_JSON
#define E_SUPPROT_JSON 1
#endif
#endif

/*@}*/

#endif
