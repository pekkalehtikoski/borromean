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
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    m_osal_handle = OS_NULL;
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
    /* if (m_osal_handle)
    {
	    osal_thread_request_exit(m_osal_handle);
	    osal_thread_join(m_osal_handle);
        m_osal_handle = OS_NULL;
    } */
}


/* Request to terminate a thread.
    */
void eThreadHandle::terminate()
{
    osal_thread_request_exit(m_osal_handle);
}

/* Wait until thread has terminated.
    */
void eThreadHandle::join()
{
    if (m_osal_handle)
    {
	    osal_thread_join(m_osal_handle);
        m_osal_handle = OS_NULL;
    }
}
