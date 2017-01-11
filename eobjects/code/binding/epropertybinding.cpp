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

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
ePropertyBinding::ePropertyBinding(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eBinding(parent, oid, flags)
{
    /* Clear member variables.
     */
    m_propertyname = OS_NULL;
    m_propertynamesz = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

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
  @param  oid Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *ePropertyBinding::clone(
    eObject *parent, 
    e_oid oid,
    os_int aflags)
{
    eObject
        *clonedobj,
        *child;

    clonedobj = new ePropertyBinding(parent, oid, flags());

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

  @brief Write propertybinding content to stream.

  The ePropertyBinding::writer() function serialized the propertybinding to stream. This writes only the 
  content, use eObject::write() to save also class information, attachements, etc.
  
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
    const os_int 
        version = 1;

    eObject
        *child;

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

  @brief Read conatiner content from stream.

  The ePropertyBinding::reader() function reads serialized propertybinding from stream. This function 
  reads only the object content. To read whole object including attachments, names, etc, 
  use eObject::read().
  
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
    os_int 
        version;

    os_long
        count;

	/* Begin the object and write version number.
     */
    if (stream->read_begin_block(version)) goto failed;

    /* Write child count to stream (no attachments).
     */
    if (*stream >> count)  goto failed;

    /* Read children
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
  @return None.

****************************************************************************************************
*/
void ePropertyBinding::bind(
    os_int localpropertynr,
    os_char *remotepath,
    os_char *remoteproperty,
    os_int bflags)
{
    eSet 
        *parameters;

    /* Save bind parameters and flags.
     */
    set_propertyname(remoteproperty);
    m_localpropertynr = localpropertynr;
    m_bflags = bflags | EBIND_CLIENT;

    /* Get parameters from derived class and add flags to parameters.
     */
    parameters = new eSet(this);
    get_bind_parameters(parameters);
    parameters->setl(E_BINDPRM_FLAGS, bflags & EBIND_SER_MASK);
    parameters->sets(E_BINDPRM_PROPERTYNAME, remoteproperty);

    /* Call base class to do binding.
     */
    eBinding::bind(remotepath, parameters);
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
    eVariable propertyname;

    parameters = eSet::cast(envelope->content());
    if (parameters == OS_NULL)
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: no content.");
#endif
        goto notarget;
    }

    /* Get property name. 
     */
    if (!parameters->get(E_BINDPRM_PROPERTYNAME, &propertyname)) 
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: Property name missing.");
#endif
        goto notarget;
    }

    /* Convert property name to property number (-1 = unknown property).
     */
    m_localpropertynr = obj->propertynr(propertyname.gets());
    if (m_localpropertynr < 0)
    {
#if OSAL_DEBUG
        osal_debug_error("srvbind() failed: Property name unknwon.");
        osal_debug_error(propertyname.gets());
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

    /* . If subproperties are requested, list ones matching in both ends.
         Store initial property value.
     */
    reply = new eSet(this);
    /* if (m_flags & ATTR)
    {

    } */

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

    /* Call base class to complete the binding.
     */
    cbindok_base(envelope);
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
    os_char *propertyname)
{
    if (m_propertyname)
    {
        osal_memory_free(m_propertyname, m_propertynamesz);
        m_propertyname = OS_NULL;
        m_propertynamesz = 0;
    }

    if (propertyname)
    {
        m_propertynamesz = (os_short)os_strlen(propertyname);
        m_propertyname = (os_char*)osal_memory_allocate(m_propertynamesz, OS_NULL);
        os_memcpy(m_propertyname, propertyname, m_propertynamesz);
    }
}


/**
****************************************************************************************************

  @brief Save property name.

  The ePropertyBinding::set_propertyname() releases current m_propertyname and stores 
  propertyname given as argument. If propertyname is OS_NULL, memory is just freeed.

  @param  propertyname Pointer to object path.
  @return None.

****************************************************************************************************
*/
/* void ePropertyBinding::set_propertyvalue(
    os_char *propertyname)
{

}
*/

