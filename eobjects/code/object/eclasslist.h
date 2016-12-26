/**

  @file    eclasslist.h
  @brief   Class list.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Maintain list of classes which can be created dynamically by class ID.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECLASSLIST_INCLUDED
#define ECLASSLIST_INCLUDED

typedef eObject *(*eNewObjFunc)(
    eObject *parent,
    e_oid oid,
	os_int flags);

/**
****************************************************************************************************

  @brief Class list structure.

  X...


****************************************************************************************************
*/
typedef struct eClassList
{
    eNewObjFunc func[ECLASSID_MAX+1];
}
eClassList;

/* Add class to class list.
 */
void eclasslist_add(
    os_int cid, 
    eNewObjFunc func);

/* Add a eobjects base classes to class list.
 */
void eclasslist_add_eobjects();

/* Get static constuctor function pointer by class ID.
 */
eNewObjFunc eclasslist_get_func(
    os_int cid);

#endif
