/**

  @file    eprocess.h
  @brief   enet process functionality.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ENETPROCESS_INCLUDED
#define ENETPROCESS_INCLUDED

/* Initialize this as enet process. First argument is process name and second argument
   is process identification number.
 */
void enet_process_initialize(
    const os_char *process_name,
    const os_char *process_nr);

/* Save enet process settings to file.
 */
void enet_process_save_settings();

/* Load composition JSON files.
 */
void enet_process_load_composition();

/* Start listening for incoming TCP connections.
 */
void enet_process_listen(
    const os_char *tcpport);

/* Connect process to ewebservice.
 */
void enet_process_connect_ewebservice(
    const os_char *enet_service_ip_address);

/* Startup code for a service process.
 */
void enet_start_service_process(
    const os_char *process_name,
    const os_char *tcpport,
    os_int argc,
    os_char *argv[]);

#endif
