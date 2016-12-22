/**

  @file    ehandleroot.cpp
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

  The ehandleroot_reservehandles reserves handles for use by thread. It is recommendable to
  reserve set of handles rather than one at a time to make threads's handle's closer to each
  others in memory to take better rdvantage of processor cache.

  @param   nro_handles Number of handles to reserve, >+ 1.
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
    osal_mutex_system_lock();

    while (nro_handles-- > 0)
    {
        if (hroot->m_first_free == OS_NULL)
        {
            if (hroot->m_nrotables > EHANDLE_TABLE_MAX) 
            {
                osal_debug_error("Maximum eHandle limit reached");
                return OS_NULL;
            }
            htable = new eHandleTable(hroot->m_nrotables * (EHANDLE_HANDLE_MAX + 1) + 1);
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

    osal_mutex_system_unlock();

    return newchain;
}


// nro_handles = 0, release all
eHandle *ehandleroot_releasehandles(
	eHandle *h,
	e_oix nro_handles)
{
	eHandle
		*first_to_keep,
		*last_to_join;

	first_to_keep = h;
	last_to_join = OS_NULL;
	if (nro_handles == 0) nro_handles = ~(e_oix)0;
	while (nro_handles-- != 0 && first_to_keep)
	{
		last_to_join = first_to_keep;
		first_to_keep = first_to_keep->right();
	}

	/* Synchronize while handling global free handles
	*/
	osal_mutex_system_lock();
	if (last_to_join) last_to_join->setright(eglobal->hroot.m_first_free);
	eglobal->hroot.m_first_free = h;
	osal_mutex_system_unlock();

	/* Return pointer to first ehanle to keep allocated for the thread
 	 */
	return first_to_keep;
}


/* THIS MUST BE AS HAST FUNCTION AS POSSIBLE
 */
inline eHandle *eget_handle(
    e_oix oix)
{
	oix++;
    return eglobal->hroot.m_table[oix >> EHANDLE_HANDLE_BITS]->m_handle + (oix & EHANDLE_TABLE_MAX);
}

inline eObject *eget_object(
    e_oix oix)
{
/* 
    handletable oix >> EHANDLE_HANDLE_BITS;
    if (handletable >= m_nrotables) return OS_NULL;
    return eglobal->handleroot.m_table[oix >> EHANDLE_HANDLE_BITS]->m_handle[oix & EHANDLE_TABLE_MAX].m_object;
*/
return 0;
}


