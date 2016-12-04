/**

  @file    modules/socket/osal_socket_util.c
  @brief   Socket helper functions.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Socket helper functions common to all operating systems.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/osalx.h"


/**
****************************************************************************************************

  @brief Get host and port from network address string.
  @anchor osal_socket_get_host_name_and_port

  The osal_socket_get_host_name_and_port() function examines the network address string within 
  long parameter string. If the host name or numeric IP address is specified, the function
  returns pointer to it. If port number is specified in parameter string, the function
  stores it to port.

  @param   parameters Socket parameters, a list string. "addr=host:port" sets host name or
	       numeric address and port number.
  @param   port Pointer to integer into which to store the port number. If the address
		   parameters do not specify port number, the port will be set to zero.
  @param   buf_sz Pointer to integer into which to store size of allocated temporary buffer.
		   This is used only for releasing memory.

  @return  If the host was specified, the function returns pointer to null terminated
		   string containing the host name. This buffer must be freed using
		   osal_memory_free(buf, buf_sz) function. If parameters do not specify host name,
		   the function returns OS_NULL.

****************************************************************************************************
*/
os_char *osal_socket_get_host_name_and_port(
	os_char *parameters,
	os_int  *port,
	os_memsz *buf_sz)
{
	os_memsz
		n_chars;

	os_char
		*value_pos,
		*port_pos,
		*is_ipv6, 
		*buf;

	os_boolean
		no_host;

	*port = 0;
	*buf_sz = 0;

	/* Find newtwork address / port within parameter string. 
	 */
	value_pos = osal_string_get_item_value(parameters, "addr", 
		&n_chars, OSAL_STRING_SEARCH_LINE_ONLY);
	if (value_pos == OS_NULL) return OS_NULL;

	/* Allocate buffer and copy parameter value into it. Null terminate the string.
	 */
	*buf_sz = n_chars + 1;
	buf = osal_memory_allocate(*buf_sz, OS_NULL);
	if (buf == OS_NULL) return OS_NULL;
	os_memcpy(buf, value_pos, n_chars);
	buf[n_chars] = '\0';

	/* If this has two or more colons, this must be ipv6 address. 
	 */
	is_ipv6 = os_strchr(buf, ':');
	if (is_ipv6) is_ipv6 = os_strchr(is_ipv6+1, ':');

	/* Search for port number position. If this is ipv4 address or unknown address type, 
	   the port number is separared by ':' or '#'. If this is ipv6 address, port number 
	   must be separated by '#'. The '#' may change, since we get more familiar with ipv6 
	   addresses.
	 */
	port_pos = os_strchr(buf, '#');
	if (port_pos == OS_NULL && is_ipv6==OS_NULL) 
		port_pos = os_strchr(buf, ':');
	no_host = OS_FALSE;

	/* If we found separator character for port number position.
	 */
	if (port_pos)
	{
		/* If separator is first character of the address, we have no host name.
		 */
		if (port_pos == buf) no_host = OS_TRUE;

		/* Cut port number out of host name.
		 */
		*(port_pos++) = '\0';
	}

	/* If we didn't find port position, this may be port number only, without any IP address.
	 */
	else if (!is_ipv6 && osal_char_isdigit(*buf) && os_strchr(buf, '.') == OS_NULL)
	{
		port_pos = buf;
		no_host = OS_TRUE;
	}

	/* Parse port number from string.
	 */
	if (port_pos)
	{
		*port = (os_int)osal_string_to_int(port_pos, OS_NULL);
	}

	/* If we have no host, no byffer needed
	 */
	if (no_host)
	{
		osal_memory_free(buf, *buf_sz);
		buf = OS_NULL;
		*buf_sz = 0;
	}

	return buf;
}
