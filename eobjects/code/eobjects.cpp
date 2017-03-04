/**

  @file    eobjects.cpp
  @brief   Main eobject library C file.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This eobject library base main header file. If further includes rest of eobjects base 
  headers.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Initialize eobject library for use.
  @anchor eobjects_initialize

  The eobjects_initialize() function initializes eobjects library for use. This function should
  be the first eobjects function called. The eobjects_shutdown() function cleans up resources
  used by the library.

  @param  reserved For now OS_NULL.
  @return None.

****************************************************************************************************
*/
void eobjects_initialize(
    void *reserved)
{
    /* Do nothing if the library has been initialized.
     */
    if (eglobal->initialized) return;

    /* Clear the global strcture and mark initialized.
     */
    os_memclear(eglobal, sizeof(eGlobal));
    eglobal->initialized = OS_TRUE;

    /* Initialize operating system abstraction layer.
     */
    osal_initialize(OSAL_INIT_DEFAULT);

    /* Initialize handle tables.
     */
    ehandleroot_initialize();

    /* Initialize class list
     */
    eclasslist_initialize();

    /* Add console stream for debugging.
     */
    eglobal->console = new eConsole();
}


/**
****************************************************************************************************

  @brief Shut down eobjects library.
  @anchor osal_shutdown

  The eobjects_shutdown() function cleans up resources used by the eobjects library.

  @return  None.

****************************************************************************************************
*/
void eobjects_shutdown()
{
    /* Do nothing if the library has not been initialized.
     */
    if (!eglobal->initialized) return;

    /* Delete debugging console stream.
     */
    delete eglobal->console;

    /* Release resources allocated for the class list.
     */
    eclasslist_release();

    /* Delete handle tables.
     */
    ehandleroot_shutdown();

    /* Mark eobjects library uninitialized.
     */
    eglobal->initialized = OS_FALSE;

    /* Release resources allocated by operating system abstraction layer.
     */
    osal_shutdown();
}

