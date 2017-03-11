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
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Releases memory allocated for the file.

****************************************************************************************************
*/
eFile::~eFile()
{
}


/**
****************************************************************************************************

  @brief Open the file as stream.
  The open() function sets read position to zero.

  @param  parameters Ignored by eFile.
  @param  flags Ignored by eFile.
  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eFile::open(
	os_char *parameters,
    os_int flags) 
{
}


/**
****************************************************************************************************

  @brief Close the stream.
  The close() function does nothing for eFile.

  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eFile::close()
{
}


/**
****************************************************************************************************

  @brief Write data to file.

  The write() function appends data into file.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nwritten Pointer to integer where to store number of bytes written to queue. This is
          always same as byte_sz. Can be set to OS_NULL, if not needed.

  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eFile::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    /* Return number of bytes written and success code.
     */
//    if (nwritten != OS_NULL) *nwritten = buf_sz;
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read data from file.

  The read function reads data from file.

  @param  buf Pointer to buffer into which to read data.
  @param  buf_sz Size of buffer in bytes.
  @param  nread Pointer to integer where to store number of bytes read from queue. This may be
          less than buffer size if the function runs out of data in queue. Can be set to 
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

    /* Return number of bytes read and success code.
     */
  //  if (nread != OS_NULL) *nread = buf_sz;
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write character to file.

  The writechar function appends one character to file.

  @param  c Character to write.
  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eFile::writechar(
    os_int c)
{
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read character from file.

  The readchar function reads one characted from file.

  @return If no more data available, the function returns E_STREM_END_OF_DATA.
          Otherwise if we have data the function returns next character to read.

****************************************************************************************************
*/
os_int eFile::readchar()
{
    eStatus s;
    os_char buf;
    os_memsz nread;

    s = read(&buf, 1, &nread);
    /* If no data available.
     */
//    if (m_pos == m_used) return E_STREM_END_OF_DATA;

    /* Return the characted.
     */
    return s;
}


