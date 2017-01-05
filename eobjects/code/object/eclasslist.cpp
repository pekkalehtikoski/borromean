/**

  @file    eclasslist.cpp
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
#include "eobjects/eobjects.h"

static eClassList eclasslist_buf;

eClassList *eclasslist = &eclasslist_buf;


/**
****************************************************************************************************

  @brief Add a class to class list.

  Class list contains pointers to static constructor for the class by class ID index.
  It is used to constructr objects so that class can be a parameter.

  @return  None.

****************************************************************************************************
*/
void eclasslist_add(
    os_int cid, 
    eNewObjFunc nfunc,
    eSetupClassFunc sfunc)
{
//    eclasslist->func[0] = eVariable::newobj;

    if (cid > 0 && cid <= ECLASSID_MAX)
    {
//        eclasslist->nfunc[cid] = nfunc;
//        eclasslist->sfunc[cid] = sfunc;
    }
}


/**
****************************************************************************************************

  @brief Add a eobjects base classes to class list.

  The eclasslist_add_eobjects function...

  @return  None.

****************************************************************************************************
*/
void eclasslist_add_eobjects()
{
    if (eclasslist == OS_NULL)
    {
        eclasslist = &eclasslist_buf;

        /* eVariable must be first to add to class list, since it is used to describe properties
           for the class.
         */
        eclasslist_add(ECLASSID_VARIABLE, (eNewObjFunc)eVariable::newobj, (eSetupClassFunc)eVariable::setupclass); 
    }
}


/**
****************************************************************************************************

  @brief Get static constuctor function pointer by class ID.

  The eclasslist_get_func function...

  @param   cid Class ifentifier to look for.
  @return  Pointer to static constructor function, or OS_NULL if none found.

****************************************************************************************************
*/
eNewObjFunc eclasslist_newobj_func(
    os_int cid)
{
    eNewObjFunc func;

    func = OS_NULL;
    osal_mutex_system_lock();

/*     item = eglobal->classlist->first(cid);
    if (item) func = eclasslist->func[cid]; */

    osal_mutex_system_unlock();

    return func;
}

/* Initialize class list.
   Must be called before any threads are created.
 */
void eclasslist_initialize()
{
    eglobal->classlist = new eContainer();

    eclasslist_add_eobjects();
}

/* release class list.
   Must be called when all thread except current one have been terminated.
 */
void eclasslist_release()
{
    delete eglobal->classlist;
}
