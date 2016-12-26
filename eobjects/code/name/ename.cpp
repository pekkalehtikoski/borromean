/**

  @file    ename.cpp
  @brief   Name class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The names map to name space. These are used to represent tree structure of named objects.
  The eName class is derived from eVariable class.

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
eName::eName(
	eObject *parent,
	e_oid oid,
	os_int flags)
	: eVariable(parent, oid, flags)
{
	m_ileft = m_iright = m_iup = OS_NULL;
	m_index = OS_NULL;

	/* If this is name space.
	 */
	if (oid == EOID_NAME)
	{
		/* Flag this as attachment.
		 */
		setflags(EOBJ_IS_ATTACHMENT);

		/* If we have parent object, flag parent that it has name space.
		 */
		// if (parent) parent->setflags(EOBJ_HAS_NAMESPACE);
	}
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eName::~eName()
{
}


/**
****************************************************************************************************

  @brief Write name to stream.

  The eName::writer() function serialized name to stream. This writes only content, 
  use eObject::write() to save also class information, attachements, etc.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eName::writer(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Increment if new serialized items are to the object,
       and check for new version's items in read() function.
     */
    const os_int 
        version = 1;

	/* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Use base class'es function to do the work.
     */
    eVariable::writer(stream, flags);

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

  @brief Read name from stream.

  The eName::reader() function reads serialized name from stream. This function reads only content. 
  To read whole object including attachments, names, etc, use eObject::read().
  
  @param  stream The stream to read from.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eName::reader(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Used to check which versions item's are in serialized data.
     */
    os_int 
        version;

	/* Begin the object and write version number.
     */
    if (stream->read_begin_block(version)) goto failed;

    /* Use base class'es function to do the work.
     */
    eVariable::reader(stream, flags);

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

  @brief Get next child of same parent object.

  The eName::getnext() function returns pointer to next name in namespace. 
  Next name is the next child of the same parent object as this object (it could be called 
  sibling). 

  @param   name_match OS_TRUE (default) to get next name inly if it is same name as this object.
           OS_FALSE to get next name in name space, regardless of the value.

  @return  Pointer to next name, or OS_NULL if no matching object was found. 

****************************************************************************************************
*/
eName *eName::nextname(
	os_boolean name_match)
{
    eName 
        *n,
        *m;

    n = this;

    if (n->m_iright)
    {
        n = n->m_iright;
        while (n->m_ileft) n = n->m_ileft;

        if (!name_match) return n;

		return compare(n) ? OS_NULL : n;
    }
    else
    {
        m = n->m_iup;

        while (OS_TRUE) 
        {
            if (m == OS_NULL) return OS_NULL;
            if (m->m_ileft == n) break;
            n = m;
            m = n->m_iup;
        }

        if (!name_match) return m;

		return compare(m) ? OS_NULL : m;
    }
}
