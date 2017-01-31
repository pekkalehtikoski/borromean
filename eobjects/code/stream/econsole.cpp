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
	e_oid oid,
	os_int flags)
    : eStream(parent, oid, flags)
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
  which is used for serialization reader functions. The property stet stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eConsole::setupclass()
{
    const os_int cls = ECLASSID_CONSOLE;

    /* Add the class to class list.
     */
    osal_mutex_system_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj);
    osal_mutex_system_unlock();
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

  The write () function releases places data into console. The data can be coded.

  @param  buf Pointer to data to write.
  @param  buf_sz Bumber of bytes to write. 
  @param  nwritten Pointer to integer where to store number of bytes written to console. This is
          always same as byte_sz. Can be set to  to OS_NULL, if not needed.

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
        text = (os_char*)osal_memory_allocate(buf_sz+1);
        os_memcpy(text, buf, buf_sz);
        text{buf_sz} = '\0';

        /* Write string to console, and be done.
         */
        osal_console_write(text);        
        osal_memory_free(text, buf_sz+1);
    }

    if (nwritten != OS_NULL) *nwritten = buf_sz; 
    return ESTATUS_SUCCESS;
}



/**
****************************************************************************************************

  @brief Read data from console.

  The read function reads data from console. This is used for actual data, not when control codes
  are expected.

  @param  buf Pointer to buffer into which to read data. If buffer is OS_NULL, up to buf_sz
          bytes are removed from console but not stored anywhere. Null buffer can be used only
          when reading plain data (no OSAL_STREAM_DECODE_ON_READ flag)
  @param  buf_sz Size of buffer in bytes.
  @param  nread Pointer to integer where to store number of bytes read from console. This may be
          less than buffer size if the function runs out of data in console. Can be set to 
          OS_NULL, if not needed. 
  @param  Zero for default operation. Flag OSAL_STREAM_PEEK causes data to be read, but not removed
          from console. This flag works only when reading plain data as is (no 
          OSAL_STREAM_DECODE_ON_READ flag given to open() or accept).

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values
          indicate an error. eConsole class cannot fail, so return value is always 
          ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eConsole::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    return ESTATUS_FAILED;
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
    /* If writing without encoding, just write the character.
     */
    if ((m_flags & OSAL_STREAM_ENCODE_ON_WRITE) == 0) 
    {
        putcharacter(c);
        m_bytes++;
        return ESTATUS_SUCCESS;
    }

    /* Make sure that everything written is in buffer.
     */
    if (m_wr_prevc != ECONSOLE_NO_PREVIOUS_CHAR) 
    {
        complete_last_write();
    }

    switch (c)
    {
        case E_STREAM_BEGIN:
            c = E_STREAM_CTRLCH_BEGIN_BLOCK;
            break;

        case E_STREAM_END:
            c = E_STREAM_CTRLCH_END_BLOCK;
            break;

        case E_STREAM_DISCONNECT:
            c = OSAL_STREAM_CTRLCH_DISCONNECT;
            break;

        default:
            putcharacter(c);
            m_bytes++;
            return ESTATUS_SUCCESS;
    }
    
    putcharacter(E_STREAM_CTRL_CHAR);
    putcharacter(c);
    m_bytes += 2;
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read character or control code from console.

  The readchar function reads either one byte or one control code from console.

  @return Byte of data or control code. Possible control codes are E_STREAM_BEGIN, 
          E_STREAM_END, OSAL_STREAM_DISCONNECT or E_STREM_END_OF_DATA. 
          Version number if returned with E_STREAM_BEGIN, use E_STREAM_CTRL_MASK to
          get only control code.

****************************************************************************************************
*/
os_int eConsole::readchar()
{
    return E_STREM_END_OF_DATA;
}


