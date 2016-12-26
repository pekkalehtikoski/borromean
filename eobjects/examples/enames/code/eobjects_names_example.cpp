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
		cont;

	eVariable 
		*var,
        dar;

    os_long 
        u = 22;

	/* Enable name space for container.
	 */
	cont.createnamespace();

	/* Create variable as child of container and give it a value.
	 */
	var = new eVariable(&cont);
	var->sets("myvalue");

	/* Set name.
	 */
	var->addname("myname");


/*     dar = var = "abba ";
    dar = 10;
    dar = u;

    var += dar;
    var += " ja nasse";
    var += " ja pulla";

	econsole_write(var.gets(), OSAL_DEBUG_CONSOLE_NR);
	econsole_write("\n", OSAL_DEBUG_CONSOLE_NR);


    dar = 1110.018;
	dar.setddigs(5);
	econsole_write(dar.gets(), OSAL_DEBUG_CONSOLE_NR);
	econsole_write("\n", OSAL_DEBUG_CONSOLE_NR);
*/


    return 0;
}
