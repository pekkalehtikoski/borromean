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
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    m_my_pair = OS_NULL;
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
    if (m_my_pair) set(OS_NULL);
}


/**
****************************************************************************************************

  @brief Set automatic pointer.

  X...

  @param  ptr Pointer to object to refer to.
  @return None.

****************************************************************************************************
*/
void ePointer::set(
    eObject *ptr)
{
    /* If no change, do nothing.
     */
    if (ptr == OS_NULL && m_my_pair == OS_NULL) return;

    /* Disconnect existing ePointer object pair.
     */
    if (m_my_pair)
    {
        /* If this is pointer target side, this cannot no longer be
           referenced. 
         */
        if (oid() == EOID_PPTR_TARGET)
        {
            m_my_pair->m_my_pair = OS_NULL;
        }

        /* This is active side end of ePointer pair.
         */
        else 
        {
            /* If right pair is already there, do nothing.
             */
            if (m_my_pair->parent() == ptr) return;
            
            /* Delete ePointer object at target.
             */
            delete m_my_pair;
        }

        /* Now pair detached.
         */
        m_my_pair = OS_NULL;
    }

    /* No new pair. If ptr given as argument is also NULL, we do nothing.
       Also set() function can be used on target end only to detach
       pointer pair.
     */
    if (ptr == OS_NULL || oid() == EOID_PPTR_TARGET) return;

    /* Create ePointer object to target side and make pair.
     */
    m_my_pair = new ePointer(ptr, EOID_PPTR_TARGET);
    m_my_pair->setflags(EOBJ_IS_ATTACHMENT);
    m_my_pair->m_my_pair = this;
}

