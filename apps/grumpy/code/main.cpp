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
#include "eobjects/extensions/socket/esocket.h"
#include "eobjects/extensions/netprocess/enetprocess.h"

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

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return  Application return value.

****************************************************************************************************
*/
os_int emain(
    os_int argc,
    os_char *argv[])
{
    const os_char
        *process_id,
        *enet_service_ip_address;

    /* This program can take two optional command line arguments. First is process
       identification and second one is IP address of ewebservice.
     */
    process_id = argc >= 2 ? argv[1] : "0";
    enet_service_ip_address = argc >= 3 ? argv[2] : OS_NULL;

    /* Setup optional classes needed by this application.
     */
    eSocket::setupclass();

    /* Initialize this as enet process. First argument is process name and second argument
       is process identification number.
     */
    enet_process_initialize("grumpy", argc >= 2 ? argv[1] : "0");

    /* Load composition JSON files.
     */
    enet_process_load_composition();

    /* Start listening for incoming TCP connections. Argument is default TCP port
       to listen to.
     */
    enet_process_listen("14119");

    /* If process needs to connect to ewebservice, do it.
     */
    if (enet_service_ip_address)
    {
        enet_process_connect_ewebservice(enet_service_ip_address);
    }

    /* Start the application.
     */
    // grumpy.start();


    /* Finished.
     */
    return 0;
}

