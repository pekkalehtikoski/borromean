/**

  @file    initialize/common/osal_initialize.h
  @brief   OSAL initialization and shut down.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  OSAL library initialization, shut down and pointer to global OSAL state structure.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_INITIALIZE_INCLUDED
#define OSAL_INITIALIZE_INCLUDED


/**
****************************************************************************************************

  @name Pointer to Global OSAL State Structure.

  The state structure holds global OSAL state. The stucture is always accessed trough this
  pointer so that DLLs are able to share the state structure of the process which loaded
  them.

****************************************************************************************************
*/
/*@{*/

/* Pointer to global OSAL state structure.
 */
extern osalGlobalStruct *osal_global;
/*@}*/


/**
****************************************************************************************************

  @name Initialization and Shut Down Functions

  The osal_initialize() function initializes OSAL library for use. This function should
  be the first OSAL function called. The osal_shutdown() function cleans up resources
  used by the OSAL library.

****************************************************************************************************
 */
/*@{*/

/* Initialize OSAL library for use.
 */
void osal_initialize(
    void);

/* Shut down OSAL libeary, clean up.
 */
void osal_shutdown(
    void);

/*@}*/

#endif
