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
    eNewObjFunc nfunc)
{
    eVariable *pointer;

    /* Syncronization neeeded for eclasslist_add() function.
     */
    osal_mutex_system_lock();

#if OSAL_DEBUG
    /* Check for duplicated calls with same cid.
     */
    pointer = eglobal->classlist->firstv(cid);
    if (pointer)
    {
        osal_debug_error("eclasslist_add() called with same cid twice");
        goto getout;
    }
#endif
    /* Store pointer to class'es newobj() function.
     */
    pointer = new eVariable(eglobal->classlist, cid);
    pointer->setp(nfunc);

getout:
    /* Finished with synchronization.
     */
    osal_mutex_system_unlock();
}


/**
****************************************************************************************************

  @brief Get static constuctor function pointer by class ID.

  The eclasslist_newobj function...

  @param   cid Class ifentifier to look for.
  @return  Pointer to static constructor function, or OS_NULL if none found.

****************************************************************************************************
*/
eNewObjFunc eclasslist_newobj(
    os_int cid)
{
    eVariable *pointer;
    eNewObjFunc nfunc;

    nfunc = OS_NULL;
    osal_mutex_system_lock();

    /* Check for duplicated calls with same cid.
     */
    pointer = eglobal->classlist->firstv(cid);
    if (pointer)
    {
        nfunc = (eNewObjFunc)pointer->getp();
    }
    else 
    {
        osal_debug_error("eclasslist_newobj: Class not found.");
    }

    osal_mutex_system_unlock();
    return nfunc;
}


/**
****************************************************************************************************

  @brief Add a eobjects base classes to class list.

  The eclasslist_add_eobjects function...

****************************************************************************************************
*/
void eclasslist_add_eobjects()
{
    /* eVariable should be first to add to class list followed by then eSet and eContainer. 
       Reason is that these same classes are used to store description of classes, including
       themselves.
     */
    eVariable::setupclass(); 
    eSet::setupclass(); 
}


/**
****************************************************************************************************

  @brief Initialize class list and property sets.

  The eclasslist_initialize function must be called before any objects are created.

****************************************************************************************************
*/
void eclasslist_initialize()
{
    eglobal->root = new eContainer();
    eglobal->classlist = new eContainer(eglobal->root);
    eglobal->propertysets = new eContainer(eglobal->root);
    eglobal->empty = new eVariable();

    eclasslist_add_eobjects();
}


/**
****************************************************************************************************

  @brief Free memory allocated for class list, property sets, etc.

  The eclasslist_release function should be called when all thread except current one have 
  been terminated.

****************************************************************************************************
*/
void eclasslist_release()
{
    delete eglobal->root;
    delete eglobal->empty;
}
