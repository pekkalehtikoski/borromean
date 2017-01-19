/**

  @file    eosal/examples/socket_server/code/socket_server_example.c
  @brief   Socket server example.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This example demonstrates how to use console.

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
    osalStream handle[OSAL_SOCKET_SELECT_MAX];
    osalStatus status;
    osalSelectData selectdata;
    os_int i;

    os_memclear(handle, sizeof(handle));

    handle[0] = osal_stream_open(OSAL_SOCKET_IFACE, ":" OSAL_DEFAULT_SOCKET_PORT_STR,
        OS_NULL, &status, OSAL_STREAM_LISTEN);
    if (status)
    {
	    osal_console_write("osal_stream_open failed\n");
        return 0;
    }

    while (OS_TRUE)
    {
        status = osal_stream_select(handle, OSAL_SOCKET_SELECT_MAX, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (status)
        {
	        osal_console_write("osal_stream_select failed\n");
            break;
        }

        if (selectdata.eventflags & OSAL_STREAM_ACCEPT_EVENT)
        {
            osal_console_write("accept event\n");
            /* Find free handle
             */
            for (i = 1; i < OSAL_SOCKET_SELECT_MAX; i++)
            {
                if (handle[i] == OS_NULL)
                {
                    handle[i] = osal_stream_accept(handle[0], OS_NULL, &status, OSAL_STREAM_DEFAULT);
                    break;
                }
            }
            if (i == OSAL_SOCKET_SELECT_MAX)
            {
	            osal_console_write("Failed: handles table full\n");
            }
        }

        if (selectdata.eventflags & OSAL_STREAM_CLOSE_EVENT)
        {
            osal_console_write("close event\n");
            osal_stream_close(handle[selectdata.stream_nr]);
            handle[selectdata.stream_nr] = OS_NULL;
        }

        if (selectdata.eventflags & OSAL_STREAM_CONNECT_EVENT)
            osal_console_write("connect event\n");

        if (selectdata.eventflags & OSAL_STREAM_READ_EVENT)
            osal_console_write("read event\n");

        if (selectdata.eventflags & OSAL_STREAM_WRITE_EVENT)
            osal_console_write("write event\n");
    }

    osal_stream_close(handle[0]);

	// osal_console_write("osal_console_example\npress any key... ");

    return 0;
}
