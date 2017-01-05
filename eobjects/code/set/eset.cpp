/**

  @file    eset.cpp
  @brief   Simple set.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The set stores enumerated collection of values.

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
eSet::eSet(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    m_items = OS_NULL;
    m_used = m_alloc = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eSet::~eSet()
{
}


/**
****************************************************************************************************

  @brief Clone object

  The eSet::clone function clones and object including object's children. 
  Names will be left detached in clone.

  @param  parent Parent for the clone.
  @param  oid Object identifier for the clone.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eSet::clone(
    eObject *parent, 
    e_oid oid,
    os_int aflags)
{
    eObject
        *clonedobj,
        *child;

    clonedobj = new eSet(parent, oid, flags());

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

  @brief Write set content to stream.

  The eSet::writer() function serialized the set to stream. This writes only the 
  content, use eObject::write() to save also class information, attachements, etc.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eSet::writer(
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

  The eSet::reader() function reads serialized set from stream. This function 
  reads only the object content. To read whole object including attachments, names, etc, 
  use eObject::read().
  
  @param  stream The stream to read from.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eSet::reader(
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



/* Store value into set.
 */
void eSet::set(
    os_int id,
    eVariable *x,
    os_int sflags)
{
    eVariable *v;

    /* If we have old variable with this id, remove it.
     */
    v = firstv(id);
    if (v) delete v;
    
    if (id < 0 || id > 255) 
    {
        goto store_as_var;
    }

//    ibytes = 
    

       
   return;

store_as_var:
    v = new eVariable(this, id);
    v->setv(x);
}

/* Get value from set.
   Return value can be used between empty value and unset value. This is needed for properties.
   @return OS_TRUE if item is set. OS_FALSE if not.
 */
os_boolean eSet::get(
    os_int id,
    eVariable *x)
{
    eVariable *v;
    eObject *objptr;
    os_char *p, *e, *strptr;
    os_uchar iid, ibytes;
    os_schar itype;

    /* Try first if this value is stored in separate variable.
     */
    v = firstv(id);
    if (v)
    {
        x->setv(v);
        return OS_TRUE;
    }

    /* If this ID cannot be presented as unsigned char.
     */
    if (id < 0 || id > 255) goto getout;

    /* Prepare to go trough items.
     */
    p = m_items;
    if (p == OS_NULL) goto getout;
    e = p + m_used;

    /* Search id from items untim match found.
     */
    while (p < e)
    {
        iid = *(os_uchar*)(p++);
        ibytes = *(os_uchar*)(p++);
        if (iid == id)
        {
            if (ibytes == 0)
            {
                x->clear();
                return OS_TRUE;
            }
            itype = *(os_schar*)(p++);

            switch (itype)
            {
                case OS_CHAR:
                    x->setl(*(os_schar*)p);
                    return OS_TRUE;

                case OS_INT:
                    x->setl(*(os_short*)p);
                    return OS_TRUE;

                case OS_LONG:
                    x->setl(*(os_long*)p);
                    return OS_TRUE;

                case OS_DOUBLE:
                    if (ibytes == 1)
                        x->setd(*(os_schar*)p);
                    else
                        x->setd(*(os_double*)p);
                    return OS_TRUE;

                case OS_STRING:
                    x->sets(p, ibytes+1);
                    return OS_TRUE;

                case -OS_STRING:
                    strptr = *(os_char**)p;
                    x->sets(strptr);
                    return OS_TRUE;

                case OS_OBJECT:
                    objptr = *(eObject**)p;
                    x->seto(objptr);
                    return OS_TRUE;
            }
                
            return OS_TRUE;
        }

        p += ibytes + 1;       
    }        

    /* comtinues ...
     */
getout:
    x->clear();
    return OS_FALSE;
}
