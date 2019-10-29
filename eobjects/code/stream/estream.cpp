/**

  @file    estream.cpp
  @brief   Stream base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Stream base class sets up general way to interace with different types of streams.

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

  X...

  @return  None.

****************************************************************************************************
*/
eStream::eStream(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eStream::~eStream()
{
}


/**
****************************************************************************************************

  @brief Write one character

  This default implementation calls class'es write function to write one character.

  @param  c Character to write.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::writechar(
    os_int c)
{
    os_memsz nwritten;
    os_char buf;
    eStatus rval;

    buf = (os_char)c;

    rval = write(&buf, 1, &nwritten);
    if (nwritten != 1) rval = ESTATUS_FAILED;

    return rval;
}


/**
****************************************************************************************************

  @brief Write one character

  This default implementation calls class'es read function to read one character.

  @return If successfull, the function returns the character. Otherwise if failed returns
          E_STREAM_DISCONNECT.

****************************************************************************************************
*/
os_int eStream::readchar()
{
    os_memsz nread;
    os_uchar buf;
    os_int c = E_STREAM_DISCONNECT;
    eStatus rval;

    rval = read((os_char*)&buf, 1, &nread);
    if (rval == ESTATUS_SUCCESS && nread == 1) c = buf;

    return c;
}


/**
****************************************************************************************************

  @brief Read until end of object or other block.

  The read_end_block() function reads from the stream until end of block character code is 
  found. This skips data added by later versions of object.

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::read_end_block() 
{
    while (OS_TRUE) switch (readchar() & E_STREAM_CTRL_MASK)
    {
        case E_STREAM_END:
            return ESTATUS_SUCCESS;
        
        case E_STREM_END_OF_DATA:
            return ESTATUS_FAILED;
    }
}


/**
****************************************************************************************************

  @brief Store integer value to stream.

  The eStream::putl function stores long integer value to stream. Value is packed in
  serialization format to strem. Serialization format is processsor architecture independent
  and packed to save space.

  @param  x Integer value to put to stream.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::putl(
    os_long x)
{
    os_char buf[OSAL_INTSER_BUF_SZ];
    os_int bytes;

    /* Pack integer value into serialization format and write it.
     */
    bytes = osal_intser_writer(buf, x);
    return write(buf, bytes);
}


/**
****************************************************************************************************

  @brief Store floating point value to stream.

  The eStream::putf function stores single precision floating point value to stream. 
  Value is packed in serialization format to strem. Serialization format is processsor 
  architecture independent and packed to save space.

  @param  x Float value to put to stream.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::putf(
	os_float x)
{
    os_long m;
    os_short e;
    os_char buf[2*OSAL_INTSER_BUF_SZ];
    os_int bytes, bytes2;

    /* Split float to mantissa and exponent.
     */
    osal_float2ints(x, &m, &e);

    /* Pack mantissa and exponent (unless value is zero) to serialization format.
     */
    bytes = osal_intser_writer(buf, m);
    if (bytes && m)
    {
        bytes2 = osal_intser_writer(buf + bytes, e);
        bytes += bytes2;
    }
    
    /* Write to stream.
     */
    return write(buf, bytes);
}


/**
****************************************************************************************************

  @brief Store double precision floating point value to stream.

  The eStream::putd function stores double precision floating point value to stream. 
  Value is packed in serialization format to strem. 

  @param  x Double value to put to stream.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::putd(
	os_double x)
{
    os_long m;
    os_short e;
    os_char buf[2*OSAL_INTSER_BUF_SZ];
    os_int bytes, bytes2;

    /* Split double to mantissa and exponent.
     */
    osal_double2ints(x, &m, &e);

    /* Pack mantissa and exponent to serialization format.
     */
    bytes = osal_intser_writer(buf, m);
    if (bytes && m)
    {
        bytes2 = osal_intser_writer(buf + bytes, e);
        bytes += bytes2;
    }
    
    /* Write to stream.
     */
    return write(buf, bytes);
}


/**
****************************************************************************************************

  @brief Store string to stream.

  The eStream::puts function stores a string value to stream. Value is packed in serialization
  format to strem. 

  @param  x String value to put to stream.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::puts(
	const os_char *x)
{
    os_memsz bytes;
    eStatus rval;

    /* Get string length, write string length and then the string to stream.
     */
    bytes = os_strlen(x) - 1;
    rval = putl(bytes);
    if (rval == ESTATUS_SUCCESS && bytes > 0) 
    {
        rval = write(x, bytes);
    }

    return rval;
}


/**
****************************************************************************************************

  @brief Store string to stream.

  The eStream::puts function stores a string value from variable to stream. If variable contains
  other data type than string, value is converted to string. Value is packed in serialization
  format to strem. 

  @param  x Variable containing value to put to stream as string.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::puts(
	eVariable *x)
{
    os_char *str;
    os_memsz bytes;
    eStatus rval;
    os_boolean tmpbuf;

    /* Get string length and write string to stream.
       Free gets() buffer if it was not allocated already before this call.
       We compare to bytes > 1, because we do not bother to write a lone 
       terminating NULL char.
     */
    tmpbuf = x->tmpstrallocated();
    str = x->gets(&bytes);
    rval = putl(--bytes);
    if (rval == ESTATUS_SUCCESS && bytes > 0) 
    {
        rval = write(str, bytes);
    }

    if (!tmpbuf) x->gets_free();
    return rval;
}


/**
****************************************************************************************************

  @brief Get integer value from stream.

  The eStream::getl function reads long integer value from stream. Value is converted from
  packed serialization format.

  @param  x Pointer where to store the integer value.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::getl(
    os_long *x)
{
    os_char buf[OSAL_INTSER_BUF_SZ];
    eStatus rval;
    os_int more;

    /* Get first byte to get length of the rest
     */
    rval = read(buf, 1);
    if (rval) goto getout;

    /* Read as much more dat as there is to this number.
     */
    more = osal_intser_more_bytes(buf[0]);
    if (more) 
    {
        rval = read(buf+1, more);
        if (rval) goto getout;
    }

    /* Convert serialization format to long and return
     */
    osal_intser_reader(buf, x);
    return ESTATUS_SUCCESS;

getout:
    *x = 0;
    return rval;
}


/**
****************************************************************************************************

  @brief Get floating point value from stream.

  The eStream::getf function reads single precision floating point value from stream. Value is 
  converted from packed serialization format.

  @param  x Pointer where to store the floating point value.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::getf(
    os_float *x)
{
    os_long m, e;
    eStatus rval;

    /* Read mantissa.
     */
    rval = getl(&m);
    if (rval) goto getout;

    /* If zero, this is zero value
     */
    if (m == 0)
    {
        *x = 0;
        return ESTATUS_SUCCESS;
    }

    /* Read exponent, optional.
     */
    rval = getl(&e);
    if (rval) goto getout;

    /* Convert to float
     */
    if (!osal_ints2float(x, m, (os_short)e))
    {
        rval = ESTATUS_STREAM_FLOAT_ERROR;
    }
    return rval;

getout:
    *x = 0;
    return rval;
}


/**
****************************************************************************************************

  @brief Get double precision floating point value from stream.

  The eStream::getd function reads double precision floating point value from stream. Value is 
  converted from packed serialization format.

  @param  x Pointer where to store the floating point value.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::getd(
    os_double *x)
{
    os_long m, e;
    eStatus rval;

    /* Read mantissa.
     */
    rval = getl(&m);
    if (rval) goto getout;

    /* If zero, this is zero value
     */
    if (m == 0)
    {
        *x = 0;
        return ESTATUS_SUCCESS;
    }

    /* Read exponent, optional.
     */
    rval = getl(&e);
    if (rval) goto getout;

    /* Convert to float
     */
    if (!osal_ints2double(x, m, (os_short)e))
    {
        rval = ESTATUS_STREAM_FLOAT_ERROR;
    }
    return rval;

getout:
    *x = 0;
    return rval;
}


/**
****************************************************************************************************

  @brief Get string value point value from stream.

  The eStream::gets function string value from stream and stores it into variable x. Value is 
  converted from packed serialization format.

  @param  x Pointer to variable into which to store the string value.
  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values 
          indicate an error.

****************************************************************************************************
*/
eStatus eStream::gets(
    eVariable *x)
{
    os_char *buf;
    os_long bytes;
    eStatus rval;

    rval = getl(&bytes);
    if (rval || bytes <= 0) goto getout;

    buf = x->allocate(bytes);
    rval = read(buf, bytes);
    if (rval) goto getout;
    return rval;

getout:
    x->sets(OS_NULL);
    return rval;
}
