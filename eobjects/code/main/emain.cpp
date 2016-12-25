/**

  @file    emain.h
  @brief   Calling eobjects application entry point.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    25.12.2016

  This file implements osal_main() function, which intializes eobject library and calls emain()
  function to start the application.
  
  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Call eobjects application entry point.
  @anchor osal_main

  The osal_main() function is part of etry sequence to eobjects console application.
  It is not used when eobject is used as library nor propablu with GUI applications.

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument plus.

  @return  Integer return value to caller.

****************************************************************************************************
*/
os_int osal_main(
    os_int argc,
    os_char *argv[])
{
    os_int 
        rval;

    /* Initialize eobject library for use.
     */
    eobjects_initialize(OS_NULL);

    /* Call eobjects application main program
     */
    rval = emain(argc, argv);

    /* Shut down eobjects library.
     */
    eobjects_shutdown();

    return rval;
}
