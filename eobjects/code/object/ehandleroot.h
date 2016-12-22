/**

  @file    ehandleroot.h
  @brief   Global handle manager object.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The handle manager is keeps track of handle tables and globally free handles in them. Handles
  can be reserved by thread or an another roobot object. Handle manager state is stored in 
  eHandleRoot structure within eglobals.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EHANDLEROOT_INCLUDED
#define EHANDLEROOT_INCLUDED

class eStream;
class eVariable;
class eNameSpace;
class eName;
class eHandleTable;

/**
****************************************************************************************************

  @name Handle tree parmateres.

  Specifies organization of object tree. Optimal values depend on system resources and
  memory use of the application. Anyhow this is not precise, when we have these in right
  ballpark, we are fine.

****************************************************************************************************
*/
/*@{*/
/** Maximum index for eHandleRoot's m_table array.
 */
#define EHANDLE_TABLE_MAX 0x1FFF


/**
****************************************************************************************************

  @brief Handle class.

  The eHandle is class for indexing the thread's object tree.
  networked objects.

****************************************************************************************************
*/
typedef struct eHandleRoot
{
    eHandleTable *m_table[EHANDLE_TABLE_MAX+1];

    os_int m_nrotables;

    eHandle *m_first_free;
//    eHandle *m_last_free;
}
eHandleRoot;

/* Initialize handle tables.
 */
void ehandleroot_initialize();

/* Delete handle tables.
 */
void ehandleroot_shutdown();

eHandle *ehandleroot_reservehandles(
    e_oix nro_handles);

eHandle *ehandleroot_releasehandles(
	eHandle *h,
	e_oix nro_handles);

/* THIS MUST BE AS FAST FUNCTION AS POSSIBLE
 */
inline eHandle *ehandle_find(
    e_oix oix);

inline eObject *ehandle_find_object(
    e_oix oix);

#endif



