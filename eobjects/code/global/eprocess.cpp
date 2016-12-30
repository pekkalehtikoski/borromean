/**

  @file    eprocess.cpp
  @brief   Simple object process.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The process object is like a box holding a set of child objects.

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
eProcess::eProcess(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eThread(parent, oid, flags)
{
    eglobal->process_ns = new eNameSpace(this, EOID_NAMESPACE);
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eProcess::~eProcess()
{
}

void eProcess::initialize(eContainer *params)
{
    osal_console_write("initializing worker\n");
}

void eProcess::run()
{
    for (os_int i = 0; i<10 && !exitnow(); i++)
    {
        osal_console_write("worker running\n");
        osal_thread_sleep(1000);
    }
}

eStatus eProcess::onmessage(
    eEnvelope *envelope) 
{
    eThread::onmessage(envelope);
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Create eProcess object and start  thread to run it.

  The eprocess_create() function...


  @return  None.

****************************************************************************************************
*/
void eprocess_create()
{
    eProcess 
        *process;

    osal_mutex_system_lock();
    if (eglobal->processhandle == OS_NULL)
    {
        /* Create process object and start thread to run it.
         */
        process = new eProcess();
        eglobal->process = process;
    //	t->addname("_process", ENAME_PROCESS_NS);
    //    t->setpriority();
        eglobal->processhandle = new eThreadHandle();
        process->start(eglobal->processhandle);         /* After this t pointer is useless */
    }
    osal_mutex_system_unlock();
}

/**
****************************************************************************************************

  @brief Terminate eProcess thread and clean up.

  The eprocess_close() function...

  @return  None.

****************************************************************************************************
*/
void eprocess_close()
{
    osal_mutex_system_lock();
    if (eglobal->processhandle)
    {
       /* Request to process to exit and wait for thread to terminate.
         */
        eglobal->processhandle->terminate();
        eglobal->processhandle->join();

        eglobal->processhandle = OS_NULL;
    }
    osal_mutex_system_unlock();
}
