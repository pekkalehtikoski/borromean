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
    while (!exitnow())
    {
        /* Process messages
         */
        osal_mutex_system_lock();
        alive(0);
        osal_mutex_system_unlock();

        osal_console_write("worker running\n");

        /* Wait for thread to be trigged.
         */
        osal_event_wait(trigger(), 1000); // OSAL_EVENT_INFINITE); // SHOULD BE OSAL_EVENT_INFINITE, NOT YET READY

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

    eThreadHandle
        *processhandle;

    if (eglobal->processhandle == OS_NULL)
    {
        /* Create process object and start thread to run it.
         */
        process = new eProcess();
        processhandle = new eThreadHandle();
        process->start(processhandle);         /* After this t pointer is useless */

        /* Add as global process only when process has been created.
         */
        osal_mutex_system_lock();
        eglobal->process = process;
        eglobal->processhandle = processhandle;
        osal_mutex_system_unlock();
    }
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
    if (eglobal->processhandle)
    {
       /* Request to process to exit and wait for thread to terminate.
         */
        eglobal->processhandle->terminate();
        eglobal->processhandle->join();

        eglobal->processhandle = OS_NULL;
    }
}
