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
#ifndef EMAIN_INCLUDED
#define EMAIN_INCLUDED

/* If C++ compilation, all functions, etc. from this point on in this header file are
   plain C and must be left undecorated.
 */
OSAL_C_HEADER_BEGINS

/* Prototype for application's entry point function.
 */
os_int emain(
    os_int argc,
    os_char *argv[]);

/* If C++ compilation, end the undecorated code.
 */
OSAL_C_HEADER_ENDS

#endif
