/**

  @file    enetprocess.cpp
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
#include "eobjects/eobjects.h"
#include "eobjects/extensions/netprocess/enetprocess.h"

/* Default root directory.
 */
#ifndef ENET_ROOT_DIR
#define ENET_ROOT_DIR OSAL_FS_ROOT "coderoot/borromean/"
#endif

/* Default composition root directory.
 */
#ifndef ENET_COMPOSITION_DIR
#define ENET_COMPOSITION_DIR ENET_ROOT_DIR "apps/"
#endif

/* Default executable directory.
 */
#ifndef ENET_BIN_DIR
#define ENET_BIN_DIR ENET_ROOT_DIR "bin/" OSAL_BIN_NAME "/"
#endif

/* Default root parameter directory.
 */
#ifndef ENET_PRM_DIR
#define ENET_PRM_DIR ENET_ROOT_DIR "prm/"
#endif

/* Default root database directory.
 */
#ifndef ENET_DB_DIR
#define ENET_DB_DIR ENET_ROOT_DIR "db/"
#endif


/**
****************************************************************************************************

  @brief Initialize this process as enet process.

  The enet_process_initialize function stores process name and identification number into
  global structure and loads process settings from file.

  @param  process_name Process name, like "grumpy".
  @param  process_nr Process identification number. Identifies the instances of the executable.
          Can be serial number or short string. Foe example "10" could identify the process
          as "grumpy_10".
  @return None.

****************************************************************************************************
*/
void enet_process_initialize(
    const os_char *process_name,
    const os_char *process_nr)
{
    /* Save process identification in global flat structure, so synchronization
       is not needed to access these.
     */
    os_strncpy(eglobal->process_name, process_name, ENET_PROCESS_NAME_SZ);
    os_strncpy(eglobal->process_nr, process_nr, ENET_PROCESS_NR_SZ);
    os_strncpy(eglobal->process_id, process_name, ENET_PROCESS_ID_SZ);
    os_strncat(eglobal->process_id, "_", ENET_PROCESS_ID_SZ);
    os_strncat(eglobal->process_id, process_nr, ENET_PROCESS_ID_SZ);
    os_strncpy(eglobal->process_nick_name, eglobal->process_id,
        ENET_PROCESS_NICK_NAME_SZ);

    /* Set paths: to composition folder, executables folder, parameter folder
       and to database folder. These can be set by environment variables or hard
       coded during compilation from -D compiler option. There are defaults for
       each operating system if noting is specified.
       On some devices, at least on android, it is necessary to locate writable folders
       at run time.
     */
    os_strncpy(eglobal->composition_dir, ENET_COMPOSITION_DIR, ENET_DIR_SZ);
    os_strncat(eglobal->composition_dir, process_name, ENET_DIR_SZ);
    os_strncat(eglobal->composition_dir, "/", ENET_DIR_SZ);
    os_strncpy(eglobal->bin_dir, ENET_BIN_DIR, ENET_DIR_SZ);
    os_strncpy(eglobal->prm_dir, ENET_PRM_DIR, ENET_DIR_SZ);
    os_strncat(eglobal->prm_dir, eglobal->process_id, ENET_DIR_SZ);
    os_strncat(eglobal->prm_dir, "/", ENET_DIR_SZ);
    os_strncpy(eglobal->db_dir, ENET_DB_DIR, ENET_DIR_SZ);
    os_strncat(eglobal->db_dir, eglobal->process_id, ENET_DIR_SZ);
    os_strncat(eglobal->db_dir, "/", ENET_DIR_SZ);

    /* Set default top composition file name.
     */
    os_strncpy(eglobal->composition, "default", ENET_COMPOSITION_NAME_SZ);

    /* Load process specific parameters from file.
     */

}


/**
****************************************************************************************************

  @brief Save enet process settings to file.

  The enet_process_save_settings function needs to be called after process settings in global
  structure have changed to write the changes to disc.

  @return None.

****************************************************************************************************
*/
void enet_process_save_settings()
{
}


/**
****************************************************************************************************

  @brief Load JSON composition files.

  The enet_process_load_composition function loads JSON composition files for parameters,
  devices and IO mapping. The top level composition file includes other composition files
  which list the files to be loaded. If these lists include wildcard, files are loaded
  in alphabetical order.

  @return None.

****************************************************************************************************
*/
void enet_process_load_composition()
{

}


/**
****************************************************************************************************

  @brief Start listening for incoming TCP connections.

  The enet_process_listen function starts listening TCP socket connections.

  @param  tcpport TCP port number to listen to.

  @return None.

****************************************************************************************************
*/
void enet_process_listen(
    const os_char *tcpport)
{
    eThread *t;

    /* Create and start thread to listen for incoming socket connections,
       name it "endpoint". After this t pointer is useless.
     */
    t = new eEndPoint();
    t->addname("//endpoint");
    t->setpropertys(EENDPP_IPADDR, tcpport);
    t->start();
}


/**
****************************************************************************************************

  @brief Connect process to ewebservice.

  The enet_process_connect_ewebservice function...

  @param  enet_service_ip_address

  @return None.

****************************************************************************************************
*/
void enet_process_connect_ewebservice(
    const os_char *enet_service_ip_address)
{

}

