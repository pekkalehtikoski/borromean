/**

  @file    eosal/examples/socket_client/code/socket_client_example.c
  @brief   Socket client example.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eosal/eosalx.h"


/**
****************************************************************************************************

  @brief Process entry point.

  The osal_main() function is OS independent entry point.

  @param   argc Number of command line arguments.
  @param   argv Array of string pointers, one for each command line argument. UTF8 encoded.

  @return  None.

****************************************************************************************************
*/
os_int osal_main(
    os_int argc,
    os_char *argv[])
{
    osalStream handle;
    osalStatus status;
    osalSelectData selectdata;
    os_char mystr[] = "eppu", buf[64];
    os_memsz n_read, n_written;

    handle = osal_stream_open(OSAL_SOCKET_IFACE, "127.0.0.1:" OSAL_DEFAULT_SOCKET_PORT_STR,
        OS_NULL, &status, OSAL_STREAM_CONNECT);

    if (status)
    {
	    osal_console_write("osal_stream_open failed\n");
        return 0;
    }

    while (OS_TRUE)
    {
        status = osal_stream_select(&handle, 1, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (status)
        {
	        osal_console_write("osal_stream_select failed\n");
            break;
        }

        if (selectdata.eventflags & OSAL_STREAM_ACCEPT_EVENT)
        {
            osal_console_write("accept event\n");
        }

        if (selectdata.eventflags & OSAL_STREAM_CLOSE_EVENT)
            osal_console_write("close event\n");

        if (selectdata.eventflags & OSAL_STREAM_CONNECT_EVENT)
            osal_console_write("connect event\n");

        if (selectdata.eventflags & OSAL_STREAM_READ_EVENT)
        {
            osal_console_write("read event\n");
            os_memclear(buf, sizeof(buf));
            status = osal_stream_read(handle, buf, sizeof(buf) - 1, &n_read, OSAL_STREAM_DEFAULT);
        }

        if (selectdata.eventflags & OSAL_STREAM_WRITE_EVENT)
        {
            osal_console_write("write event\n");
            status = osal_stream_write(handle, mystr, os_strlen(mystr)-1, &n_written, OSAL_STREAM_DEFAULT);
        }
    }

    osal_stream_close(handle);

    return 0;
}
