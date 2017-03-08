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
    e_oid id,
	os_int flags)
    : eThread(parent, id, flags)
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
        /* Wait for thread to be trigged.
         */
        osal_event_wait(trigger(), OSAL_EVENT_INFINITE); 

        /* Process messages
         */
        os_lock();
        alive(EALIVE_RETURN_IMMEDIATELY);
        os_unlock();
    }
}

void eProcess::onmessage(
    eEnvelope *envelope) 
{
    eThread::onmessage(envelope);
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

    eTimer
        *tim;

    eThreadHandle
        *processhandle,
        *timerhandle;

    if (eglobal->processhandle == OS_NULL)
    {
        /* Create process object and start thread to run it.
         */
        process = new eProcess();
        processhandle = new eThreadHandle();
        process->start(processhandle);         /* After this process pointer is useless */

        /* Create thread which runs timers.
         */
        tim = new eTimer();
        timerhandle = new eThreadHandle();
        tim->start(timerhandle); /* After this tim pointer is useless */

        /* Add as global process only when process has been created.
         */
        os_lock();
        eglobal->process = process;
        eglobal->processhandle = processhandle;
        eglobal->timerhandle = timerhandle;
        os_unlock();
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
        /* Request timer thread to exit and wait for thread to terminate.
         */
        eglobal->timerhandle->terminate();
        eglobal->timerhandle->join();

        /* Request process thread to exit and wait for thread to terminate.
         */
        eglobal->processhandle->terminate();
        eglobal->processhandle->join();

        eglobal->processhandle = OS_NULL;
    }
}
