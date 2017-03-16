/**

  @file    epropertybinding.cpp
  @brief   Simple object propertybinding.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The propertybinding object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Property binding class constructor.

  Clear member variables. 
  @return  None.

****************************************************************************************************
*/
ePropertyBinding::ePropertyBinding(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eBinding(parent, id, flags)
{
    /* Clear member variables.
     */
    m_propertyname = OS_NULL;
    m_propertynamesz = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  If connected, disconnect binding. Release all resources allocated for the binging.
  @return  None.

****************************************************************************************************
*/
ePropertyBinding::~ePropertyBinding()
{
    set_propertyname(OS_NULL);
}


/**
****************************************************************************************************

  @brief Clone object

  The ePropertyBinding::clone function clones and object including object's children. 
  Names will be left detached in clone.

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *ePropertyBinding::clone(
    eObject *parent, 
    e_oid id,
    os_int aflags)
{
    eObject
        *clonedobj,
        *child;

    clonedobj = new ePropertyBinding(parent, id == EOID_CHILD ? oid() : id, flags());

    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isclonable())
        {
            child->clone(clonedobj, child->oid(), aflags);
        }
    }

    return clonedobj;
}


/**
****************************************************************************************************

  @brief Add the class to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds the class to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void ePropertyBinding::setupclass()
{
    const os_int cls = ECLASSID_PROPERTY_BINDING;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "ePropertyBinding");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Write property binding content to stream.

  The ePropertyBinding::writer() function serializes the property binding to stream. This writes
  only the content, use eObject::write() to save also class information, attachements, etc.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus ePropertyBinding::writer(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Increment if new serialized items are added to the object,
       and check for new version's items in read() function.
     */
    const os_int version = 0;
    eObject *child;

	/* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Write child count to stream (no attachments).
     */
    if (*stream << childcount())  goto failed;

    /* Write childern (no attachments).
     */
    for (child = first(); child; child = child->next())
    {
        child->write(stream, flags);
    }

	/* End the object.
     */
    if (stream->write_end_block()) goto failed;

    /* Object succesfully written.
     */
    return ESTATUS_SUCCESS;

    /* Writing object failed.
     */
failed:
    return ESTATUS_WRITING_OBJ_FAILED;
}


/**
****************************************************************************************************

  @brief Read property binding content from stream.

  The ePropertyBinding::reader() function reads serialized propertybinding from stream. 
  This function reads only the object content. To read whole object including attachments, 
  names, etc, use eObject::read().
  
  @param  stream The stream to read from.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus ePropertyBinding::reader(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Used to check which versions item's are in serialized data.
     */
    os_int version;
    os_long count;

	/* Read object start mark and version number.
     */
    if (stream->read_begin_block(&version)) goto failed;

    /* Read child count from (no attachments).
     */
    if (*stream >> count)  goto failed;

    /* Read children.
     */
    while (count-- > 0)
    {
        read(stream, flags);
    }

	/* End the object.
     */
    if (stream->read_end_block()) goto failed;

    /* Object succesfully read.
     */
    return ESTATUS_SUCCESS;

    /* Reading object failed.
     */
failed:
    return ESTATUS_READING_OBJ_FAILED;
}


/**
****************************************************************************************************

  @brief Function to process incoming messages. 

  The ePropertyBinding::onmessage function handles messages received by object. If this function
  doesn't process message, it calls parent class'es onmessage function.
  
  @param   envelope Message envelope. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void ePropertyBinding::onmessage(
    eEnvelope *envelope) 
{
    /* If at final destination for the message.
     */
    if (*envelope->target()=='\0')
    {
        switch (envelope->command())
        {
            case ECMD_BIND_REPLY:
                cbindok(this, envelope);
                return;

            case ECMD_UNBIND:
            case ECMD_SRV_UNBIND:
            case ECMD_NO_TARGET:
                if (m_bflags & EBIND_CLIENT)
                {
                    disconnect(OS_TRUE);
                }
                else
                {
                    delete this;
                }
                return;

            case ECMD_FWRD:
                update(envelope);
                return;
    
            case ECMD_ACK:
                ack(envelope);
                return;

            case ECMD_REBIND:
                bind2(OS_NULL);
                return;
        }
    }

    /* Call parent class'es onmessage. 
     */
    eBinding::onmessage(envelope);
}


/**
****************************************************************************************************

  @brief Bind this object's property to remote property.

  The eObject::bind() function creates binding to remote property. When two variables are bound
  together, they have the same value. When the other changes, so does the another. Bindings
  work over messaging, thus binding work as well between objects in same thread or objects in 
  different computers.
  
  @param  localpropertyno This object's propery number to bind.
  @param  remotepath Path to remote object to bind to.
  @param  remoteproperty Name of remote property to bind. If OS_NULL variable value
          is assumed.
  @param  bflags Combination of EBIND_DEFAULT (0), EBIND_CLIENTINIT, EBIND_NOFLOWCLT
          and EBIND_METADATA. 
          - EBIND_DEFAULT:  bind without special options.
          - EBIND_CLIENTINIT: Local property value is used as initial value. Normally
            remote end's value is used as initial value.
          - EBIND_NOFLOWCLT: Disable flow control. Normally if property value changes
            faster than it can be transferred, some values are skipped. If EBIND_NOFLOWCLT
            flag is given, it disables flow control and every value is transferred without
            any limit to buffered memory use.
          - EBIND_METADATA: If meta data, like text, unit, attributes, etc exists, it is 
            also transferred from remote object to local object.
          - EBIND_ATTR: Bind also attributes (subproperties like "x.min").
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::bind(
    os_int localpropertynr,
    const os_char *remotepath,
    const os_char *remoteproperty,
    os_int bflags)
{
    /* Save bind parameters and flags.
     */
    set_propertyname(remoteproperty);
    m_localpropertynr = localpropertynr;
    m_bflags = bflags | EBIND_CLIENT;

    bind2(remotepath);
}

/* If remotepath is OS_NULL last used name will be preserved/
*/
void ePropertyBinding::bind2(
    const os_char *remotepath)
{
    eSet *parameters;
    eVariable x;

    /* Get parameters from derived class and add flags to parameters.
     */
    parameters = new eSet(this);
    parameters->setl(E_BINDPRM_FLAGS, m_bflags & EBIND_SER_MASK);
    parameters->sets(E_BINDPRM_PROPERTYNAME, m_propertyname);

    /* If this client is master, get property value.
     */
    if (m_bflags & EBIND_CLIENTINIT)
    {
        if (!binding_getproperty(&x))
        {
#if OSAL_DEBUG
            osal_debug_error("bind(): Unknown property number");
#endif
            return;
        }
        parameters->set(E_BINDPRM_VALUE, &x);
    }

    /* If we are binding attributes like "x.min", get these.
     */
    if (m_bflags & EBIND_ATTR)
    {
        if (listattr(m_localpropertynr, &x))
        {
            parameters->set(E_BINDPRM_ATTRLIST, &x);
        }
    }

    /* Call base class to do binding.
     */
    eBinding::bind_base(remotepath, parameters);
}


/**
****************************************************************************************************

  @brief Create server end property binding.

  The ePropertyBinding::srvbind() function...

  @param  envelope Recetved ECMD_BIND command envelope.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::srvbind(
    eObject *obj,
    eEnvelope *envelope)
{
    eSet *parameters, *reply;
    eVariable v;

    parameters = eSet::cast(envelope->content());
    if (parameters == OS_NULL)
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: no content");
#endif
        goto notarget;
    }

    /* Get property name. 
     */
    if (!parameters->get(E_BINDPRM_PROPERTYNAME, &v)) 
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: Property name missing");
#endif
        goto notarget;
    }

    /* Convert property name to property number (-1 = unknown property).
     */
    m_localpropertynr = obj->propertynr(v.gets());
    if (m_localpropertynr < 0)
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: Property name unknwon");
        osal_debug_error(v.gets());
#endif
        goto notarget;
    }

    /* Set flags. Set EBIND_INTERTHREAD if envelope has not been moved from thread to another.
     */
    m_bflags = (os_short)parameters->getl(E_BINDPRM_FLAGS);
    if (envelope->mflags() & EMSG_INTERTHREAD)
    {
        m_bflags |= EBIND_INTERTHREAD;
    }

    /* If subproperties are requested, list ones matching in both ends.
       Store initial property value, unless clientmaster.
     */
    reply = new eSet(this);
    /* if (m_flags & ATTR)
    {
        
xxxx

    } */

    /* If this client is nor master at initialization, get property value.
     */
    if ((m_bflags & EBIND_CLIENTINIT) == 0)
    {
        binding_getproperty(&v);
        reply->set(E_BINDPRM_VALUE, &v);
    }
    else
    {
        parameters->get(E_BINDPRM_VALUE, &v);
        binding_setproperty(&v);
    }

    /* Complete the server end of binding and return.  
     */
    srvbind_base(envelope, reply);
    return;

notarget:
    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, EMSG_DEFAULT);
    }
}


/**
****************************************************************************************************

  @brief Complete property binding at client end.

  The ePropertyBinding::cbindok() function...

  @param  obj Pointer to object being bound.
  @param  envelope The enveloped returned from server end as ECMD_BIND_REPLY.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::cbindok(
    eObject *obj,
    eEnvelope *envelope)
{
    eSet *parameters;
    eVariable v;

    parameters = eSet::cast(envelope->content());
    if (parameters == OS_NULL)
    {
#if OSAL_DEBUG
        osal_debug_error("cbindok() failed: no content");
#endif
        goto notarget;
    }

    /* If this server side is master at initialization, get property value.
     */
    if ((m_bflags & EBIND_CLIENTINIT) == 0)
    {
        parameters->get(E_BINDPRM_VALUE, &v);    
        binding_setproperty(&v);
    }

notarget:

    /* Call base class to complete the binding.
     */
    cbindok_base(envelope);
}


/**
****************************************************************************************************

  @brief Mark property value changed.

  The ePropertyBinding::changed function marks a property value changed, so that it needs
  to forwarded. The function forwards the property value immediately, if flow control allows. 
  Otherwise the property just remain marked to be forwarded.
  If property number given as argument is not for this binding, do nothing.

  @param propertynr Property number of the changed property.
  @param x Optional property value, used to save requerying it in common case.
  
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::changed(
    os_int propertynr,
    eVariable *x,
    os_boolean delete_x)
{
    /* If not for this property, do nothing.
     */
    if (propertynr != m_localpropertynr) return;

    /* Mark property value, etc changed. Forward immediately, if binding if flow
       control does not block it.
     */
    setchanged();
    forward(x, delete_x);
}


/**
****************************************************************************************************

  @brief Forward property value trough binding.

  The forward function sends value of a property if flow control allows. 
  
  @param  x Variable containing value, if available.
  @param  delete_x Flag weather value should be deleted.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::forward(
    eVariable *x,
    os_boolean delete_x)
{
    eVariable *tmp;

    if (forwardnow())
    {

        if (x == OS_NULL)
        {
            tmp = new eVariable;
            binding_getproperty(tmp);

            message(ECMD_FWRD, m_bindpath, OS_NULL, tmp, 
                EMSG_DEL_CONTENT /* EMSG_NO_ERROR_MSGS */);
        }
        else
        {
            /* Send data as ECMD_FWRD message.
             */
            message(ECMD_FWRD, m_bindpath, OS_NULL, x, 
                delete_x ? EMSG_DEL_CONTENT : EMSG_DEFAULT  /* EMSG_NO_ERROR_MSGS */);
            x = OS_NULL;
        }

        /* Clear changed bit and increment acknowledge count.
         */
        forwarddone();
    }

    if (delete_x && x) 
    {
        delete x;
    }
}


/**
****************************************************************************************************

  @brief Property value has been received from binding.

  The ePropertyBinding::update function...
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::update(
    eEnvelope *envelope)
{
    eVariable *x;

    x = eVariable::cast(envelope->content());
    binding_setproperty(x);
    sendack(envelope);
}


/**
****************************************************************************************************

  @brief Send acknowledge.

  The sendack function.
  
  @param  envelope Message envelope from server binding.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::sendack(
    eEnvelope *envelope)
{
    sendack_base(envelope);

    /* If this is server, if m_ackcount is not zero, mark changed.
     */
    if ((m_bflags & EBIND_CLIENT) == 0 && m_ackcount)
    {
        setchanged();
    }
}


/**
****************************************************************************************************

  @brief Acknowledge received.

  The ack function decrements acknowledge wait count and tries to send again.
  
  @param  envelope Message envelope from server binding.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::ack(
    eEnvelope *envelope)
{
    ack_base(envelope);
}


/**
****************************************************************************************************

  @brief Save remote property name.

  The ePropertyBinding::set_propertyname() releases current m_propertyname and stores 
  propertyname given as argument. If propertyname is OS_NULL, memory is just freeed.

  @param  propertyname Pointer to object path.
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::set_propertyname(
    const os_char *propertyname)
{
    if (m_propertyname)
    {
        os_free(m_propertyname, m_propertynamesz);
        m_propertyname = OS_NULL;
        m_propertynamesz = 0;
    }

    if (propertyname)
    {
        m_propertynamesz = (os_short)os_strlen(propertyname);
        m_propertyname = os_malloc(m_propertynamesz, OS_NULL);
        os_memcpy(m_propertyname, propertyname, m_propertynamesz);
    }
}


/**
****************************************************************************************************

  @brief Save property value.

  The binding_setproperty() is used to set a property of bound object.

  @param  x Variable holding property value to set.
  @return OS_TRUE if successfull.

****************************************************************************************************
*/
os_boolean ePropertyBinding::binding_setproperty(
    eVariable *x)
{
    eObject *obj;

    obj = grandparent();
    if (obj == OS_NULL) return OS_FALSE;
    
    /* Set property value.
     */
    obj->setpropertyv(m_localpropertynr, x, this);

    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Get property value.

  The binding_getproperty() function gets property value of bound object.

  @param  x Variable where to store property value.
  @return OS_TRUE if successfull.

****************************************************************************************************
*/
os_boolean ePropertyBinding::binding_getproperty(
    eVariable *x)
{
    eObject *obj;

    obj = grandparent();
    if (obj == OS_NULL) return OS_FALSE;
    
    /* Set property value.
     */
    obj->propertyv(m_localpropertynr, x);

    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief List attributes (subproperties like "x.min") for the property.

  The ePropertyBinding::listattr() function...

  @param  x Variable where to store attribute list.
  @return OS_TRUE if successfull.

****************************************************************************************************
*/
os_boolean ePropertyBinding::listattr(
    os_int propertynr,
    eVariable *x)
{
    eContainer *propertyset;
    eVariable *propertyvar;
    eObject *obj;

    /* Get property set for the class.
     */
    obj = grandparent();
    if (obj == OS_NULL) return OS_FALSE;
    os_lock();
    propertyset = eglobal->propertysets->firstc(obj->classid());
    os_unlock();

    /* Get property var
     */
    propertyvar = propertyset->firstv(propertynr);
    if (propertyvar == OS_NULL) return OS_FALSE;

    /* Get subproperty list
     */
    propertyvar->propertyv(EVARP_CONF, x);
    return !x->isempty();
}
