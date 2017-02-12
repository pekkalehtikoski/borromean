/**

  @file    ehandleroot.cpp
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
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Initialize handle root structure.

  The ehandleroot_initialize function...

  @return  None.

****************************************************************************************************
*/
void ehandleroot_initialize()
{
    eglobal->hroot.m_nrotables = 0;
    eglobal->hroot.m_first_free = OS_NULL;
}


/**
****************************************************************************************************

  @brief Shut down handle management.

  The ehandleroot_shutdown function deletes all allocated handle tables and marks handle root
  structure unused.

  @return  None.

****************************************************************************************************
*/
void ehandleroot_shutdown()
{
    eHandleRoot
        *hroot;

    eHandleTable 
        **table;

    os_int
        i,
        n;

    hroot = &eglobal->hroot;
    table = hroot->m_table;
    n = hroot->m_nrotables;
    for (i = 0; i < n; i++) 
        delete table[i];
    hroot->m_nrotables = 0;
    hroot->m_first_free = OS_NULL;
}


/**
****************************************************************************************************

  @brief Reserve handles for thread or another root object.

  The ehandleroot_reservehandles reserves handles from common handle tables of free handles for 
  use by specific root object. TIt is recommendable to reserve set of handles rather than one at 
  a time to make threads's handle's closer to each others in memory to take better advantage of 
  processor cache.

  @param   nro_handles Number of handles to reserve, >= 1.
  @return  Pointer to first handle in linked list of allocated handles to be returned.

****************************************************************************************************
*/
eHandle *ehandleroot_reservehandles(
	e_oix nro_handles)
{
    eHandleRoot
        *hroot;

    eHandleTable
        *htable;

    eHandle 
        *newchain = OS_NULL,
        *last_h = OS_NULL,
        *h;

    hroot = &eglobal->hroot;

	/* Synchronize while handling global free handles
	 */
    os_lock();

    while (nro_handles-- > 0)
    {
        if (hroot->m_first_free == OS_NULL)
        {
            if (hroot->m_nrotables > EHANDLE_TABLE_MAX) 
            {
                osal_debug_error("Maximum eHandle limit reached");
                return OS_NULL;
            }
            htable = new eHandleTable(hroot->m_nrotables * (EHANDLE_HANDLE_MAX + 1) /* + 1 */);
			hroot->m_first_free = htable->firsthandle();
			hroot->m_table[hroot->m_nrotables++] = htable;
		}

        /* Take of from current chain.
         */
        h = hroot->m_first_free;
        hroot->m_first_free = h->right();

        /* Add to new chain
         */
        if (newchain == OS_NULL)
        {
            newchain = h;
        }
        else
        {
            last_h->setright(h);
        }
        last_h = h;
    }

	if (last_h) last_h->setright(OS_NULL);

    os_unlock();

    return newchain;
}


/**
****************************************************************************************************

  @brief Release handles from thread or another root object.

  The ehandleroot_releasehandles releases handles reserved by thread to common list of free handles
  in handle tables.

  @param   h Pointer to first handle in linked list of handles to release.
  @param   nro_handles Maximum number of handles to release, >= 1. 0 to release all handles  in 
           linked list.
  @return  Pointer to the first handle to keep allocated for thread. OS_NULL if none.

****************************************************************************************************
*/
eHandle *ehandleroot_releasehandles(
	eHandle *h,
	e_oix nro_handles)
{
	eHandle
		*first_to_keep,
		*last_to_join;

    /* Find out last handle to join to globally free handles and first handle to keep
       reserved for a root object. 
     */
	first_to_keep = h;
	last_to_join = OS_NULL;
	if (nro_handles == 0) nro_handles = ~(e_oix)0;
	while (nro_handles-- != 0 && first_to_keep)
	{
		last_to_join = first_to_keep;
        last_to_join->ucnt_mark_unused();
		first_to_keep = first_to_keep->right();
	}

	/* Synchronize while handling global free handles.
	*/
	os_lock();
	if (last_to_join) 
    {   last_to_join->setright(eglobal->hroot.m_first_free);
	    eglobal->hroot.m_first_free = h;
    }
	os_unlock();

	/* Return pointer to first eHandle to keep allocated for the thread.
 	 */
	return first_to_keep;
}
