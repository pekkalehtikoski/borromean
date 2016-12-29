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
    m_flags = m_target_pos = m_source_end = m_source_alloc = 0;

    m_target = m_source = OS_NULL;
    m_content = m_context = OS_NULL;
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
}


/**
****************************************************************************************************

  @brief Set destination for the envelope.

  The eEnvelope::settarget() function. 

  @return  None.

****************************************************************************************************
*/
void eEnvelope::settarget(
    os_char *target)
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


