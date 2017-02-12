/**

  @file    windows/osal_defs.h
  @brief   Operating system specific defines for Windows.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    25.5.2016

  This file contains platform specific defines for windows compilation. The platform specific 
  defines here are defaults, which can be overwritten by compiler settings.

  Copyright 2016 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_DEFS_INCLUDED
#define OSAL_DEFS_INCLUDED

/** Operating system identifier define. There is define for each supported operating system,
    for example OEAL_WINDOWS, OSAL_LINUX, OSAL_VXWORKS, OSAL_NETOS... Compilation can be conditioned
	by the define, for example "#ifdef OSAL_LINUX". 
 */
#define OSAL_WINDOWS 1

/** Generic include debug code in compilation. The debug code checks for programming errors.
 */
#ifndef OSAL_DEBUG
#define OSAL_DEBUG 1
#endif

/** Include memory debug code. If OSAL_MEMORY_DEBUG flags is nonzero, the memory
    block overwflows and freed block size are checked.
 */
#ifndef OSAL_MEMORY_DEBUG
#define OSAL_MEMORY_DEBUG 1
#endif

/** OSAL memory allocation manager flag. If OSAL_MEMORY_MANAGER flags is nonzero, then
    memory is allocated through OSAL memory management. If this flag is zero, then
    operating system memory allocation is called directly.
 */
#ifndef OSAL_MEMORY_MANAGER
#define OSAL_MEMORY_MANAGER 1
#endif

/** Include resource monitor code. If OSAL_RESOURCE_MONITOR flags is nonzero, code for
    monitoring operating system resource use is included.
 */
#ifndef OSAL_RESOURCE_MONITOR
#if OSAL_DEBUG
#define OSAL_RESOURCE_MONITOR 1
#else
#define OSAL_RESOURCE_MONITOR 0
#endif
#endif

/** Include code to force os_lock() to switch to time critical priority
    when in system mutex is locked. This can be used on systems which do not support
	priority inheritance to avoid priority reversal, like desktop windows. Anyhow setting
	this option slows the code down. On systems which do support priority inheritance,
	like Windows mobile, ThreadX, etc. this should be set to zero.
 */
#ifndef OSAL_TIME_CRITICAL_SYSTEM_LOCK 
#define OSAL_TIME_CRITICAL_SYSTEM_LOCK 0
#endif

/** Include UTF8 character encoding support, define 1 or 0. For most systems UTF8 character 
    encoding should be enabled. Disale UTF8 only for very minimalistic systems which need 
	only English strings and where every extra byte counts. 
 */
#ifndef OSAL_UTF8
#define OSAL_UTF8 1
#endif

 /** Include UTF16 character encoding support, define 1 or 0. For most systems UTF16
     character encoding should be disabled. Enable UTF16 for Windows Unicode support.
     Notice that OSAL_UTF8 must be enbaled to enable OSAL_UTF16.
 */
#ifndef OSAL_UTF16
#define OSAL_UTF16 1
#endif

/** Enable or disable console support.
 */
#ifndef OSAL_CONSOLE
#define OSAL_CONSOLE 1
#endif

/** Short name referring operating system and compilation. For example "win32", "linux32", 
    "linux64", "vxworks", "netos"... There can be several names for same operating system.
	This is useful for example is two compilers are used for the same operating system, 
	or there are multiple processor architectures, and we need to maintain compiler output
	for several of these. 
 */
#ifndef OSAL_BIN_NAME
#define OSAL_BIN_NAME "windows"
#endif

/** Default file system root. This is path to default root of the file system.
 */
#ifndef OSAL_FS_ROOT
#define OSAL_FS_ROOT "c:/"
#endif

/** Endianess. Define "OSAL_SMALL_ENDIAN 1" for small endian processors and 0 for big endian
    processors.
 */
#ifndef OSAL_SMALL_ENDIAN
#define OSAL_SMALL_ENDIAN 1
#endif

/** Needed memory alignment. Some processors require that variables are allocated at 
    type size boundary. For example ARM7 requires that 2 byte integers starts from 
	pair addressess and 4 byte integers from addressess dividable by four. If so, 
	define "OSAL_MEMORY_TYPE_ALIGNMENT 4". If no aligment is needed define 0 here.
 */
#ifndef OSAL_MEMORY_TYPE_ALIGNMENT
#define OSAL_MEMORY_TYPE_ALIGNMENT 0
#endif

/** Support for 64 bit integers. Define 1 if compiler supports 64 bit integers, like "__int64"
    or "long long" or plain "long" in 64 bit GNU compilers. Define 0 if compiler doesn't 
	have 64 bit support.
 */
#ifndef OSAL_LONG_IS_64_BITS
#define OSAL_LONG_IS_64_BITS 1
#endif

/** OSAL proces cleanup code needed flag. If OSAL_PROCESS_CLEANUP_SUPPORT flags is nonzero, 
    then code to do memory, etc. cleanup when process exists or restarts is included. 
	If this flag is zero, then cleanup code is not included. The cleanup code should 
	be included in platforms like Windows or Linux, where processes can be terminated
    or restarted. Memory cleanup code is not necessary on most of embedded systems,
    disabling it saves a few bytes of memory.
 */
#ifndef OSAL_PROCESS_CLEANUP_SUPPORT
#define OSAL_PROCESS_CLEANUP_SUPPORT 1
#endif

/** Multithreading support. Define 1 if operating system supports multi threading. This
    enables code for thread, mutexes, event, etc. Define 0 if there is no multithreading 
	support for this operating system.
 */
#ifndef OSAL_MULTITHREAD_SUPPORT
#define OSAL_MULTITHREAD_SUPPORT 1
#endif

/** If compiler can support function pointers and interfaces, define 1. Define zero
    only for systems which do not handle function pointers properly (for example PIC).
 */
#ifndef OSAL_FUNCTION_POINTER_SUPPORT
#define OSAL_FUNCTION_POINTER_SUPPORT 1
#endif

 /** If compiler can support recursive function calls, define 1. Define zero
     only for micro controllers which do not have stack (for example PIC).
  */
#ifndef OSAL_RECURSION_SUPPORT
#define OSAL_RECURSION_SUPPORT 1
#endif

#endif
