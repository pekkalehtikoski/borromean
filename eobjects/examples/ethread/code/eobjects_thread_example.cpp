/**

  @file    eobjects_thread_example.cpp
  @brief   Example code about naming objects.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  This example demonstrates how to create threads.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"

/* Generate entry code for console application.
 */
EMAIN_CONSOLE_ENTRY


/**
****************************************************************************************************

  @brief Example thread class.

  X...

****************************************************************************************************
*/
class eMyThread : public eThread
{
    virtual void initialize(eContainer *params = OS_NULL)
    {
        osal_console_write("initializing worker\n");
    }

    virtual void run()
    {
        while (!exitnow())
        {
            alive();

            osal_console_write("worker running\n");
        }
    }

    virtual eStatus onmessage(
        eEnvelope *envelope) 
    {
        /* If at final destination for the message.
         */
        if (*envelope->target()=='\0')
        {
            osal_console_write(envelope->source());
            osal_console_write(": ");
            eVariable *v = eVariable::cast(envelope->content());
            osal_console_write(v ? v->gets() : "NULL");

            osal_console_write("\n");
            return ESTATUS_SUCCESS;
        }

        eThread::onmessage(envelope);

        return ESTATUS_SUCCESS;
    }
};


/**
****************************************************************************************************

  @brief Application entry point.

  The emain() function is eobjects application's entry point.

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return  None.

****************************************************************************************************
*/
os_int emain(
    os_int argc,
    os_char *argv[])
{
	eContainer
		root;

    eVariable
        *txt;

    eThread
        *t;

    eThreadHandle 
        thandle;

    /* Create and start thread named "worker".
     */
    t = new eMyThread();
	t->addname("worker", ENAME_PROCESS_NS);
//    t->setpriority();
    t->start(&thandle); /* After this t pointer is useless */

    for (os_int i = 0; i<1000; i++)
    {
        osal_console_write("master running\n");
        osal_thread_sleep(20);

        txt = new eVariable(&root);
        txt->sets("message content");
        root.message (10, "//worker", OS_NULL, txt, EMSG_DEL_CONTENT /* |EMGS_NO_REPLIES */);
    }

    /* Wait for thread to terminate
     */
    thandle.terminate();
    thandle.join();

    return 0;
}
