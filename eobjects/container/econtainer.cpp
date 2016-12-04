/**

  @file    econtainer.cpp
  @brief   Simple object container.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The container object is like a box holding a set of child objects.

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
eContainer::eContainer(
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
eContainer::~eContainer()
{
}


/**
****************************************************************************************************

  @brief Write container content to stream.

  The eContainer::writer() function serialized the container to stream. This writes only the 
  content, use eObject::write() to save also class information, attachements, etc.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eContainer::writer(
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
    for (child = getfirst(); child; child = child->getnext())
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

  The eContainer::reader() function reads serialized container from stream. This function 
  reads only the object content. To read whole object including attachments, names, etc, 
  use eObject::read().
  
  @param  stream The stream to read from.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eContainer::reader(
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

