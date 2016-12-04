/**

  @file    osal.h
  @brief   Main OSAL header file.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This operating system abstraction layer (OSAL) base main header file. If further includes
  rest of OSAL base headers. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_BASE_INCLUDED
#define OSAL_BASE_INCLUDED

/* Include operating system specific defines.
 */
#ifdef _WIN32
#include "eosal/backbone/defs/windows/osal_defs.h"
#include "eosal/backbone/defs/windows/osal_types.h"
#endif

/* Include definitions common to all operating systems.
 */
#include "eosal/backbone/defs/osal_common_defs.h"

/* If C++ compilation, all functions, etc. from this point on in this header file are
   plain C and must be left undecorated.
 */
OSAL_C_HEADER_BEGINS

/* Include generic OSAL headers.
 */
#include "eosal/backbone/int64/osal_int64.h"
#include "eosal/backbone/memory/osal_memory.h"
#include "eosal/backbone/memory/osal_sysmem.h"
#include "eosal/backbone/string/osal_char.h"
#include "eosal/backbone/string/osal_string.h"
#include "eosal/backbone/defs/osal_status.h"
#include "eosal/backbone/timer/osal_timer.h"
#include "eosal/backbone/debug/osal_debug.h"
#include "eosal/backbone/resmon/osal_resource_monitor.h"
#include "eosal/backbone/console/osal_console.h"
#include "eosal/backbone/console/osal_sysconsole.h"
#include "eosal/backbone/defs/osal_global.h"
#include "eosal/backbone/initialize/osal_initialize.h"
#include "eosal/backbone/mutex/osal_mutex.h"
#include "eosal/backbone/event/osal_event.h"
#include "eosal/backbone/thread/osal_thread.h"
#include "eosal/backbone/utf16/osal_utf16.h"
#include "eosal/backbone/utf32/osal_utf32.h"

/* If C++ compilation, end the undecorated code.
 */
OSAL_C_HEADER_ENDS

#endif
