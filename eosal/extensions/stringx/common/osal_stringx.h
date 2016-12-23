/**

  @file    common/osal_string.h
  @brief   String manipulation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file contains functions prototypes for string manipulation.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef OSAL_STRING_INCLUDED
#define OSAL_STRING_INCLUDED


/** 
****************************************************************************************************

  @name Flags For String Functions

  The ...

****************************************************************************************************
 */
/*@{*/

/** Default flag. Use the OSAL_STRING_DEFAULT (0) to specify that there are no specifal flags.
 */
#define OSAL_STRING_DEFAULT 0

/** Search for item name only. If specified the osal_string_find_string() function will 
    search only for item name, whole word, not value.
 */
#define OSAL_STRING_SEARCH_ITEM_NAME 1 

/** Search first line only. If specified the osal_string_find_string(), 
    osal_string_get_item_value(), osal_string_get_item_int()...  functions will search only
	first line of the sting.
 */
#define OSAL_STRING_SEARCH_LINE_ONLY 2

/* #define OSAL_STRING_SEARCH_ITEM_ONLY 4 */

/*@}*/

/** 
****************************************************************************************************

  @name String Manipulation Functions

  The ...

****************************************************************************************************
 */
/*@{*/

/* Copy string.
 */
void os_strncpy(
    os_char *dst,
    const os_char *src,
    os_memsz dst_size);

/* Append a string to another string.
 */
void os_strncat(
    os_char *dst,
    const os_char *src,
    os_memsz dst_size);

/* Compare two strings, case sensitive.
 */
os_int os_strcmp(
    const os_char *str1,
    const os_char *str2);

/* Get string size in bytes.
 */
os_memsz os_strlen(
    const os_char *str);

/* Find a character within string.
 */
os_char *os_strchr(
    os_char *str,
	os_uint c32);

/* Find a substring within a string. Used also to find named items from list.
 */
os_char *osal_string_find_string(
    os_char *str,
	const os_char *substr,
	os_short flags);

/* Find a value of specified list item.
 */
os_char *osal_string_get_item_value(
    os_char *list_str,
	const os_char *item_name,
	os_memsz *n_chars,
	os_short flags);

/* Find beginning of next line in list string.
 */
os_char *osal_string_get_next_line(
    os_char *list_str);

/* Get integer value of an item in list string.
 */
os_long osal_string_get_item_int(
    os_char *list_str,
	const os_char *item_name,
	os_long default_value,
	os_short flags);

/* Convert integer to string.
 */
os_memsz osal_int_to_string(
    os_char *buf,
	os_memsz buf_sz,
	os_long x);

/* Convert string to integer.
 */
os_long osal_string_to_int(
    os_char *str,
	os_memsz *count);

/*@}*/

#endif
