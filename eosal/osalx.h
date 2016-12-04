/**

  @file    osalx.h
  @brief   Main OSAL header file with extensions.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This operating system abstraction layer (OSAL) base main header file with extensions. 
  If further OSAL base and extension headers. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_EXTENDED_INCLUDED
#define OSAL_EXTENDED_INCLUDED

/* Include osal base.
 */
#include "eosal/osal.h"

/* If C++ compilation, all functions, etc. from this point on in this header file are
   plain C and must be left undecorated.
 */
OSAL_C_HEADER_BEGINS

/* Include extension headers.
 */
#include "eosal/extensions/clock/osal_clock.h"
#include "eosal/extensions/main/osal_main.h"
#include "eosal/extensions/strcnv/osal_strcnv.h"
#include "eosal/extensions/stream/osal_stream.h"
#include "eosal/extensions/stringx/osal_stringx.h"
#include "eosal/extensions/typeid/osal_typeid.h"

/* If C++ compilation, end the undecorated code.
 */
OSAL_C_HEADER_ENDS

#endif
