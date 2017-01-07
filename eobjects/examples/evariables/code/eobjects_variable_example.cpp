/**

  @file    eobjects_variable_example.cpp
  @brief   Example code about using eobjects variable class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This example demonstrates how to use dynamically typed variable class eVariable.

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
	eVariable 
		var,
        dar;

    os_long 
        u = 22;

    dar = var = "abba ";
    dar = 10;
    dar = u;
	osal_console_write(dar.gets());
	osal_console_write("\n");

    var += dar;
    var += " ja nasse";
    var += " ja pulla";

	osal_console_write(var.gets());
	osal_console_write("\n");

    dar = 1110.018;
	dar.setdigs(5);
    dar += var;
	osal_console_write(dar.gets());
	osal_console_write("\n");

    return 0;
}
