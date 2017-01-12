/**

  @file    modules/utf16/string/osal_string_utf16_to_utf8_malloc.c
  @brief   String conversion from UTF16 to UTF8 into new allocated buffer.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Conversion of UTF16 encoded string to UTF8 encoded string, buffer for the new string
  is allocated by osal_memory_allocate() function.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/eosal.h"

/* If we need UTF16 support (windows), we need to be ablt to convert between UTF16 and UTF8 
   characters.
 */
#if OSAL_UTF16

/**
****************************************************************************************************

  @brief Convert an UTF16 string to UTF8 string in newly allocated buffer.
  @anchor osal_string_utf16_to_utf8_malloc

  The osal_string_utf16_to_utf8_malloc() converts an UTF16 encoded string to UTF8 encoded string.
  The UTF8 string is stored into buffer allocated using osal_memory_allocate() by this function.

  Code sniplet below demonstrates converting string, and then releasing the conversion buffer.
  \verbatim
  os_char *str;
  os_long sz;

  str = osal_string_utf16_to_utf8_malloc(utf16_str, &sz);
  osal_memory_free(str, sz);
  \endverbatim

  @param   str16 Pointer to UTF16 encoded null terminated string. If OS_NULL, the function
           works as if str16 was en empty string "".
  @param   sz_ptr Pointer to integer into which to store size of allocated buffer in bytes
           OS_NULL if not needed.

  @return  Pointer to UTF8 string (Beginning of allocated memory block).

****************************************************************************************************
*/
os_char *osal_string_utf16_to_utf8_malloc(
    const os_ushort *str16,
    os_memsz *sz_ptr)
{
    os_memsz
        sz;

    os_char
        *buf8;

    /* Calculate buffer size to hold the UTF8 encoded string.
     */
    sz = osal_string_utf16_to_utf8(OS_NULL, 0, str16);

    /* Allocate buffer for the string.
     */
    buf8 = osal_memory_allocate(sz, OS_NULL);

    /* Convert the string from UTF16 to UTF8.
     */
    osal_string_utf16_to_utf8(buf8, sz, str16);

    /* Return the buffer size if needed, and return the
       string pointer.
     */
    if (sz_ptr) *sz_ptr = sz;
    return buf8;
}

#endif