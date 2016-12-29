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
        for (os_int i = 0; i<10 && !exitnow(); i++)
        {
            osal_console_write("worker running\n");
            osal_thread_sleep(1000);
        }
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
		threadparams;

    eThread
        *t;

    eThreadHandle 
        thandle;


    t = new eMyThread();
//    t->setpriority();
    t->start(&thandle, &threadparams); /* After this t pointer is useless */

    for (os_int i = 0; i<3; i++)
    {
        osal_console_write("master running\n");
        osal_thread_sleep(2000);
    }

    /* Wait for thread to terminate
     */
    thandle.join();

    return 0;
}
