/**

  @file    eosal.h
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
#include "eosal/code/defs/windows/osal_defs.h"
#include "eosal/code/defs/windows/osal_types.h"
#else
#include "eosal/code/defs/linux/osal_defs.h"
#include "eosal/code/defs/linux/osal_types.h"
#endif

/* Include definitions common to all operating systems.
 */
#include "eosal/code/defs/common/osal_common_defs.h"

/* If C++ compilation, all functions, etc. from this point on in this header file are
   plain C and must be left undecorated.
 */
OSAL_C_HEADER_BEGINS

/* Include generic OSAL headers.
 */
#include "eosal/code/int64/common/osal_int64.h"
#include "eosal/code/memory/common/osal_memory.h"
#include "eosal/code/memory/common/osal_sysmem.h"
#include "eosal/code/string/common/osal_char.h"
#include "eosal/code/string/common/osal_string.h"
#include "eosal/code/defs/common/osal_status.h"
#include "eosal/code/timer/common/osal_timer.h"
#include "eosal/code/debugcode/common/osal_debug.h"
#include "eosal/code/resmon/common/osal_resource_monitor.h"
#include "eosal/code/console/common/osal_console.h"
#include "eosal/code/console/common/osal_sysconsole.h"
#include "eosal/code/defs/common/osal_global.h"
#include "eosal/code/initialize/common/osal_initialize.h"
#include "eosal/code/mutex/common/osal_mutex.h"
#include "eosal/code/event/common/osal_event.h"
#include "eosal/code/thread/common/osal_thread.h"
#include "eosal/code/utf16/common/osal_utf16.h"
#include "eosal/code/utf32/common/osal_utf32.h"

/* If C++ compilation, end the undecorated code.
 */
OSAL_C_HEADER_ENDS

#endif
