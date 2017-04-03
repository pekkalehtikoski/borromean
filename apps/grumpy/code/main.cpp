/**

  @file    main.cpp
  @brief   Application entry point.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Entry point to the application.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects/extensions/netprocess/enetprocess.h"
#include "grumpy.h"

/* Generate entry code for console application.
 */
EMAIN_CONSOLE_ENTRY


/**
****************************************************************************************************

  @brief Application entry point.

  The emain() function is eobjects application's entry point. When the emain() function is
  called, eobjects library is already initialized. Notice that it is not necessary to start
  the application this way, you can use any application entry code and initialize components
  needed from there.

  This program can take two optional command line arguments. First is process identification
  and second one is IP address of ewebservice.

  @param  argc Number of command line arguments.
  @param  argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return Application return value.

****************************************************************************************************
*/
os_int emain(
    os_int argc,
    os_char *argv[])
{
    Grumpy grumpy;

    /* Setup necessary optional classes, initialize as enet process, load composition,
       listen for TCP socket and optionally connect to ewebservice. Application (process)
       name is "grumpy" and it listens to TCP port 14119 by default.
     */
    enet_start_service_process("grumpy", ":14119", argc, argv);

    /* Start the application.
     */
    grumpy.run();

    /* Finished.
     */
    return 0;
}
