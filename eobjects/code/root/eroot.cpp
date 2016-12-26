/**

  @file    eroot.cpp
  @brief   Root helper object.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

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
eRoot::eRoot(
	eObject *parent,
	e_oid oid,
	os_int flags)
	: eObject(parent, oid, flags)
{
	/* No free handles reserved yet. Set number of handles to reserver to zero.
	   This means that empty default is used.
	 */
	m_first_free_handle = OS_NULL;
	// m_reserved_handle_count = 0;
	m_reserve_at_once = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eRoot::~eRoot()
{
}


/* Reserve and initialize handle for object obj.
    */
void eRoot::newhandle(
	eObject *obj,
	eObject *parent,
	e_oid oid,
	os_int flags)
{
	eHandle
		*handle;

	/* If we have no free handles, allocate more. Incse number of handles to 
	   allocate at once. 
	 */
	if (m_first_free_handle == OS_NULL)
	{
		if (m_reserve_at_once < 4)
		{
			m_reserve_at_once = 4;
		}
		else if (m_reserve_at_once < 16)
		{
			m_reserve_at_once = 16;
		}
		else if (m_reserve_at_once <= 256)
		{
			m_reserve_at_once *= 2;
		}
		m_first_free_handle = ehandleroot_reservehandles(m_reserve_at_once);
		// m_reserved_handle_count += m_reserve_at_once;
	}

	/* Remove handle to use from chain of free handles.
	 */
	handle = m_first_free_handle;
	m_first_free_handle = handle->right();

	/* Save object identifier, clear flags, mark new node as red,
	   join to tree hierarchy, no children yet.
	 */
	handle->clear(obj, oid, flags);
	obj->mm_handle = handle;

	/* Save parent object pointer. If parent object is given, join the new object
	to red black tree of parent's children.
	*/
	if (parent) 
	{
		parent->mm_handle->rbtree_insert(handle);
	}
}


/* Close handle of object obj.
 */
void eRoot::closehandle(
    eObject *obj)
{
	if (mm_parent && (flags() & EOBJ_FAST_DELETE) == 0)
	{
//		m_parent->rbtree_remove(this);
	}
}
