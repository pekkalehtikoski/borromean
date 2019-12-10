/**

  @file    efile.cpp
  @brief   Stream IO for files.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Reading and writing files as a stream.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Constructor.
  The constructor clears member variables.

****************************************************************************************************
*/
eFile::eFile(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eStream(parent, id, flags)
{
    m_handle = OS_NULL;
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Releases memory allocated for the file.

****************************************************************************************************
*/
eFile::~eFile()
{
    close();
}


/**
****************************************************************************************************

  @brief Open the file as stream.
  The open() function sets read position to zero.

  @param  parameters Path to file.
  @param  flags Bit fields: OSAL_STREAM_READ, OSAL_STREAM_WRITE, OSAL_STREAM_RW,
          OSAL_STREAM_APPEND.

  @return If successfull, the function returns ESTATUS_SUCCESS (0). Other return values
          indicate an error.

****************************************************************************************************
*/
eStatus eFile::open(
	os_char *parameters,
    os_int flags) 
{
    osalStatus status;

    close();

    m_handle = osal_stream_open(OSAL_FILE_IFACE, parameters, OS_NULL, &status, flags);
    return status ? ESTATUS_FAILED : ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Close the stream.
  The close() function does nothing for eFile.

  @return If successfull, the function returns ESTATUS_SUCCESS (0). Other return values
          indicate an error.

****************************************************************************************************
*/
eStatus eFile::close()
{
    if (m_handle)
    {
        osal_stream_close(m_handle);
        m_handle = OS_NULL;
    }
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Flush written data to stream.
  The flush() function..

  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
/* Flush written data to stream.
 */
eStatus eFile::flush(
    os_int flags)
{
    osalStatus status;
    if (m_handle)
    {
        status = osal_stream_flush(m_handle, flags);
        return status ? ESTATUS_FAILED : ESTATUS_SUCCESS;
    }
    return ESTATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Write data to file.

  The write() function appends data into file.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nwritten Pointer to integer where to store number of bytes written to queue. This is
          always same as byte_sz. Can be set to OS_NULL, if not needed.

  @return If successfull, the function returns ESTATUS_SUCCESS (0). Other return values
          indicate an error.

****************************************************************************************************
*/
eStatus eFile::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    os_memsz nwr = 0;
    osalStatus status;
    eStatus rval = ESTATUS_FAILED;

    /* If we got handle, try to write.
     */
    if (m_handle != OS_NULL)
    {
        status = osal_stream_write(m_handle, buf, buf_sz, &nwr, 0);
        if (status == OSAL_SUCCESS && nwr == buf_sz) rval = ESTATUS_SUCCESS;
    }

    /* Return number of bytes written and status code.
     */
    if (nwritten != OS_NULL) *nwritten = nwr;
    return rval;
}


/**
****************************************************************************************************

  @brief Read data from file.

  The read function reads data from file.

  @param  buf Pointer to buffer into which to read data.
  @param  buf_sz Size of buffer in bytes.
  @param  nread Pointer to integer where to store number of bytes read from queue. This may be
          less than buffer size if the function runs out of data in file. Can be set to
          OS_NULL, if not needed. 
  @param  flags Ignored.

  @return If successfull, the function returns ESTATUS_SUCCESS. Other if no more data is
          available for reading in buffer, the function returns ESTATUS_STREAM_END.

****************************************************************************************************
*/
eStatus eFile::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    os_memsz nrd = 0;
    osalStatus status;
    eStatus rval = ESTATUS_FAILED;

    /* If we got handle, try to read.
     */
    if (m_handle != OS_NULL)
    {
        status = osal_stream_read(m_handle, (os_uchar*)buf, buf_sz, &nrd, 0);
        if (status == OSAL_SUCCESS) rval = ESTATUS_SUCCESS;
        if (status == OSAL_END_OF_FILE) rval = ESTATUS_STREAM_END;
    }

    /* Return number of bytes read and status code.
     */
    if (nread != OS_NULL) *nread = nrd;
    return rval;
}
