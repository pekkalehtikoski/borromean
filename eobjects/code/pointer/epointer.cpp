/**

  @file    epointer.cpp
  @brief   Automatic object pointers.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    14.5.2015

  Automatic object pointer is a pointer which detects if object pointing to it is deleted.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
ePointer::ePointer(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
    os_memclear(&m_ref, sizeof(ePointerRef));
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
ePointer::~ePointer()
{
}


/**
****************************************************************************************************

  @brief Set object pointer.

  The object pointer is much like C pointer, but it is known if the pointer object is deleted.

  @param  ptr Pointer to object to save.
  @return None.

****************************************************************************************************
*/
void ePointer::set(
    eObject *ptr)
{
    eHandle *handle;

    /* If no change, do nothing.
     */
    if (ptr == OS_NULL) 
    {
        os_memclear(&m_ref, sizeof(ePointerRef));
        return;
    }

    /* If object to point to is not part of tree, make it to be root of the tree.
     */
    handle = ptr->handle();
    if (handle == OS_NULL)
    {
        ptr->makeroot(EOID_ITEM, EOBJ_DEFAULT);
        handle  = ptr->handle();
    }

    m_ref.ref.oix = handle->oix();
    m_ref.ref.ucnt = handle->ucnt();
}


/**
****************************************************************************************************

  @brief Get object pointer.

  The object pointer is much like C pointer, but it is known if the pointer object is deleted.

  @param  ptr Pointer to object to save.
  @return Pointer to object. OS_NULL if not set or pointed object has been deleted.

****************************************************************************************************
*/
eObject *ePointer::get()
{
    eHandle *handle;

    /* If not set.
     */
    if (m_ref.ref.ucnt <= 0) return OS_NULL;
    
    handle = eget_handle(m_ref.ref.oix);
    if (m_ref.ref.ucnt != handle->m_ucnt)
    {
        return OS_NULL;
    }

    return handle->object();
}
