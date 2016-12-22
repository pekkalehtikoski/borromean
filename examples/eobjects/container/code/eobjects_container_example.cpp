/**

  @file    examples/simple/container/eobjects_container_example.cpp
  @brief   Example code about using eobjects container class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This example demonstrates how to use containers to hold multiple objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include <stdlib.h>

/**
****************************************************************************************************

  @brief Process entry point.

  The emain() function is OS independent entry point.

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return  None.

****************************************************************************************************
*/
os_int osal_main(
    os_int argc,
    os_char *argv[])
{
	eContainer
		*c;

	eVariable
		*v;

    eObject
        *o;

	os_int
		i; 

    /* Initialize eobject library for use.
     */
    eobjects_initialize(OS_NULL);

	c = new eContainer;

	for (i = 0; i<40; i++)
	{
		// v = new eVariable(c, rand() % 2000);
		// *v = i;
		v = new eVariable(c, i+1);
        *v = i; // rand();
	}

	/* for (i = 0; i<40; i++)
	{
		o = c->getfirst(rand() % 20);
		delete o;
	} */

	// delete v;

	v = new eVariable(c);
	*v = "ukemi";

	i = 0;
	for (o = c->getfirst(); o; o = o->getnext())
	{
		v = (eVariable*)o;

		osal_console_write(v->gets());
		osal_console_write(", ");
	} 

	delete c;
	
/* Shut down eobjects library.
*/
    eobjects_shutdown();
    return 0;
}

