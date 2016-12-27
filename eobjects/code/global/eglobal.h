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


/**
****************************************************************************************************

  @brief Class list structure.

  X...


****************************************************************************************************
*/
typedef struct eGlobal
{
    /* Flags indicating that eobjects library has been initialized.
     */
    os_boolean initialized;

    eHandleRoot hroot;

    /* Process name space
     */
    eNameSpace *process_ns;
}
eGlobal;

/* Global pointer to global structure.
 */
extern eGlobal *eglobal;

#endif
