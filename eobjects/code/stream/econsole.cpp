/**

  @file    econsole.cpp
  @brief   Console stream class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Console as eobjects output stream.

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
  Constructs and initializes an empty console object.

****************************************************************************************************
*/
eConsole::eConsole(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eStream(parent, id, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Releases all memory allocated for the console.

****************************************************************************************************
*/
eConsole::~eConsole()
{
}

/**
****************************************************************************************************

  @brief Add the class to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds the class to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eConsole::setupclass()
{
    const os_int cls = ECLASSID_CONSOLE;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eConsole");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Open the console.

  The open() function clears the console into initial state and sets flags for encoding and
  decoding data on read and write.

  @param  parameters Ignored by eConsole.
  @param  flags for eConsole, bit fields:
          - OSAL_STREAM_ENCODE_ON_WRITE: Encode data when writing into console. 
            If flag not set, data is written to console as is.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eConsole class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eConsole::open(
	os_char *parameters,
    os_int flags) 
{
    m_flags = flags;
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Close the console.

  The close() function releases all memory blocks but one and marks console empty.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eConsole class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eConsole::close()
{
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to console.

  The write() function writes data to console. The data can be coded.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nwritten Pointer to integer where to store number of bytes written to console. This is
          always same as byte_sz. Can be set to OS_NULL, if not needed.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eConsole class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eConsole::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    os_char *text;

    /* Write data either encoded or "as is".
     */
    if ((m_flags & OSAL_STREAM_ENCODE_ON_WRITE) == 0)
    {
        /* Allocate copy just to terminate with NULL character.
         */
        text = os_malloc(buf_sz+1, OS_NULL);
        os_memcpy(text, buf, buf_sz);
        text[buf_sz] = '\0';

        /* Write string to console, and be done.
         */
        osal_console_write(text);        
        os_free(text, buf_sz+1);
    }

    if (nwritten != OS_NULL) *nwritten = buf_sz; 
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write character to stream.

  The writechar function writes character or more oftentypically control code to stream.
  Control codes E_STREAM_BEGIN and E_STREAM_END mark beginning and end of object or other block.
  This begin/end marks are needed for versioning and implementing "eUnknown" objects.
  Control character E_STREAM_DISCONNECT indicates that stream (typically socket) is 
  just about to be disconnected.

  @param  c Byte of data or control code to write. Possible control codes are E_STREAM_BEGIN, 
          E_STREAM_END or OSAL_STREAM_DISCONNECT.

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values
          indicate an error. eConsole class cannot fail, so return value is always 
          ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eConsole::writechar(
    os_int c)
{
    os_char text[2];

    /* If writing without encoding, just write the character.
     */
    if ((m_flags & OSAL_STREAM_ENCODE_ON_WRITE) == 0) 
    {
        text[0] = (os_char)c;
        text[1] = '\0';

        /* Write string to console, and be done.
         */
        osal_console_write(text);        
        return ESTATUS_SUCCESS;
    }

    return ESTATUS_SUCCESS;
}
