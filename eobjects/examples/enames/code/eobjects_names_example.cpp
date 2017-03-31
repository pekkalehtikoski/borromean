/**

  @file    eobjects_names_example.cpp
  @brief   Example code about naming objects.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This example demonstrates how to name objects.

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
		c;

	eVariable 
		*v;

    eName
        *n;

	/* Enable name space for container.
	 */
	c.ns_create();

	/* Create variable as child of container and give it a value.
	 */
	v = new eVariable(&c);
	v->sets("myvalue");
	v->addname("myname");

	/* Create variable as child of container and give it a value.
	 */
	v = new eVariable(&c);
	v->sets("yourvalue");
	v->addname("yourname");

	/* Create variable as child of container and give it a value.
	 */
	v = new eVariable(&c);
	v->sets("hervalue");
	v->addname("hername");

    c.handle()->verify_whole_tree();

    /* List all names in conteiner's namespace.
     */
    for (n = c.ns_first(); n; n = n->ns_next(OS_FALSE))
    {
	    osal_console_write(n->gets());
	    osal_console_write(" ");
        v = eVariable::cast(n->parent());
	    osal_console_write(v->gets());
	    osal_console_write("\n");
    }

    /* Print your value.
     */
    v = c.ns_getv("yourname"); 
    if (v)
    {
	    osal_console_write(v->gets());
	    osal_console_write("\n");
    }

    return 0;
}
