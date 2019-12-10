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
  @param  indent Indentation depth, 0, 1... Writes 2x this spaces at beginning of a line.
          -1 is same as 0, but with extra new line at the end.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eObject::json_write(
    eStream *stream, 
    os_int sflags,
    os_int indent,
    os_boolean *comma) 
{
    os_char *str;
    os_int i;
    eVariable list, *p, value;
    eName *name;
    eContainer *propertyset, *bindings;
    eBinding *b;
    os_boolean comma1 = OS_FALSE, comma2 = OS_FALSE, property_listed;
    os_boolean end_with_nl = OS_FALSE;
    
    if (indent < 0)
    {
        indent = 0;
        end_with_nl = OS_TRUE;
    }

    if (comma)
    {
        if (*comma) 
        {
            if (json_puts(stream, ",")) return ESTATUS_FAILED;
        }
        if (json_puts(stream, "\n")) goto failed;
        *comma = OS_TRUE;
    }

    /* Write starting '{'
     */
    if (comma) if (json_indent(stream, indent, EJSON_NO_NEW_LINE)) goto failed;
    indent++;
    if (json_puts(stream, "{")) goto failed;

    /* Class name.
     */
    str = eclasslist_classname(classid());
    if (str)
    {
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma1)) goto failed;
        if (json_puts(stream, "\"class\": ")) goto failed;
        if (json_putqs(stream, str)) goto failed;
    }

    /* Names 
     */
    list.clear();
    for (name = firstn(EOID_NAME); name; name = name->nextn(EOID_NAME))
    {
        str = name->namespaceid();
        if (str)
        {
            value = str;
            value += "/";
        }
        else
        {
            value = "";
        }
        value += *name;
        json_append_list_item(&list, value.gets(), 0, 0);
    }
    if (!list.isempty())
    {
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma1)) goto failed;
        if (json_puts(stream, "\"names\": [")) goto failed;
        if (json_puts(stream, list.gets())) goto failed;
        if (json_puts(stream, "]")) goto failed;
    }

    /* Object identifier, oid.
     */
    if (oid() != EOID_ITEM)
    {
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma1)) goto failed;
        if (json_puts(stream, "\"oid\": ")) goto failed;
        if (json_putl(stream, oid())) goto failed;
    }

    /* Write flags.
     */
    list.clear();
    i = flags();
    json_append_list_item(&list, "attachment", i, EOBJ_IS_ATTACHMENT);
    json_append_list_item(&list, "namespace", i, EOBJ_HAS_NAMESPACE);
    json_append_list_item(&list, "cf_1", i, EOBJ_CUST_FLAG1);
    json_append_list_item(&list, "cf_2", i, EOBJ_CUST_FLAG2);
    json_append_list_item(&list, "cf_3", i, EOBJ_CUST_FLAG3);
    json_append_list_item(&list, "cf_4", i, EOBJ_CUST_FLAG4);
    json_append_list_item(&list, "cf_5", i, EOBJ_CUST_FLAG5);
    if (!list.isempty())
    {
        if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma1)) goto failed;
        if (json_puts(stream, "\"flags\": [")) goto failed;
        if (json_puts(stream, list.gets())) goto failed;
        if (json_puts(stream, "]")) goto failed;
    }

    /* Write properties which are different from default value.
     */
    os_lock();
    propertyset = eglobal->propertysets->firstc(classid());
    os_unlock();
    if (propertyset)
    {
        property_listed = OS_FALSE;
        for (p = propertyset->firstv(); p; p = p->nextv())
        {
            /* Get property value and property name. Skip if same as default value or no name.
             */
            propertyv((os_int)p->oid(), &value);
            if (!value.compare(p)) continue;
            name = p->firstn(EOID_NAME);
            if (name == OS_NULL) continue;

            /* If property list not started, start now.
             */
            if (!property_listed)
            {
                if (json_indent(stream, indent, EJSON_NEW_LINE_BEFORE, &comma1)) goto failed;
                if (json_puts(stream, "\"properties\": {")) goto failed;
                property_listed = OS_TRUE;
            }
                
            if (json_indent(stream, indent+1, EJSON_NEW_LINE_BEFORE, &comma2)) goto failed;
            if (json_putqs(stream, name->gets())) goto failed;
            if (json_puts(stream, ": ")) goto failed;
            if (json_putv(stream, p, &value, sflags, indent+1)) goto failed;
        }

        if (property_listed)
        {
            if (json_indent(stream, indent)) goto failed;
            if (json_puts(stream, "}")) goto failed;
        }
    }

    /* Write bindings.
     */
    bindings = firstc(EOID_BINDINGS);
    if (bindings) 
    {
        for (b = eBinding::cast(first()); b; b = eBinding::cast(b->next()))
        {
            b->json_write(stream, sflags, indent);
        }
    }
    
    /* Write content (children, etc)
     */
    if (json_writer(stream, sflags, indent)) goto failed;

    /* Write terminating '}'
     */
    if (json_indent(stream, --indent)) goto failed;
    if (json_puts(stream, "}")) goto failed;
    if (comma || end_with_nl) if (json_indent(stream, 0, EJSON_NEW_LINE_ONLY)) goto failed;
    
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
    os_int iflags,
    os_boolean *comma) 
{
    os_int i;

    if (iflags & (EJSON_NEW_LINE_BEFORE|EJSON_NEW_LINE_ONLY))
    {
        if (comma)
        {
            if (*comma) 
            {
                if (json_puts(stream, ",")) return ESTATUS_FAILED;
            }
            *comma = OS_TRUE;
        }
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


/**
****************************************************************************************************

  @brief Write string to JSON output.

  The eObject::json_puts() function writes string to JSON stream.
  
  @param  stream The stream to write JSON to.
  @param  str String to write.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eStatus eObject::json_puts(
    eStream *stream, 
    const os_char *str)
{
    os_memsz len;
    len = os_strlen(str);

    return stream->write(str, len);
}


/**
****************************************************************************************************

  @brief Write quoted string to JSON output.

  The eObject::json_putqs() function writes quoted string to JSON stream.
  
  @param  stream The stream to write JSON to.
  @param  str String to write.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eStatus eObject::json_putqs(
    eStream *stream, 
    const os_char *str)
{
    if (json_puts(stream, "\"")) return ESTATUS_FAILED;
    if (json_puts(stream, str)) return ESTATUS_FAILED;
    return json_puts(stream, "\"");
}

/**
****************************************************************************************************

  @brief Write long integer to JSON output.

  The eObject::json_putl() function writes long integer to JSON stream.
  
  @param  stream The stream to write JSON to.
  @param  x Long integer to write.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eStatus eObject::json_putl(
    eStream *stream, 
    os_long x)
{
    os_char nbuf[OSAL_NBUF_SZ];

    osal_int_to_string(nbuf, sizeof(nbuf), x);
    return json_puts(stream, nbuf);
}


/**
****************************************************************************************************

  @brief Write variable value to JSON output.

  The eObject::json_putv() function writes variable value to JSON stream.
  
  @param  stream The stream to write JSON to.
  @param  p Property in property set.
  @param  value Value to write. May be modified by this function.
  @param  sflags Serialization flags. EOBJ_SERIALIZE_DEFAULT
  @param  indent Indentation depth, 0, 1... Writes 2x this spaces at beginning of a line.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eStatus eObject::json_putv(
    eStream *stream, 
    eVariable *p,
    eVariable *value,
    os_int sflags,
    os_int indent) 
{
    eObject *obj;
    os_boolean quote;
    os_long typ;
    
    /* If the value contains object, write it
     */
    obj = value->geto();
    if (obj) 
    {
        return obj->json_write(stream, sflags, indent);
    }

    /* Copy number of decimal digits
     */
    if (p) value->setdigs(p->digs());

    /* Select weather to qute the value
     */
    quote = OS_TRUE;
    if (p) 
    {
        typ = p->propertyl(EVARP_TYPE);
    }
    else
    {
        typ = OS_UNDEFINED_TYPE;
    }

    switch (typ)
    {
        default:
            if (value->isempty()) 
            {
                value->sets("null");
                quote = OS_FALSE;
            }
            else
            {
                if (p) value->autotype(OS_TRUE);
                if (value->type() == OS_LONG || value->type() == OS_DOUBLE)
                {
                    quote = OS_FALSE;
                }
            }
            break;

        case OS_STR:
            break;
    }

    return quote ? json_putqs(stream, value->gets()) : json_puts(stream, value->gets());
}

/**
****************************************************************************************************

  @brief Append list item string to variable if bit is set in flags.

  The eObject::json_append_list_item() function appends quoted item to variable list, if bit
  is set in flags. If list is not empty appends also separating comma.
  
  @param  list List to append to.
  @param  item Item name without quotes.
  @param  flags Value of flags.
  @param  bit Bit to check in flags.

  @return None.

****************************************************************************************************
*/
void eObject::json_append_list_item(
    eVariable *list, 
    const os_char *item,
    os_int flags,
    os_int bit)
{
    if ((flags & bit) || bit == 0)
    {
        if (!list->isempty()) list->appends(", ");
        list->appends("\"");
        list->appends(item);
        list->appends("\"");
    }
}

#endif
