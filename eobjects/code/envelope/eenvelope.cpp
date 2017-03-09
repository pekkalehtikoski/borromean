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

/* Variable property names.
 */
os_char
    eenvp_command[] = "command",
    eenvp_target[] = "target",
    eenvp_source[] = "source",
    eenvp_content[] = "content",
    eenvp_context[] = "context";


/* Place name in front of the path.
 */
void eenvelope_prepend_name(
    eEnvelopePath *path,
    const os_char *name)
{
    os_char *newstr, *p;
    os_memsz sz;
    os_int name_sz, newpos;
    os_boolean hasoldpath;

    name_sz = (os_int)os_strlen(name);
    hasoldpath = (os_boolean)(path->str_pos + 1 < path->str_alloc);

    /* If name doesn't fit, we need to allocate more space.
     */
    if (name_sz > path->str_pos)
    {
	    newstr = os_malloc(path->str_alloc + name_sz - path->str_pos + 14, &sz);
        newpos = (os_int)(sz - (path->str_alloc - path->str_pos) - name_sz);
        p = newstr + newpos;
        os_memcpy(p, name, name_sz);
        if (path->str)
        {
            if (hasoldpath)
            {
                p[name_sz - 1] = '/';
                os_memcpy(p + name_sz, path->str + path->str_pos, path->str_alloc - path->str_pos);
            }
            os_free(path->str, path->str_alloc);
        }
        path->str = newstr;
        path->str_alloc = (os_short)sz;
        path->str_pos = newpos;
    }

    /* Name fits, place it in.
     */
    else
    {
        path->str_pos -= name_sz;
        p = path->str + path->str_pos;
        os_memcpy(p, name, name_sz);
        if (hasoldpath) p[name_sz - 1] = '/';
    }
}



/* Clear the path and release memory allocated for it.
 */
void eenvelope_clear_path(
    eEnvelopePath *path)
{
    if (path->str) 
    {
        os_free(path->str, path->str_alloc);
        path->str = OS_NULL;
    }
    path->str_alloc = 0;
    path->str_pos = 0;
}



/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEnvelope::eEnvelope(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
    /** Clear member variables.
     */
    m_command = 0;
    m_mflags = 0;

    /* m_target_pos = m_source_end = m_source_alloc = 0;
    m_target = m_source = OS_NULL; */

    os_memclear(&m_target, sizeof(eEnvelopePath));
    os_memclear(&m_source, sizeof(eEnvelopePath));
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
    if (m_target.str)
    {
        eenvelope_clear_path(&m_target);
    }
    if (m_source.str)
    {
        eenvelope_clear_path(&m_source);
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
    e_oid id,
    os_int aflags)
{
    eEnvelope *clonedobj;

    /* Clone must have parent object.
     */
    osal_debug_assert(parent);

    clonedobj = new eEnvelope(parent, id == EOID_CHILD ? oid() : id, flags());
  
    /** Clone envelope specific stuff.
     */
    clonedobj->m_command = m_command;
    clonedobj->m_mflags = m_mflags;
    clonedobj->settarget(target());
    clonedobj->prependsource(source());

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
  which is used for serialization reader functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eEnvelope::setupclass()
{
    const os_int cls = ECLASSID_ENVELOPE;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eEnvelope");

    addpropertyl(cls, EENVP_COMMAND, eenvp_command, EPRO_PERSISTENT|EPRO_SIMPLE, "command");
    addpropertys(cls, EENVP_TARGET, eenvp_target, EPRO_PERSISTENT|EPRO_SIMPLE, "target");
    addpropertys(cls, EENVP_SOURCE, eenvp_source, EPRO_PERSISTENT|EPRO_SIMPLE, "source");
    addproperty (cls, EENVP_CONTENT, eenvp_content, EPRO_PERSISTENT|EPRO_SIMPLE, "content");
    addproperty (cls, EENVP_CONTEXT, eenvp_context, EPRO_PERSISTENT|EPRO_SIMPLE, "context");
    os_unlock();
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
void eEnvelope::onpropertychange(
    os_int propertynr, 
    eVariable *x, 
    os_int flags)
{
    eObject *obj;
    eVariable *var;

    switch (propertynr)
    {
        case EENVP_COMMAND:
            m_command = (os_int)x->getl();
            break;

        case EENVP_TARGET:
            eenvelope_clear_path(&m_target);
            eenvelope_prepend_name(&m_target, x->gets());
            break;

        case EENVP_SOURCE:
            eenvelope_clear_path(&m_source);
            eenvelope_prepend_name(&m_source, x->gets());
            break;

        case EENVP_CONTENT:
            delete content();
            obj = x->geto();
            if (obj)
            {
                obj->clone(this, EOID_CONTENT);
            }
            else
            {
                var = new eVariable(this, EOID_CONTENT);
                var->setv(x);
            }
            break;

        case EENVP_CONTEXT:
            delete context();
            obj = x->geto();
            if (obj)
            {
                obj->clone(this, EOID_CONTEXT);
            }
            else
            {
                var = new eVariable(this, EOID_CONTEXT);
                var->setv(x);
            }
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
  @param   x Variable into which to store the property value.
  @return  If property with property number was stored in x, the function returns
           ESTATUS_SUCCESS (0). Nonzero return values indicate that property with
           given number was not among simple properties.

****************************************************************************************************
*/
eStatus eEnvelope::simpleproperty(
    os_int propertynr, 
    eVariable *x)
{
    eObject *obj;

    switch (propertynr)
    {
        case EENVP_COMMAND:
            x->setl(m_command);
            break;

        case EENVP_TARGET:
            x->sets(target());
            break;

        case EENVP_SOURCE:
            x->sets(source());
            break;

        case EENVP_CONTENT:
            obj = content();
            x->seto(obj);
            break;

        case EENVP_CONTEXT:
            obj = context();
            x->seto(obj);
            break;
   
        default:
            x->clear();
            /* return eObject::simpleproperty(propertynr, x); */
            return ESTATUS_NO_SIMPLE_PROPERTY_NR;
    }
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write envelope to stream.

  The eEnvelope::writer() function serializes envelope to stream. This writes only envelope
  specific content, use eObject::write() to save also class information, attachements, etc.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eEnvelope::writer(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Increment if new serialized items are to the object,
       and check for new version's items in read() function.
     */
    const os_int version = 0;
    os_int n;
    os_short mflags;
    os_memsz nmoved;
    eObject *ctnt, *ctxt;

	/* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Write command.
     */
    if (stream->putl(m_command)) goto failed;

    /* Decide on flags which need to be passed.
     */
    mflags = m_mflags & (EMSG_NO_REPLIES | EMSG_NO_ERRORS);
    ctnt = content();
    ctxt = context();
    if (ctnt) mflags |= EMSG_HAS_CONTENT;
    if (ctxt) mflags |= EMSG_HAS_CONTEXT;
    if (stream->putl(mflags)) goto failed;

    /* Write target.
     */
    if (m_target.str)
    {
        n = (os_int)(m_target.str_alloc - m_target.str_pos) - 1;
    }
    else
    {
        n = 0;
    }
    if (stream->putl(n)) goto failed;
    if (n>0) 
    {
        stream->write(m_target.str + m_target.str_pos, n, &nmoved);
        if (nmoved != n) goto failed;
    }

    /* Write source, unless EMSG_NO_REPLIES is given.
     */
    if ((m_mflags & EMSG_NO_REPLIES) == 0)
    {
        if (m_source.str)
        {
            n = (os_int)(m_source.str_alloc - m_source.str_pos) - 1;
        }
        else
        {
            n = 0;
        }
        if (stream->putl(n)) goto failed;
        if (n>0) 
        {
            stream->write(m_source.str + m_source.str_pos, n, &nmoved);
            if (nmoved != n) goto failed;
        }
    }

    /* Write content.
     */
    if (ctnt) 
    {
        if (ctnt->write(stream, flags)) goto failed;
    }

    /* Write context.
     */
    if (ctxt) 
    {
        if (ctxt->write(stream, flags)) goto failed;
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

  @brief Read envelope from stream.

  The eEnvelope::reader() function reads serialized envelope from stream. This function reads 
  only the object content. To read whole object including attachments, names, etc, 
  use eObject::read().
  
  @param  stream The stream to read from.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eEnvelope::reader(
    eStream *stream, 
    os_int flags) 
{
    /* Version number. Used to check which versions item's are in serialized data.
     */
    os_int version;
    os_long l, mflags;
    os_memsz sz;
    os_int c;

	/* Read object start mark and version number.
       Special case, check if we received invisible flush count character which changed
       the flush count to zero (no more whole objects buffered in stream). If so, 
       return ESTATUS_NO_WHOLE_MESSAGES_TO_READ.
     */
    c = stream->readchar();
    if (c == E_STREAM_FLUSH)
    {        
        if (stream->flushcount() <= 0)
        {
            return ESTATUS_NO_WHOLE_MESSAGES_TO_READ;
        }
            
        c = stream->readchar();
    }
    version = (c & E_STREAM_COUNT_MASK);
    if ((c & E_STREAM_CTRL_MASK) != E_STREAM_BEGIN) goto failed;

    /* Read command.
     */
    if (stream->getl(&l)) goto failed;
    m_command = (os_int)l;

    /* Get flags, separate flags to be passed over connection only.
     */
    if (stream->getl(&mflags)) goto failed;
    m_mflags = (mflags & (EMSG_NO_REPLIES | EMSG_NO_ERRORS)) | EMSG_NO_RESOLVE;

    /* Read target.
     */
    if (stream->getl(&l)) goto failed;
    if (l > 0)
    {
	    m_target.str = os_malloc(l + 1 + 14, &sz);
        m_target.str_alloc = (os_short)sz;
        m_target.str_pos = (os_short)(sz - l - 1);
        stream->read(m_target.str + m_target.str_pos, l, &sz);
        if (sz != l) goto failed;
        m_target.str[m_target.str_pos + l] = '\0';
    }

    /* Read source, unless EMSG_NO_REPLIES is given.
     */
    if ((m_mflags & EMSG_NO_REPLIES) == 0)
    {
        if (stream->getl(&l)) goto failed;
        if (l > 0)
        {
	        m_source.str = os_malloc(l + 1 + 14, &sz);
            m_source.str_alloc = (os_short)sz;
            m_source.str_pos = (os_short)(sz - l - 1);
            stream->read(m_source.str + m_source.str_pos, l, &sz);
            if (sz != l) goto failed;
            m_source.str[m_source.str_pos + l] = '\0';
        }
    }

    /* Read content.
     */
    if (mflags & EMSG_HAS_CONTENT) 
    {
        if (read(stream, flags) == OS_NULL) goto failed;
    }

    /* Read context.
     */
    if (mflags & EMSG_HAS_CONTEXT) 
    {
        if (read(stream, flags) == OS_NULL) goto failed;
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
        len;
  
    p = target->gets(&len);
    settarget(p);
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

  The eEnvelope::prependsourceoix function...

  Example prepended string:
  - "@17_3" oix=15, ucnt = 3
  - "@15" oix=15, ucnt = 0

  @param  o Pointer to object whose use oix and ucnt are to be appended.
  @return None.

****************************************************************************************************
*/
void eEnvelope::prependsourceoix(
    eObject *o)
{
    os_char 
        buf[E_OIXSTR_BUF_SZ];

    /** Get oix and ucnt as string.
     */
    o->oixstr(buf, sizeof(buf));
    prependsource(buf);
}
