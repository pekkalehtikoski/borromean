/**

  @file    ehandleroot.h
  @brief   Global handle manager object.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The handle root keeps track of handle tables and global free handles in them. Handles
  can be reserved by thread or an another root object. Handle root state is stored in 
  eHandleRoot structure within eglobals.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EHANDLEROOT_INCLUDED
#define EHANDLEROOT_INCLUDED

class eHandleTable;

/** Maximum index for eHandleRoot's m_table array.
 */
#define EHANDLE_TABLE_MAX 0x1FFF

/**
****************************************************************************************************

  @brief Handle root class.

  The handle eHandleRoot object manages holds pointers to allocated handle tables and maintains
  list of free handles, which are not reserved for any root object. There is one handle root
  object per process.

****************************************************************************************************
*/
typedef struct eHandleRoot
{
    /** Array of handle table pointers. Index is from 0 to EHANDLE_TABLE_MAX.
     */
    eHandleTable *m_table[EHANDLE_TABLE_MAX+1];

    /** Number of allocated handle tables.
     */
    os_int m_nrotables;

    /* First free common handle (not reserved for any root object) in handle tables.
     */
    eHandle *m_first_free;
}
eHandleRoot;


/**
****************************************************************************************************

  @name Handle root functions.

  Function of the handle root is to allocate and manage handle tables, and allow (thread) root 
  objects to reserve/release handles for use.

****************************************************************************************************
*/
/*@{*/

/* Initialize handle tables.
 */
void ehandleroot_initialize();

/* Delete handle tables.
 */
void ehandleroot_shutdown();

/* Reserve handles for thread or another root object.
 */
eHandle *ehandleroot_reservehandles(
    e_oix nro_handles);

/* Release handles from thread or another root object.
 */
eHandle *ehandleroot_releasehandles(
	eHandle *h,
	e_oix nro_handles);


/*@}*/

#endif



