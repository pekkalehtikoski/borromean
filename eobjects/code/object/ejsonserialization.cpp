/**

  @file    ejsonserialization.cpp
  @brief   Object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Object serialization in json format. 
  
  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#if E_SUPPROT_JSON


/**
****************************************************************************************************

  @brief Write object to stream as JSON.

  The eObject::json_write() function writes object to stream as JSON.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags. EOBJ_SERIALIZE_DEFAULT

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eObject::json_write(
    eStream *stream, 
    os_int sflags,
    os_int indent) 
{
    eObject *child;
    os_long n_attachements;

    /* Write starting '{'
     */
    if (json_indent(stream, indent++, EJSON_NO_NEW_LINE)) goto failed;
    if (json_puts(stream, "{")) goto failed;


#if 0

    /* Write class identifier, object identifier and persistant object flags.
     */
    if (*stream << classid()) goto failed;
    if (*stream << oid()) goto failed;
    if (*stream << flags() & (EOBJ_SERIALIZATION_MASK)) goto failed;

    /* Calculate and write number of attachments.
       HERE WE SHOULD USE HANDLES FOR SPEED
     */
    n_attachements = 0;
    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isserattachment()) n_attachements++;
    }
    if (*stream << n_attachements) goto failed;
    
    /* Write the object content.
     */
    if (writer(stream, sflags)) goto failed;

    /* Write attachments.
       HERE WE SHOULD USE HANDLES FOR SPEED
     */
    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isserattachment()) 
        {
            if (child->write(stream, sflags)) goto failed;
        }
    }
#endif

    /* Write terminating '}'
     */
    if (json_indent(stream, --indent)) goto failed;
    if (json_puts(stream, "}")) goto failed;
    if (json_indent(stream, 0, EJSON_NEW_LINE_ONLY)) goto failed;
    
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

  @brief Read object from stream.

  The eObject::read() function reads class information, etc from the stream, creates new 
  child object and reads child object content and attachments.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags. EOBJ_SERIALIZE_DEFAULT

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eObject *eObject::json_read(
    eStream *stream, 
    os_int sflags)
{
    os_int cid, oid, oflags;
    os_long n_attachements, i;
    eObject *child;

    /* Read class identifier, object identifier, persistant object flags
       and number of attachments.
     */
    if (*stream >> cid) goto failed;
    if (*stream >> oid) goto failed;
    if (*stream >> oflags) goto failed;
    if (*stream >> n_attachements) goto failed;

    /* Generate new object.
     */
    child = newchild(cid, oid);
    if (child == OS_NULL) goto failed;

    /* Set flags.
     */
    child->setflags(oflags);
    
    /* Read the object content.
     */
    if (child->reader(stream, sflags)) goto failed;

    /* Read attachments.
     */
    for (i = 0; i<n_attachements; i++)
    {
        if (read(stream, sflags) == OS_NULL) goto failed;
    }

    /* Object succesfully read, return pointer to it.
     */
    return child;

    /* Reading object failed.
     */
failed:
    return OS_NULL;
}

/* 
  @param  indent Indentation depth, 0, 1... Writes 2x this spaces before the line.
  @param  iflags EJSON_NO_NEW_LINE, EJSON_NEW_LINE_BEFORE, EJSON_NEW_LINE_ONLY
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return
          values indicate an error.
 */
eStatus eObject::json_indent(
    eStream *stream, 
    os_int indent,
    os_int iflags) 
{
    os_int i;

    if (iflags & (EJSON_NEW_LINE_BEFORE|EJSON_NEW_LINE_ONLY))
    {
        if (json_puts(stream, "\n")) return ESTATUS_FAILED;
    }   

    if ((iflags & EJSON_NEW_LINE_ONLY)==0)
    {
        for (i = 0; i<indent; i++)
        {
            if (json_puts(stream, "  ")) return ESTATUS_FAILED;
        }
    }

    return ESTATUS_SUCCESS;
}

/* 
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return
          values indicate an error.
 */
eStatus eObject::json_puts(
    eStream *stream, 
    os_char *str)
{
    os_memsz len;
    len = os_strlen(str);

    return stream->write(str, len);
}

#endif
