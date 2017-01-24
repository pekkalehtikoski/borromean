/**

  @file    eendpoint.cpp
  @brief   End point class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  The eEndPoint is socket end point listening to specific TCP port for new connections.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"

/* End point property names.
 */
os_char
    eendpp_ipaddr[] = "ipaddr";


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEndPoint::eEndPoint(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEndPoint::~eEndPoint()
{
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
void eEndPoint::setupclass()
{
    const os_int cls = ECLASSID_ENDPOINT;

    /* Synchwonize, add the class to class list and properties to property set.
     */
    osal_mutex_system_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj);
    addproperty(cls, EENDPP_IPADDR, eendpp_ipaddr, EPRO_PERSISTENT|EPRO_SIMPLE, "IP");
    osal_mutex_system_unlock();
}


/**
****************************************************************************************************

  @brief Called to inform the class about property value change (override).

  The onpropertychange() function is called when class'es property changes, unless the
  property is flagged with EPRO_NOONPRCH. 
  If property is flagged as EPRO_SIMPLE, this function shuold save the property value
  in class members and and return it when simpleproperty() is called.

  Notice for change logging: Previous value is still valid when this function is called.
  You can get the old value by calling property() function inside onpropertychange()
  function.

  @param   propertynr Property number of changed property.
  @param   x Variable containing the new value.
  @param   flags
  @return  None.

****************************************************************************************************
*/
void eEndPoint::onpropertychange(
    os_int propertynr, 
    eVariable *x, 
    os_int flags)
{
    switch (propertynr)
    {
        case EENDPP_IPADDR:
            m_ipaddr->setv(x);
            break;

        default:
            /* eObject::onpropertychange(propertynr, x, flags); */
            break;
    }
}


/**
****************************************************************************************************

  @brief Get value of simple property (override).

  The simpleproperty() function stores current value of simple property into variable x.

  @param   propertynr Property number to get.
  @param   x Variable info which to store the property value.
  @return  If fproperty with property number was stored in x, the function returns 
           ESTATUS_SUCCESS (0). Nonzero return values indicate that property with
           given number was not among simple properties.

****************************************************************************************************
*/
eStatus eEndPoint::simpleproperty(
    os_int propertynr, 
    eVariable *x)
{
    switch (propertynr)
    {
        case EENDPP_IPADDR:
            x->setv(m_ipaddr);
            break;
   
        default:
            /* return eObject::simpleproperty(propertynr, x); */
            return ESTATUS_NO_SIMPLE_PROPERTY_NR;
    }
    return ESTATUS_SUCCESS;
}
