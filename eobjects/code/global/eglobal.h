/**

  @file    eglobal.h
  @brief   Global structure.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  All global variables are placed in global structure, so it is eanough to pass to eglobal pointer 
  to Windows DLL to share eccess to globals.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EGLOBAL_INCLUDED
#define EGLOBAL_INCLUDED

class eThreadHandle;
class eConsole;

/* Space allocation for process name, nr, id, etc. strings.
 */
#define ENET_PROCESS_NAME_SZ 32
#define ENET_PROCESS_NR_SZ 16
#define ENET_PROCESS_ID_SZ 48
#define ENET_PROCESS_NICK_NAME_SZ 48

/* Space allocation for directory string and composition file name.
 */
#define ENET_DIR_SZ 128
#define ENET_COMPOSITION_NAME_SZ 32


/**
****************************************************************************************************

  @brief Global structure.

  X...


****************************************************************************************************
*/
typedef struct eGlobal
{
    /** Flags indicating that eobjects library has been initialized.
     */
    os_boolean initialized;

    eHandleRoot hroot;

    /** Root container for global objects.
     */
    eContainer *root;

    /** Empty variable which does not belong to any thread.
     */
    eVariable *empty;

    /** Container for class list.
     */
    eContainer *classlist;

    /** Container for property sets.
     */
    eContainer *propertysets;

    /** Pointer to process thread handle.
     */
    eThreadHandle *processhandle;

    /** Pointer to process object.
     */
    eProcess *process;

    /** Process name space
     */
    eNameSpace *process_ns;

    /** Pointer to timer thread handle
     */
    eThreadHandle *timerhandle;

    /** Console stream for debug output.
     */
    eConsole *console;

    /** Name of the process, like "grumpy".
     */
    os_char process_name[ENET_PROCESS_NAME_SZ];

    /** Process idenfification number, can be serial
        number or short text.
     */
    os_char process_nr[ENET_PROCESS_NR_SZ];

    /** Process identification, process name and identification name together
     */
    os_char process_id[ENET_PROCESS_ID_SZ];

    /** Process nick name to display to user.
     */
    os_char process_nick_name[ENET_PROCESS_NICK_NAME_SZ];

    /** Composition directory.
     */
    os_char composition_dir[ENET_DIR_SZ];

    /** Executable directory.
     */
    os_char bin_dir[ENET_DIR_SZ];

    /** Parameter directory.
     */
    os_char prm_dir[ENET_DIR_SZ];

    /** Database directory.
     */
    os_char db_dir[ENET_DIR_SZ];

    /** Composition file name
     */
    os_char composition[ENET_COMPOSITION_NAME_SZ];
}
eGlobal;

/* Global pointer to global structure.
 */
extern eGlobal *eglobal;

inline eNameSpace *eglobal_process_ns() 
{
    osal_debug_assert(eglobal->process_ns);
    return eglobal->process_ns;
}

/* THIS MUST BE AS FAST FUNCTION AS POSSIBLE                NOT USED YET
 */
inline eHandle *eget_handle(
    e_oix oix)
{
    return eglobal->hroot.m_table[oix >> EHANDLE_HANDLE_BITS]->m_handle + (oix & EHANDLE_TABLE_MAX);
}

/* Nicer name for console stream as debug output
 */
#define econsole (*(eglobal->console))


#endif
