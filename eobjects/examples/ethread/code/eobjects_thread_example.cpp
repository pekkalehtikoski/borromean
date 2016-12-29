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

class eMyThread : public eThread
{
    void initialize(
        eObject *parameters);

//    run();
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

	eVariable 
		*v;

    eName
        *n;

    eThread
        *t;

    eThreadHandle 
        *thandle;

	/* Enable name space for container.
	 */
	// ecreate_thread(?);

//	    osal_console_write("\n");

    t = new eMyThread("name", &threadparams, );
    t->setpriority();
    eThreadHandle = t->start(); /* After this t pointer is useless */

    

    return 0;
}
