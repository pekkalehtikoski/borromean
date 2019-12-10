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
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
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
    /* Clear the set to release all allocated memory.
     */
    clear();
    
    /* Release items buffer
     */    
    os_free(m_items, m_alloc);
}


/**
****************************************************************************************************

  @brief Add eSet to class list.

  The eSet::setupclass function adds newobj function pointer to class list. This enables creating 
  new objects dynamically by class identifier, which is used for serialization reader()
  functions.

****************************************************************************************************
*/
void eSet::setupclass()
{
    const os_int cls = ECLASSID_SET;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eSet");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Clone object

  The eSet::clone function clones and object including object's children. 

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eSet::clone(
    eObject *parent, 
    e_oid id,
    os_int aflags)
{
    eSet *clonedobj;
    eObject *objptr;
    os_char *p, *e, *strptr, *newstr;
    os_uchar iid, ibytes;
    os_schar itype;
    os_int strsz;
    os_memsz sz;
    eHandle *handle;

    clonedobj = new eSet(parent, id == EOID_CHILD ? oid() : id, flags());

    if (m_items)
    {
        clonedobj->m_items = os_malloc(m_used, &sz);
        clonedobj->m_used = m_used;
        clonedobj->m_alloc = (os_int)sz;
        os_memcpy(clonedobj->m_items, m_items, m_used);

        /* Prepare to go trough items.
         */
        p = clonedobj->m_items;
        e = p + m_used;

        /* Search id from items untim match found.
         */
        while (p < e)
        {
            iid = *(os_uchar*)(p++);
            ibytes = *(os_uchar*)(p++);
            if (ibytes)
            {
                itype = *(os_schar*)(p++);

                switch (itype)
                {
                    case -OS_STR:
                        strptr = *(os_char**)p;
                        strsz = *(os_int*)(p + sizeof(os_char*));
                        newstr = os_malloc(strsz, OS_NULL);
                        os_memcpy(newstr, strptr, strsz);
                        *(os_char**)p = newstr;
                        break;

                    case OS_OBJECT:
                        objptr = *(eObject**)p;
                        *(eObject**)p = objptr->clone(clonedobj, EOID_CHILD, EOBJ_NO_MAP);
                        break;
                }
            
                p += ibytes;       
            }
        }        
    }

//    clonegeneric(clonedobj, aflags);

    /* Copy attachments and properties stored as variable.
     */

    /* If there is no handle pointer, there can be no children to clone.
     */
    if (mm_handle == OS_NULL) return clonedobj;

    /* Copy clonable attachments or all clonable object.
     */
    for (handle = mm_handle->first();
         handle;
         handle = handle->next())
    {
        if (((handle->flags() & EOBJ_IS_ATTACHMENT) || handle->oid() >= 0) &&
            (handle->flags() & EOBJ_NOT_CLONABLE) == 0)
        {
            handle->object()->clone(clonedobj, handle->oid(), EOBJ_NO_MAP);
        }
    }

    /* Map names to name spaces.
     */
    if ((aflags & EOBJ_NO_MAP) == 0)
    {
        map(E_ATTACH_NAMES);
    }

    return clonedobj;
}


/**
****************************************************************************************************

  @brief Write set content to stream.

  The eSet::writer() function serializes the eSet to stream. This writes only the
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
    const os_int version = 0;
    os_int strsz, count;
    os_memsz nwritten;
    eObject *objptr;
    os_char *p, *e, *strptr;
    os_uchar iid, ibytes;
    os_schar itype;
    eHandle *handle;

	/* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Save properties stored as variable.
     */
    if (mm_handle) 
    {
        /* Save count.
         */
        count = 0;
        for (handle = mm_handle->first();
             handle;
             handle = handle->next())
        {
            if (handle->oid() >= 0 && (handle->flags() & EOBJ_NOT_CLONABLE) == 0)
            {
                count++;
            }
        }
        if (stream->putl(count)) goto failed;

        /* Copy clonable attachments or all clonable object.
         */
        for (handle = mm_handle->first();
             handle;
             handle = handle->next())
        {
            if (handle->oid() >= 0 && (handle->flags() & EOBJ_NOT_CLONABLE) == 0)
            {
                if (stream->putl(handle->oid())) goto failed;
                if (handle->object()->writer(stream, flags)) goto failed;
            }
        }
    }

    /* Buffer used, bytes.
     */
    if (stream->putl(m_used)) goto failed;

    /* Prepare to go trough items.
     */
    p = m_items;
    e = p + m_used;

    /* Search id from items untim match found.
     */
    while (p < e)
    {
        iid = *(os_uchar*)(p++);
        ibytes = *(os_uchar*)(p++);
        if (stream->putl(iid)) goto failed;
        if (stream->putl(ibytes)) goto failed;

        if (ibytes)
        {
            itype = *(os_schar*)(p++);
            if (stream->putl(itype)) goto failed;

            switch (itype)
            {
                case -OS_STR:
                    strptr = *(os_char**)p;
                    strsz = *(os_int*)(p + sizeof(char*)) - 1;
                    if (stream->putl(strsz)) goto failed;
                    stream->write(strptr, strsz, &nwritten);
                    if (nwritten != strsz) goto failed;
                    break;

                case OS_OBJECT:
                    objptr = *(eObject**)p;
                    if (objptr->write(stream, flags)) goto failed;
                    break;

                default:
                    stream->write(p, ibytes, &nwritten);
                    if (nwritten != ibytes) goto failed;
                    break;
            }
            
            p += ibytes;       
        }
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

  @brief Read set content from stream.

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
    os_int version;
    os_long lval, count;
    os_memsz nread;
    os_int strsz;
    eObject *objptr;
    eVariable *v;
    os_char *p, *e, *strptr;
    os_uchar iid, ibytes;
    os_schar itype;

    /* If we have old data, delete it.
     */
    /* if (m_items)
    {
        os_free(m_items, m_alloc);
        m_items = OS_NULL;
        m_alloc = m_used = 0;
    }
    while ((objptr = first())) delete objptr; */

	/* Read object start mark and version number.
     */
    if (stream->read_begin_block(&version)) goto failed;

    /* Save properties stored as variable.
     */
    if (mm_handle) 
    {
        /* Read count.
         */
        if (stream->getl(&count)) goto failed;

        /* Copy clonable attachments or all clonable object.
         */
        while (count--)
        {
            /* Read object identifier, allocate variable and
               read variable from stream.
             */
            if (stream->getl(&lval)) goto failed;
            v = new eVariable(this, (e_oid)lval);
            if (v->reader(stream, flags)) goto failed;
        }
    }

    /* Buffer used, bytes.
     */
    if (stream->getl(&lval)) goto failed;
    m_used = (os_int)lval;
    if (m_used == 0) goto skipit;

    /* Allocate buffer containing items
     */
    m_items = os_malloc(m_used, &nread);
    m_alloc = (os_int)nread;

    /* Prepare to go trough items.
     */
    p = m_items;
    e = p + m_used;

    /* Search id from items until match found.
     */
    while (p < e)
    {
        if (stream->getl(&lval)) goto failed;
        iid = (os_uchar)lval;
        if (stream->getl(&lval)) goto failed;
        ibytes = (os_uchar)lval;
        
        *(os_uchar*)(p++) = iid;
        *(os_uchar*)(p++) = ibytes;

        if (ibytes)
        {
            if (stream->getl(&lval)) goto failed;
            itype = (os_schar)lval;
            *(os_schar*)(p++) = itype;

            switch (itype)
            {
                case -OS_STR:
                    if (stream->getl(&lval)) goto failed;
                    strsz = (os_int)(lval+1);
                    strptr = os_malloc(strsz, OS_NULL);
                    stream->read(strptr, (os_memsz)lval, &nread);
                    if (nread != lval) goto failed;
                    strptr[lval] = '\0';

                    *(os_char**)p = strptr;
                    *(os_int*)(p + sizeof(char*)) = strsz;
                    break;

                case OS_OBJECT:
                    objptr = read(stream, flags);
                    *(eObject**)p = objptr;
                    break;

                default:
                    stream->read(p, ibytes, &nread);
                    if (nread != ibytes) goto failed;
                    break;
            }
            
            p += ibytes;       
        }
    }        


skipit:
	/* End the object.
     */
    if (stream->read_end_block()) goto failed;

    /* Object succesfully read.
     */
    return ESTATUS_SUCCESS;

    /* Reading object failed.
     */
failed:
    os_free(m_items, m_alloc);
    m_items = OS_NULL;
    m_alloc = m_used = 0;

    return ESTATUS_READING_OBJ_FAILED;
}


#if E_SUPPROT_JSON
/**
****************************************************************************************************

  @brief Write set to stream as JSON.

  The eSet::json_writer() function writes class specific object content to stream as JSON.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags. Typically EOBJ_SERIALIZE_DEFAULT.
  @param  indent Indentation depth, 0, 1... Writes 2x this spaces at beginning of a line.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eSet::json_writer(
    eStream *stream, 
    os_int sflags,
    os_int indent)
{
    eVariable x, *v;
    eObject *objptr;
    os_char *p, *e, *strptr, nbuf[OSAL_NBUF_SZ];
    os_uchar iid, ibytes;
    os_schar itype;
    os_boolean comma = OS_TRUE;

#if 0
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
#endif

    /* Prepare to go trough items.
     */
    p = m_items;
    if (p == OS_NULL) return ESTATUS_SUCCESS;
    e = p + m_used;

    /* Search id from items untim match found.
     */
    while (p < e)
    {
        iid = *(os_uchar*)(p++);
        ibytes = *(os_uchar*)(p++);

        if (ibytes == 0)
        {
            x.clear();
        }
        else
        {
            itype = *(os_schar*)(p++);

            switch (itype)
            {
                case OS_CHAR:
                    x.setl(*(os_schar*)p);
                    break;

                case OS_SHORT:
                    x.setl(*(os_short*)p);
                    break;

                case OS_INT:
                    x.setl(*(os_int*)p);
                    break;

                case OS_LONG:
                    x.setl(*(os_long*)p);
                    break;

                case OS_DOUBLE:
                    if (ibytes == 1)
                        x.setd(*(os_schar*)p);
                    else
                        x.setd(*(os_double*)p);
                    break;

                case OS_STR:
                    x.sets(p, ibytes);
                    break;

                case -OS_STR:
                    strptr = *(os_char**)p;
                    x.sets(strptr);
                    break;

                case OS_OBJECT:
                    objptr = *(eObject**)p;
                    x.seto(objptr);
                    break;

                default:
                    x.clear();
                    break;
            }
        }

        // if (json_puts(stream, ",\n")) goto failed;
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma)) goto failed;

        if (json_puts(stream, "\"i")) goto failed;
        osal_int_to_string(nbuf, sizeof(nbuf), iid);
        if (json_puts(stream, nbuf)) goto failed;
        if (json_puts(stream, "\": ")) goto failed;

        if (json_putv(stream, OS_NULL, &x, sflags, indent + 1)) goto failed;

        p += ibytes;
    }

    for (v = firstv(); v; v = v->nextv())
    {
        if (v->oid() < 0) continue;
    
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma)) goto failed;

        if (json_puts(stream, "\"v")) goto failed;
        osal_int_to_string(nbuf, sizeof(nbuf), v->oid());
        if (json_puts(stream, nbuf)) goto failed;
        if (json_puts(stream, "\": ")) goto failed;
        if (json_putv(stream, OS_NULL, v, sflags, indent + 1)) goto failed;
    }        

    return ESTATUS_SUCCESS;

failed:
    return ESTATUS_FAILED;
}
#endif


/**
****************************************************************************************************

  @brief Store value into set.

  The eSet::set function 
  
  @param  id Identification number (for example property number) for value to store.
  @param  x Variable containing value to store.
          - x = OS_NULL -> delete value
          - x = empty var -> store empty mark;
  @param  flags Reserved for future, set 0 for now.

  @return None.

****************************************************************************************************
*/
void eSet::set(
    os_int id,
    eVariable *x,
    os_int sflags)
{
    eVariable *v;
    eObject *o, *optr;
    os_long l;
    os_double d;
    os_int i, isz;
    os_short s;
    os_schar itype, jtype, c;
    os_uchar ibytes, jbytes, jid;
    os_char *p, *e, *q, *sptr, *start;
    os_memsz sz;
    const os_int slack = 10;
    void *iptr;

    osal_debug_assert(id >= 0);

    /* If we have variable with this id, use it.
     */
    v = firstv(id);
    if (v) 
    {
        if (x == OS_NULL) delete v;
        else v->setv(x);
        return;
    }
    
    /* If this id cannot be presented as uchar, use variable. 
     */
    if (id < 0 || id > 255) 
    {
        goto store_as_var;
    }

    /* Determnine size and type,
     */
    if (x == OS_NULL)
    {
        ibytes = -1;
    }
    else switch (x->type())
    {
        case OS_LONG:
            l = x->getl();
            if (l >= -0x80 && l <= 0x7F)
            {
                itype = OS_CHAR;
                ibytes = sizeof(os_schar);
                c = (os_schar)l;
                iptr = &c;
            }
            else if (l >= -0x8000 && l <= 0x7FFF)
            {
                itype = OS_SHORT;
                ibytes = sizeof(os_short);
                s = (os_short)l;
                iptr = &s;
            }
            else if (l >= -2147483647 && l <= 0x7FFFFFFF) 
            {
                itype = OS_INT;
                ibytes = sizeof(os_int);
                i = (os_int)l;
                iptr = &i;
            }
            else 
            {
                itype = OS_LONG;
                ibytes = sizeof(os_long);
                iptr = &l;
            }
            break;

        case OS_DOUBLE:
            d = x->getd();
            itype = OS_DOUBLE;
            ibytes = sizeof(os_double);
            iptr = &d;
            if (d >= -128.0 && d <= 127.0)
            {
                if (d == (os_double)x->getl())
                {
                    ibytes = sizeof(os_char);
                    if (d >= 0) c = (os_char)(d+0.5);
                    else c = -(os_char)(-d+0.5);
                    iptr = &c;
                }   
            }
            break;

        case OS_OBJECT:
            itype = OS_OBJECT;
            ibytes = sizeof(eObject *);
            o = x->geto();
            optr = o->clone(this, EOID_ITEM);
            iptr = &optr;
            break;

        default:
        case OS_STR:
            q = x->gets(&sz);
            if (q == '\0')
            {
                itype = OS_UNDEFINED_TYPE;
                ibytes = 0;
                iptr = OS_NULL;
            }
            else if (sz > 64) 
            {
                itype = -OS_STR;
                ibytes = sizeof(os_char*) + sizeof(os_int);
                sptr = os_malloc(sz, OS_NULL);
                os_memcpy(sptr, q, sz);
                iptr = &sptr;
                isz = (os_int)sz;
            }
            else
            {
                itype = OS_STR;
                ibytes = (os_uchar)sz;
                iptr = q;
            }
            break;

        case OS_UNDEFINED_TYPE: /* empty */
            itype = OS_UNDEFINED_TYPE;
            ibytes = 0;
            iptr = OS_NULL;
            break;
    }

    /* Prepare to go trough items.
     */
    p = m_items;
    e = p + m_used;

    /* Search id from items until match found.
     */
    while (p < e)
    {
        start = p;
        jid = *(os_uchar*)(p++);
        jbytes = *(os_uchar*)(p++);
        if (jbytes) jtype = *(os_uchar*)(p++);
        else jtype = OS_UNDEFINED_TYPE;
        if (jid == (os_uchar)id)
        {
            /* Release memory allocated for previous value.
             */
            if (jtype == OS_OBJECT) 
            {
                delete *(eObject**)p;
            }
            else if (jtype == -OS_STR) 
            {
                os_free(*(void**)p, *(os_int*)(p + sizeof(os_char*)));
            }

            /* If it is same length
             */
            if (ibytes == jbytes)
            {
                /* p = start + 1;
                *(os_uchar*)(p++) = ibytes; */
                p = start + 2;
                if (ibytes == 0) return;
                *(os_schar*)(p++) = itype;

                os_memcpy(p, iptr, ibytes);
                if (itype == -OS_STR)
                {
                    p += ibytes;
                    *(os_int*)p = isz;
                }
                return;
            }

            /* Different length, remove this entry form m_items buffer.
             */
            p += jbytes;
            if (e != p) 
            {
                os_memmove(start, p, e - p); 
            }
            m_used -= (os_int)(p - start);
            break;
        }
        p += jbytes;
    }

    if (x == OS_NULL) return;

    /* If we need to allocate more memory?
     */
    if (m_used + ibytes + 3 * sizeof(os_char) > m_alloc)
    {
        start = os_malloc(3 * sizeof(os_char)
            + ibytes + m_used + m_used/4 + slack, &sz);
        if (m_items)
        {
            os_memcpy(start, m_items, m_used);
            os_free(m_items, m_alloc);
        }
        m_alloc = (os_int)sz;
        m_items = start;
    }

    /* Append new value.
     */
    p = m_items + m_used;
    *(os_uchar*)(p++) = (os_uchar)id;
    *(os_uchar*)(p++) = (os_uchar)ibytes;
    if (ibytes)
    {
        *(os_schar*)(p++) = itype;
        if (itype != -OS_STR)
        {
            os_memcpy(p, iptr, ibytes);
            p += ibytes;
        }
        else
        {
            ibytes = sizeof(os_char*);
            os_memcpy(p, iptr, ibytes);
            p += ibytes;
            *(os_int*)p = isz;
            p += sizeof(os_int);
        }
    }
    m_used = (os_int)(p - m_items);
   return;

store_as_var:
    v = new eVariable(this, id);
    v->setv(x);
}


/**
****************************************************************************************************

  @brief Get value from set.

  The eSet::get function 
  
  @param  id Identification number (for example property number) for value to store.
  @param  x Variable containing value to store.
          - x = OS_NULL -> delete value
          - x = empty var -> store empty mark;
  @param  flags Reserved for future, set 0 for now.

  @return Return value can be used between empty value and unset value. This is needed for 
          properties. OS_TRUE if value was found, even empty one. OS_FALSE if no value for 
          the ID was found.

****************************************************************************************************
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
        if (iid == (os_uchar)id)
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
                    break;

                case OS_SHORT:
                    x->setl(*(os_short*)p);
                    break;

                case OS_INT:
                    x->setl(*(os_int*)p);
                    break;

                case OS_LONG:
                    x->setl(*(os_long*)p);
                    break;

                case OS_DOUBLE:
                    if (ibytes == 1)
                        x->setd(*(os_schar*)p);
                    else
                        x->setd(*(os_double*)p);
                    break;

                case OS_STR:
                    x->sets(p, ibytes);
                    break;

                case -OS_STR:
                    strptr = *(os_char**)p;
                    x->sets(strptr);
                    break;

                case OS_OBJECT:
                    objptr = *(eObject**)p;
                    x->seto(objptr);
                    break;

                default:
                    x->clear();
                    break;
            }
                
            return OS_TRUE;
        }

        if (ibytes) p += ibytes + 1;
    }        

    /* comtinues ...
     */
getout:
    x->clear();
    return OS_FALSE;
}

/**
****************************************************************************************************

  @brief Clear the set.

  The eSet::clear function ...

  @return Return value can be used between empty value and unset value. This is needed for 
          properties. OS_TRUE if value was found, even empty one. OS_FALSE if no value for 
          the ID was found.

****************************************************************************************************
*/
void eSet::clear()
{
    eObject *objptr;
    os_char *p, *e, *strptr;
    os_uchar iid, ibytes;
    os_schar itype;
    os_int strsz;

    /* Prepare to go trough items.
     */
    p = m_items;
    if (p == OS_NULL) return;
    e = p + m_used;

    /* Search id from items untim match found.
     */
    while (p < e)
    {
        iid = *(os_uchar*)(p++);
        ibytes = *(os_uchar*)(p++);
        if (ibytes)
        {
            itype = *(os_schar*)(p++);

            switch (itype)
            {
                case -OS_STR:
                    strptr = *(os_char**)p;
                    strsz = *(os_int*)(p + sizeof(char*)) ;
                    os_free(strptr, strsz);
                    break;

                case OS_OBJECT:
                    objptr = *(eObject**)p;
                    delete objptr;
                    break;
            }
            
            p += ibytes;       
        }
    }        

    m_used = 0;
}
