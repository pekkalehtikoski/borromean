/**

  @file    eenvelope.cpp
  @brief   Envelope class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  Messages are sent as envelope objects. The eEnvelope contains recipient and senders's addresses,
  command, message content and other data.

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
eEnvelope::eEnvelope(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    /** Clear member variables.
     */
    m_command = 0;
    m_mflags = m_target_pos = m_source_end = m_source_alloc = 0;

    m_target = m_source = OS_NULL;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEnvelope::~eEnvelope()
{
    if (m_target)
    {
        osal_memory_free(m_target, m_target_alloc);
    }
    if (m_source)
    {
        osal_memory_free(m_source, m_source_alloc);
    }
}


/**
****************************************************************************************************

  @brief Clone object

  The clone function clones and object including object's children. 
  Names will be left detached in clone if EOBJ_NO_MAP flag is given.

  @param  parent Parent for the clone.
  @param  oid Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names. 
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eEnvelope::clone(
    eObject *parent, 
    e_oid oid,
    os_int aflags)
{
    eEnvelope *clonedobj;
    clonedobj = new eEnvelope(parent, oid, flags());
  
    /** Clone envelope specific stuff.
     */
    clonedobj->m_command = m_command;
    clonedobj->m_mflags = m_command;
    clonedobj->settarget(target());
    clonedobj->appendsource(source());

    /* Copy all clonable children.
     */
    clonegeneric(clonedobj, aflags|EOBJ_CLONE_ALL_CHILDREN);
    return clonedobj;
}


/**
****************************************************************************************************

  @brief Add the class to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds the class to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property stet stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eEnvelope::setupclass()
{
    const os_int cls = ECLASSID_ENVELOPE;

    /* Add the class to class list.
     */
    osal_mutex_system_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj);
    osal_mutex_system_unlock();
}


/**
****************************************************************************************************

  @brief Set destination for the envelope.

  The eEnvelope::settarget() function. 

  @return  None.

****************************************************************************************************
*/
void eEnvelope::settarget(
    const os_char *target)
{
    os_memsz
        sz,
        len;

    if (m_target)
    {
        osal_memory_free(m_target, m_target_alloc);
    }
    
    len = os_strlen(target);
	m_target = (os_char*)osal_memory_allocate(len, &sz);
    m_target_alloc = (os_short)sz;
    m_target_pos = 0;
    os_memcpy(m_target, target, len);
}


/**
****************************************************************************************************

  @brief Set destination for the envelope.

  The eEnvelope::settarget() function. 

  @return  None.

****************************************************************************************************
*/
void eEnvelope::settarget(
    eVariable *target)
{
    os_char
        *p;

    os_memsz
        sz,
        len;

    if (m_target)
    {
        osal_memory_free(m_target, m_target_alloc);
    }
    
    p = target->gets(&len);
	m_target = (os_char*)osal_memory_allocate(len, &sz);
    m_target_alloc = (os_short)sz;
    m_target_pos = 0;
    os_memcpy(m_target, p, len);
}



/* Prepend target with with name
    */
void eEnvelope::prependtarget(
    const os_char *name)
{
    os_int name_sz, tgt_sz;
    os_char *newtgt;
    os_memsz sz;
    
    name_sz = (os_int)os_strlen(name);
    tgt_sz = (os_int)os_strlen(m_target +  m_target_pos);
    if (name_sz + tgt_sz > m_target_alloc)
    {
	    newtgt = (os_char*)osal_memory_allocate(name_sz + tgt_sz, &sz);

        os_memcpy(newtgt, name, name_sz);
        if (m_target[m_target_pos] != '\0') 
        {
            newtgt[name_sz - 1] = '/';
            os_memcpy(newtgt + name_sz, m_target + m_target_pos, tgt_sz);
        }
        osal_memory_free(m_target, m_target_alloc);
        m_target = newtgt;
        m_target_alloc = (os_int)sz;
        m_target_pos = 0;
    }
    else
    {
        if (name_sz > m_target_pos)
        {
            os_memmove(m_target + name_sz, m_target + m_target_pos, tgt_sz);
            m_target_pos = 0;
        }
        else
        {
            m_target_pos -= name_sz;
        }
        os_memcpy(m_target + m_target_pos, name, name_sz);
        if (m_target[m_target_pos + name_sz] != '\0') 
            m_target[m_target_pos + name_sz - 1] = '/';
    }
}                

/**
****************************************************************************************************

  @brief Append to destination path.

  The eEnvelope::appendsource() function....

  @return  None.

****************************************************************************************************
*/
void eEnvelope::appendsource(
    const os_char *source)
{
    os_memsz
        sz,
        len;

    os_char
        *new_source,
        *p;

    if (source)
    {
        len = os_strlen(source);

        /* If this does not fit in currect buffer allocation
         */
        if (m_source_end + len >= m_source_alloc)
        {
	        new_source = p = (os_char*)osal_memory_allocate(m_source_end + len + 10, &sz);
            if (m_source)
            {            
                os_memcpy(p, m_source, m_source_end);
                p += m_source_end;
                *(p++) = '/';
            }

            os_memcpy(p, source, len);
            p += len-1;

            m_source_end = (os_short)(p-new_source);
            if (m_source)
            {
                osal_memory_free(m_source, m_source_alloc);
            }
            m_source_alloc = (os_short)sz;
            m_source = new_source;
        }
        else
        {
           os_memcpy(m_source + m_source_end, source, len);
           m_source_end += (os_short)len-1;
        }
    }
}


/**
****************************************************************************************************

  @brief Set content object.

  The eEnvelope::setcontent() function. 

  @param dlags Bit EMSG_DEL_CONTENT to adopt content, ptherwise it is copied.
  @return  None.

****************************************************************************************************
*/
void eEnvelope::setcontent(
    eObject *o,
    os_int flags)
{
    eObject
        *c;

    /* Delete old content, if any.
     */
    c = content();
    if (c) delete c;

    if (o)
    {
        if (flags & EMSG_DEL_CONTENT)
        {
            adopt(o, EOID_CONTENT, EOBJ_NO_MAP);
        }
        else
        {
            o->clone(this, EOID_CONTENT, EOBJ_NO_MAP);
        }
    }
}


/**
****************************************************************************************************

  @brief Set context object.

  The eEnvelope::setcontext() function. 

  @param dlags Bit EMSG_DEL_CONTEXT to adopt content, ptherwise it is copied.
  @return  None.

****************************************************************************************************
*/
void eEnvelope::setcontext(
    eObject *o,
    os_int flags)
{
    eObject
        *c;

    /* Delete old context, if any.
     */
    c = context();
    if (c) delete c;

    if (o)
    {
        if (flags & EMSG_DEL_CONTEXT)
        {
            adopt(o, EOID_CONTEXT, EOBJ_NO_MAP);
        }
        else
        {
            o->clone(this, EOID_CONTEXT, EOBJ_NO_MAP);
        }
    }
}


/**
****************************************************************************************************

  @brief Get next name from target string.

  The eEnvelope::nexttarget() function...

  @param  x Pointer to variable where to store the name.
  @return None.

****************************************************************************************************
*/
void eEnvelope::nexttarget(
    eVariable *x)
{
    os_char
        *p,
        *e;

    p = e = target();

    while (*e != '/' && *e != '\0') e++;
    x->sets(p, e-p);
}


/**
****************************************************************************************************

  @brief Append object index and use counter.

  The eEnvelope::appendsourceoix function...

  Example appended string:
  - "@17_3" oix=15, ucnt = 3
  - "@15" oix=15, ucnt = 0

  @param  o Pointer to object whose use oix and ucnt are to be appended.
  @return None.

****************************************************************************************************
*/
void eEnvelope::appendsourceoix(
    eObject *o)
{
    os_char 
        buf[E_OEXSTR_BUF_SZ];

    /** Get oix and ucnt as string.
     */
    o->oixstr(buf, sizeof(buf));
    appendsource(buf);
}
