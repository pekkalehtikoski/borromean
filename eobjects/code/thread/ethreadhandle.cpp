/**

  @file    ethreadhandle.cpp
  @brief   Thread handle class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  Thread handle is used for controlling threads from another thread.

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
eThreadHandle::eThreadHandle(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
    m_osal_handle = OS_NULL;
    m_unique_thread_name[0] = '\0';
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eThreadHandle::~eThreadHandle()
{
    if (m_osal_handle)
    {
        terminate();
	    osal_thread_join(m_osal_handle);
        m_osal_handle = OS_NULL;
    } 
}


/**
****************************************************************************************************

  @brief Save unique thread name for terminating the thread.

  X...

  @return  None.

****************************************************************************************************
*/
void eThreadHandle::save_unique_thread_name(
    eThread *thread)
{
    thread->oixstr(m_unique_thread_name, sizeof(m_unique_thread_name));
}


/**
****************************************************************************************************

  @brief Request to terminate a thread.

  X...

  @return  None.

****************************************************************************************************
*/
void eThreadHandle::terminate()
{
    if (m_unique_thread_name[0] != '\0')
    {
        message (ECMD_EXIT_THREAD, m_unique_thread_name, 
            OS_NULL, OS_NULL, EMSG_NO_REPLIES);
    }
}


/**
****************************************************************************************************

  @brief Wait until thread has terminated.

  X...

  @return  None.

****************************************************************************************************
*/
void eThreadHandle::join()
{
    if (m_osal_handle)
    {
	    osal_thread_join(m_osal_handle);
        m_osal_handle = OS_NULL;
    }
}
