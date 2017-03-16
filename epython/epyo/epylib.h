/**

  @file    epylib.h
  @brief   Main header eobjects python extension epylib.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EPYLIB_INCLUDED
#define EPYLIB_INCLUDED

/* Include eobjects header.
 */
#include "eobjects/eobjects.h"


/* If C++ compilation, all functions, etc. from this point on in this header file are
   plain C and must be left undecorated.
 */
OSAL_C_HEADER_BEGINS

#include <Python.h>

/* Include generic OSAL headers.
 */
#include "code/epyinit.h"


/* If C++ compilation, end the undecorated code.
 */
OSAL_C_HEADER_ENDS

#endif
