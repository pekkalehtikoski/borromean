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
ePropertyBinding::~ePropertyBinding()
{
}


/**
****************************************************************************************************

  @brief Clone object

  The ePropertyBinding::clone function clones and object including object's children. 
  Names will be left detached in clone.

  @param  parent Parent for the clone.
  @param  oid Object identifier for the clone.
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

  @brief Get next child propertybinding identified by oid.

  The eVariable::nextc() function returns pointer to the next child propertybinding of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for. 

  @return  Pointer to the first child propertybinding, or OS_NULL if none found.

****************************************************************************************************
*/
ePropertyBinding *ePropertyBinding::nextc(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->next(oid);
    while (h)
    {
        if (h->object()->classid() == ECLASSID_PROPERTYBINDING) 
            return ePropertyBinding::cast(h->object());

        h = h->next(oid);
    }
    return OS_NULL;
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

