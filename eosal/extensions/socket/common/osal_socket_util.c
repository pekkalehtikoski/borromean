/**

  @file    socket/common/osal_socket_util.c
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
#include "eosal/eosalx.h"


/**
****************************************************************************************************

  @brief Get host and port from network address string.
  @anchor osal_socket_get_host_name_and_port

  The osal_socket_get_host_name_and_port() function examines the network address string within 
  long parameter string. If the host name or numeric IP address is specified, the function
  returns pointer to it. If port number is specified in parameter string, the function
  stores it to port.

  @param   parameters Socket parameters, a list string. "addr=host:port" or simply 
           parameter string starting with "host:port", set host name or numeric IP address
           and port number. Host may be in the brackets, like "[host]:port". This is mostly used
           for IP V6 addresses, which themselves may contain colons ':'.
           Marking like ":122" can be used just to specify port number to listen to.
  @param   port Pointer to integer into which to store the port number. If the address
		   parameters do not specify port number, the port will be set to zero.
  @param   buf_sz Pointer to integer into which to store size of allocated temporary buffer.
		   This is used only for releasing memory.
  @param   is_ipv6 Flag to set if IP v6 address has been detected. Notice that if host name
           refers to IPv6 (no numeric address), IPv6 is not detected here.
  @return  If the host was specified, the function returns pointer to null terminated
		   string containing the host name. This buffer must be freed using
		   os_free(buf, buf_sz) function. If parameters do not specify host name,
		   the function returns OS_NULL.

****************************************************************************************************
*/
os_char *osal_socket_get_host_name_and_port(
	os_char *parameters,
	os_int  *port,
	os_memsz *buf_sz,
    os_boolean *is_ipv6)
{
	os_memsz
		n_chars;

	os_char
		*value_pos,
		*port_pos,
		*buf;

	*port = 0;
	*buf_sz = 0;
    *is_ipv6 = OS_FALSE;

	/* Find newtwork address / port within parameter string. 
	 */
    if (parameters == OS_NULL) parameters = "127.0.0.1:" OSAL_DEFAULT_SOCKET_PORT_STR;
	value_pos = osal_string_get_item_value(parameters, "addr", 
		&n_chars, OSAL_STRING_SEARCH_LINE_ONLY);
	if (value_pos == OS_NULL) 
    {
        value_pos = parameters;
        n_chars = os_strlen(value_pos);
    }

	/* Allocate buffer and copy parameter value into it. Null terminate the string.
	 */
	*buf_sz = n_chars + 1;
	buf = os_malloc(*buf_sz, OS_NULL);
	if (buf == OS_NULL) return OS_NULL;
	os_memcpy(buf, value_pos, n_chars);
	buf[n_chars] = '\0';


	/* Search for port number position. If this is ipv4 address or unknown address type, 
	   the port number is separared by ':' or '#'. If this is ipv6 address, port number 
	   must be separated by '#'. The '#' may change, since we get more familiar with ipv6 
	   addresses.
	 */
	port_pos = os_strchr(buf, ']');
    if (port_pos)
    {
        *(port_pos++) = '\0';
        if (*(port_pos++) != ':') port_pos = OS_NULL;
    }
    else
    {
		port_pos = os_strchr(buf, ':');
        if (port_pos) *(port_pos++) = '\0';
    }

	/* Parse port number from string.
	 */
	if (port_pos)
	{
		*port = (os_int)osal_string_to_int(port_pos, OS_NULL);
	}

	/* If starts with bracket, skip it. If host is numeric address which contains colons ':',
       it is IPv6 address. 
	 */
    *is_ipv6 = (os_boolean)(os_strchr(buf, ':') != OS_NULL);
    if (buf[0] == '[') 
    {
        os_memmove(buf, buf+1,  os_strlen(buf+1));
        *is_ipv6 = OS_TRUE;
    }

	/* If we have no host, no buffer needed
	 */
	if (buf[0] == '\0')
	{
		os_free(buf, *buf_sz);
		buf = OS_NULL;
		*buf_sz = 0;
	}

	return buf;
}
