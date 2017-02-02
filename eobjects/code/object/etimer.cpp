/**

  @file    etimer.cpp
  @brief   Timer implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Object can enable or disable receiving ECMD_TIMER by calling base class'es eObject::timer()
  function. Timer base precision is 40ms, which is intended to be cast enough for animating 
  user interface objects at rate of 25Hz. When more precise timing is needed, it should be
  implemented by other means (for example by using loop containing os_sleep and alive() calls). 
  
  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Enable or disable timer for this object.

  If the eObject::timer() function is called with nonzero period_ms argument, timer is enabled.
  Enabling timer means that object will receive periodic ECMD_TIMER messages. Calling
  the function with zero argument, will disable the timer. 
  
  @param  period_ms How often to receive ECMD_RUN message in milliseconds, or zero to disable
          the timer. This will be rounded to 40 ms precision.

  @return None.

****************************************************************************************************
*/
eStatus eObject::timer(
    os_long period_ms) 
{
}


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eTimer::eTimer(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eThread(parent, oid, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eTimer::~eTimer()
{
    close();
}


/**
****************************************************************************************************

  @brief Add eVariable to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds eVariable to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property stet stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eTimer::setupclass()
{
    const os_int cls = ECLASSID_TIMER;
    eVariable *p;

    /* Synchwonize, add the class to class list and properties to property set.
     */
    osal_mutex_system_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eTimer");
    addproperty(cls, EENDPP_CLASSID, eendpp_classid, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "class ID");
    addproperty(cls, EENDPP_IPADDR, eendpp_ipaddr, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "IP");
    p = addpropertyl(cls, EENDPP_ISOPEN, eendpp_isopen, 
        EPRO_NOONPRCH, "is open", OS_FALSE);
    p->setpropertys(EVARP_ATTR, "rdonly;chkbox");
    osal_mutex_system_unlock();
}
