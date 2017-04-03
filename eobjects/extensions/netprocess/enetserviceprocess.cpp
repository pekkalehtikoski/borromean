/**

  @file    enetserviceprocess.cpp
  @brief   default enet process startup for a device.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects/extensions/netprocess/enetprocess.h"
#include "eobjects/extensions/socket/esocket.h"

/**
****************************************************************************************************

  @brief Startup code for a service process.

  The enet_start_service_process() function does default startup procedure for enet process
  which is expected to run as a service, like a program running a robot, etc.

  @param  process_name Process name, like "grumpy".
  @param  tcpport TCP port number to listen to.
  @param  argc Number of command line arguments.
  @param  argv Array of string pointers, one for each command line argument. UTF8 encoded.
  @return None.

****************************************************************************************************
*/
void enet_start_service_process(
    const os_char *process_name,
    const os_char *tcpport,
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
    enet_process_initialize(process_name, argc >= 2 ? argv[1] : "0");

    /* Load composition JSON files.
     */
    enet_process_load_composition();

    /* Start listening for incoming TCP connections. Argument is default TCP port.
       to listen to.
     */
    enet_process_listen(tcpport);

    /* If process needs to connect to ewebservice, do it.
     */
    if (enet_service_ip_address)
    {
        enet_process_connect_ewebservice(enet_service_ip_address);
    }
}
