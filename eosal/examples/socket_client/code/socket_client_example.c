/**

  @file    examples/console/osal_console_example.c
  @brief   Example code about using console.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This example demonstrates how to use console.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/code/main/osal_main.h"


/**
****************************************************************************************************

  @brief Process entry point.

  The osal_main() function is OS independent entry point.

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return  None.

****************************************************************************************************
*/
os_int osal_main(
    os_int argc,
    os_char *argv[])
{
	os_uint
		c;

	osal_console_write("osal_console_example\npress any key... ", OSAL_SYSTEM_CONSOLE_NR);

	c = osal_console_read_char(20000, OSAL_SYSTEM_CONSOLE_NR);

	if (c)
	{
		osal_console_write("\n", OSAL_SYSTEM_CONSOLE_NR);
	}
	else
	{
		osal_console_write("No key pressed within 20 seconds.\n", OSAL_SYSTEM_CONSOLE_NR);
	}

    return 0;
}
