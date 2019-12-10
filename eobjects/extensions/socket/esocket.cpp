/**

  @file    esocket.cpp
  @brief   TCP socket class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  TCP socket class eSocket encodes and buffers data and calls OSAL's stream functions to
  read/write the socket. This class is used by eConnection and eEndPoint classes.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects/extensions/socket/esocket.h"


/**
****************************************************************************************************

  @brief Constructor.
  Clears member variables.

****************************************************************************************************
*/
eSocket::eSocket(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eStream(parent, id, flags)
{
    m_in = m_out = OS_NULL;
    m_socket = OS_NULL;
    m_frame_sz = 1400;
    m_flushnow = OS_FALSE;
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Closes the OS socket if it is open.

****************************************************************************************************
*/
eSocket::~eSocket()
{
    close();
}


/**
****************************************************************************************************

  @brief Add eSocket to class list and class'es properties to it's property set.

  The eSocket::setupclass function adds eSocket to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eSocket::setupclass()
{
    const os_int cls = ECLASSID_SOCKET;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eSocket");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Open a socket.

  The open() function opens a socket. The function can either connect a socket or listen to 
  specific TCP port.

  @param  parameters Socket parameters, a list string or direct value.
		  Address and port to connect to, or interface and port to listen for.
          Socket IP address and port can be specified either as value of "addr" item
          or directly in parameter sstring. For example "192.168.1.55:20" or "localhost:12345"
          specify IPv4 addressed. If only port number is specified, which is often 
          useful for listening socket, for example ":12345".
          IPv4 address is automatically recognized from numeric address like
          "2001:0db8:85a3:0000:0000:8a2e:0370:7334", but not when address is specified as string
          nor for empty IP specifying only port to listen. Use brackets around IP address
          to mark IPv6 address, for example "[localhost]:12345", or "[]:12345" for empty IP.

  @param  flags Flags for creating the socket. Bit fields, combination of:
          - OSAL_STREAM_CONNECT: Connect to specified socket port at specified IP address. 
          - OSAL_STREAM_LISTEN: Open a socket to listen for incoming connections. 
          - OSAL_STREAM_UDP_MULTICAST: Open a UDP multicast socket. 
          - OSAL_STREAM_TCP_NODELAY: Disable Nagle's algorithm on TCP socket.
          - OSAL_STREAM_NO_REUSEADDR: Disable reusability of the socket descriptor.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. 

****************************************************************************************************
*/
eStatus eSocket::open(
	os_char *parameters,
    os_int flags)
{
    osalStatus s;

    /* If socket is already open.
     */
    if (m_socket) return ESTATUS_FAILED;

    /* If we are listening, delete any queues. If connecting, create and open input and 
       output queues. This clears the queues if they were already open.
     */
    setup(flags);

    /* Open socket and return ESTATUS_SUCCESS or ESTATUS_FAILED.
     */
    m_socket = osal_socket_open(parameters, OS_NULL, &s, flags);
    return s ? ESTATUS_FAILED : ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Setup queues/buffering.

  The eSocket::setup function either sets up read and write queues for the socket. T
  If we are setting up for listening socket, delete any queues (normally we do not have any).
  If setting up for connecting or accepting a socket, create and open input and output queues.
  This clears the queues if they were already open.

  @param  flags  Set OSAL_STREAM_CONNECT (0) to set up for connecting socket or accepting socket
          connection. Set bit OSAL_STREAM_LISTEN to set up for listening socket connections
          as end point.
  @return None.

****************************************************************************************************
*/
void eSocket::setup(
    os_int flags)
{
    /* If we are listening, delete any queues.
     */
    if (flags & OSAL_STREAM_LISTEN)
    {
        delete m_in;
        delete m_out;
        m_in = m_out = OS_NULL;
    }

    /* Otherwise connecting or accepting a socket, create the queues.
     */
    else
    {
        if (m_in == OS_NULL) m_in = new eQueue(this);
        if (m_out == OS_NULL) m_out = new eQueue(this);
        m_in->close();
        m_out->close();
        m_in->open(OS_NULL, OSAL_STREAM_DECODE_ON_READ|OSAL_FLUSH_CTRL_COUNT|OSAL_STREAM_SELECT);
        m_out->open(OS_NULL, OSAL_STREAM_ENCODE_ON_WRITE|OSAL_STREAM_SELECT);
    }
}


/**
****************************************************************************************************

  @brief Close a socket.

  The eSocket::close function closes underlaying operating system socket. If socket is not open
  function returns ESTATUS_FAILED and does nothing.

  @return If succesfull, the function returns ESTATUS_SUCCESS (0). Otherwise if socket is not
          open returns ESTATUS failed.

****************************************************************************************************
*/
eStatus eSocket::close()
{
    if (m_socket == OS_NULL) return ESTATUS_FAILED;

    osal_socket_close(m_socket);
    m_socket = OS_NULL;

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Flush written data to socket.

  The eSocket::flush function writes all data in output queue to socket. This uses
  eSocket::select() function, which can also read received data while writing.
  This prevents the socket from getting stick if both ends are writing large amount of data
  at same time.

  @param  flags Ignored for now.
  @return If succesfull, the function returns ESTATUS_SUCCESS (0). Otherwise if socket is not
          open returns ESTATUS failed.

****************************************************************************************************
*/
eStatus eSocket::flush(
    os_int flags)
{
    osalSelectData selectdata;
    eStream *strm;
    eStatus s;

    if (m_socket == OS_NULL) 
    {
        return ESTATUS_FAILED;
    }

    /* Try to write data to socket.
     */
    s = write_socket(OS_TRUE);
    if (s) return s;

    /* Let select handle resut of data transfers. This can also read socket so socket cannot
       get blocked by simultaneous writes from both ends.
     */
    while (m_out->bytes())
    {
        strm = this;
        select(&strm, 1, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (selectdata.errorcode) return ESTATUS_FAILED;
    }

    return osal_stream_flush(m_socket, OSAL_STREAM_DEFAULT)
        ? ESTATUS_FAILED : ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to socket output buffer/to socket.

  The eSocket::write function writes data first to output buffer. Then attempts to write
  data from output buffer into socket, as long as there are full frames and socket would
  not block.

  @return If succesfull, the function returns ESTATUS_SUCCESS (0). Otherwise if socket is not
          open returns ESTATUS failed.

****************************************************************************************************
*/
eStatus eSocket::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    if (m_socket == OS_NULL) 
    {
        *nwritten = 0;
        return ESTATUS_FAILED;
    }

    /* Write all data to queue.
     */
    m_out->write(buf, buf_sz, nwritten);

    /* If we have one frame buffered, try to write data to socket frame at a time.
     */
    return write_socket(OS_FALSE);
}


/**
****************************************************************************************************

  @brief Read data to soket input buffer, fill in by readinf from socket.

  The eSocket::read function first tries to read data from socket input buffer.
  It there is not enough data in input buffer, the function tries to read more
  data from the socket.

  @param  buf Ponter to buffer where to place the data read.
  @param  buf_sz Buffer size in bytes.
  @param  nread Pointer integer into which number of bytes read is stored.
          OS_NULL if not needed.
          Less or equal to buf_sz.
  @param  flags Ignored, set zero for now.

  @return If succesfull, the function returns ESTATUS_SUCCESS (0). Otherwise if error
          the function returns ESTATUS_FAILED.

****************************************************************************************************
*/
eStatus eSocket::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    eStatus s;
    osalSelectData selectdata;
    eStream *strm;
    os_memsz nrd, n;

    if (m_socket == OS_NULL) 
    {
        if (nread) *nread = 0;
        return ESTATUS_FAILED;
    }

#if 1

    /* Try to read socket.
     */
    s = read_socket();
    if (s) return s;

    n = 0;
    while (OS_TRUE)
    {
        /* Try to get from queue.
         */
        m_in->read(buf + n, buf_sz, &nrd);
        buf_sz -= nrd;
        n += nrd;
        buf += nrd;
        if (buf_sz <= 0) break;

        /* Let select handle data transfers.
         */
        strm = this;
        select(&strm, 1, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (selectdata.errorcode)
        {
            if (nread) *nread = n;
            return ESTATUS_FAILED;
        }
    }

    if (nread) *nread = n;
    return ESTATUS_SUCCESS;

#else
    n = 0;
    while (OS_TRUE)
    {
        /* Try to get from queue. XXXXXXXXXXXXXXXXXXXXXXXXXXXX THIS NEEDS TO BE SIMPLIFIED
         */
        m_in->read(buf + n, buf_sz, &nrd);
        buf_sz -= nrd;
        n  += nrd;
        if (buf_sz <= 0) break;

        /* Try to read socket.
         */
        s = read_socket();
        if (s) return s;

        /* Try to get from queue.
         */
        m_in->read(buf, buf_sz, &nrd);
        buf_sz -= nrd;
        n  += nrd;
        if (buf_sz <= 0) break;

        /* Let select handle data transfers.
         */
        strm = this;
        select(&strm, 1, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (selectdata.errorcode)
        {
            if (nread) *nread = n;
            return ESTATUS_FAILED;
        }
    }

    if (nread) *nread = n;
    return ESTATUS_SUCCESS;
#endif
}


/**
****************************************************************************************************

  @brief Write character, typically control code.

  The eSocket::writechar function writes character or control code.

  @param  c Character 0-255 or control code > 255 to write.
  @return If succesfull, the function returns ESTATUS_SUCCESS (0). Other return values indicate
          an error.

****************************************************************************************************
*/
eStatus eSocket::writechar(
    os_int c)
{
    /* Write the character to output queue.
     */
    m_out->writechar(c);

    /* If we have whole frame buffered, try to write data to socket.
     */
    return write_socket(OS_FALSE);
}


/**
****************************************************************************************************

  @brief Read character or control code.

  The eSocket::readchar function reads character or control code.

  @return If succesfull, the function returns Character 0-255. Return value
          E_STREM_END_OF_DATA indicates broken socket.

****************************************************************************************************
*/
os_int eSocket::readchar()
{
    os_int c;
    eStatus s;
    osalSelectData selectdata;
    eStream *strm;

    if (m_socket == OS_NULL) 
    {
        return E_STREM_END_OF_DATA;
    }

    while (OS_TRUE)
    {
        /* Try to get from queue.
         */
        c = m_in->readchar();
        if (c != E_STREM_END_OF_DATA) return c;

        /* Try to read socket.
         */
        s = read_socket();
        if (s) return E_STREM_END_OF_DATA;

        /* Try to get from queue.
         */
        c = m_in->readchar();
        if (c != E_STREM_END_OF_DATA) return c;

        /* Let select handle data transfers.
         */
        strm = this;
        select(&strm, 1, OS_NULL, &selectdata, OSAL_STREAM_DEFAULT);
        if (selectdata.errorcode) return E_STREM_END_OF_DATA;
    }
}
    

/**
****************************************************************************************************

  @brief Wait for socket or thread event.

  The eSocket::select() function waits for socket or thread events. Socket evens are typically
  lilke "read": data can be read from socket, "write": data can be written to socket,
  "connect": Socket connected, "close": Socket closed.
  Thread evens indicate that there are messages to the thread to be processed.

  @param  streams Array of socket stream pointers. This function waits for socket events from
          all these streams.
  @oaram  nstreams Number of items in streams array.
  @param  evnt Operating system event to wait for.
  @param  selectdata Pointer to structure in which to fill information about the event.
          This includes error code.
  @param  flags Reserved, set 0 for now.
  
  @return None.

****************************************************************************************************
*/
void eSocket::select(
	eStream **streams,
    os_int nstreams,
	osalEvent evnt,
	osalSelectData *selectdata,
	os_int flags)
{
    osalStatus s;
    eSocket **sockets, *so;
    osalStream osalsock[OSAL_SOCKET_SELECT_MAX];
    os_int i;

    sockets = (eSocket**)streams;

    if (nstreams == 1)
    {
        s = osal_stream_select(&sockets[0]->m_socket, 1, evnt, 
            selectdata, OSAL_STREAM_DEFAULT); 
    }
    else
    {
        for (i = 0; i<nstreams; i++)
        {
            osalsock[i] = sockets[i]->m_socket;
        }

        osal_stream_select(osalsock, nstreams, evnt,
            selectdata, OSAL_STREAM_DEFAULT); 
    }

    i = selectdata->stream_nr;
    if (selectdata->errorcode == OSAL_SUCCESS &&
        i >= 0 && i < nstreams)
    {
        so = sockets[i];

        if (selectdata->eventflags & OSAL_STREAM_READ_EVENT)
        {
            selectdata->errorcode = so->read_socket();
            if (selectdata->errorcode) return;
        }

        if (selectdata->eventflags & OSAL_STREAM_WRITE_EVENT)
        {
            selectdata->errorcode = so->write_socket(OS_FALSE);
        }
    }
}


/**
****************************************************************************************************

  @brief Accept incoming connection.

  The eSocket::accept() function accepts an incoming connection.

  @param  newstrem Pointer to newly allocated eSocket to set up for this accepted connection.
  @param  flags Reserved, set 0 for now.
  @return ESTATUS_SUCCESS indicates that connection has succesfully been accepted. 
          ESTATUS_NO_NEW_CONNECTION indicates that there were no new connections.
          Other return values indicate an error.

****************************************************************************************************
*/
eStatus eSocket::accept(
    eStream *newstream,
    os_int flags)
{
    osalStatus s;
    osalStream newosalsocket;
    eSocket *sck;

    newosalsocket = osal_stream_accept(m_socket, 
        &s, OSAL_STREAM_DEFAULT); 

    if (newosalsocket)
    {
        sck = eSocket::cast(newstream);
        
        /* Create and open input and output queues. 
         */
        sck->setup(OSAL_STREAM_CONNECT);

        /* Save OSAL socket handle
         */
        sck->m_socket = newosalsocket;

        return ESTATUS_SUCCESS;
    }

    return s == OSAL_STATUS_NO_NEW_CONNECTION 
        ? ESTATUS_NO_NEW_CONNECTION : ESTATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Write from intenal buffer m_out to OSAL socket.

  The eSocket::write_socket() function writes data from m_out queue to socket.
  If flushnow is not set, the function does nothing until m_out holds enough data for at least
  one ethernet frame. All data from m_out queue which can be sent immediately without wait,
  is written to socket. 

  @param  flushnow If  OS_TRUE, even single buffered byte is written. Otherwise waits until 
          enough bytes for ethernet frame are buffered before writing.
  @return If no error detected, the function returns ESTATUS_SUCCESS. 
          Other return values indicate an error and that socket is to be disconnected.

****************************************************************************************************
*/
eStatus eSocket::write_socket(
    os_boolean flushnow)
{
    os_memsz n, nread, nwritten;
    os_char *buf = OS_NULL;
    eStatus s = ESTATUS_SUCCESS;
    osalStatus os;

    m_flushnow |= flushnow;

    while (OS_TRUE)
    {
        n = m_out->bytes();
        if ((n < m_frame_sz && !m_flushnow) || n < 1)
        {
            if (n < 1) m_flushnow = OS_FALSE;
            break;
        }

        if (buf == OS_NULL)
        {
            buf = os_malloc(m_frame_sz, OS_NULL);
        }
   
        m_out->read(buf, m_frame_sz, &nread, OSAL_STREAM_PEEK);
        if (nread == 0) break;

        os = osal_stream_write(m_socket, buf,
            nread, &nwritten, OSAL_STREAM_DEFAULT);
        if (os)
        {
            s = ESTATUS_FAILED;
            break;
        }
        if (nwritten <= 0) break;

        m_out->read(OS_NULL, nwritten, &nread);
    }

    if (buf)
    {
        os_free(buf, m_frame_sz);
    }

    return s;
}


/**
****************************************************************************************************

  @brief Read from OSAL socket into intenal buffer m_in.

  The eSocket::read_socket() function reads data from socket and places it to m_in queue.
  All available data from socket is read.

  @return If no error detected, the function returns ESTATUS_SUCCESS. 
          Other return values indicate an error and that socket is to be disconnected.

****************************************************************************************************
*/
eStatus eSocket::read_socket()
{
    os_memsz nread, nwritten;
    os_char buf[740];
    eStatus s = ESTATUS_SUCCESS;
    osalStatus os;

    while (OS_TRUE)
    {
        os = osal_socket_read(m_socket, buf, sizeof(buf), &nread, OSAL_STREAM_DEFAULT);
        if (os)
        {
            s = ESTATUS_FAILED;
            break;
        }
        if (nread == 0) 
        {
            break;
        }

        m_in->write(buf, nread, &nwritten);
    }

    return s;
}
