/**

  @file    ebuffer.cpp
  @brief   Memory buffer.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Flat memory buffer, can be used as a stream.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"

/** Default queue block size.
 */
#define eBuffer_DEFALT_BLOCK_SZ 512

/** Value to indicate no previous character.
 */
#define eBuffer_NO_PREVIOUS_CHAR 256


/**
****************************************************************************************************

  @brief Constructor.
  The constructor clears member variables.

****************************************************************************************************
*/
eBuffer::eBuffer(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eStream(parent, id, flags)
{
    m_ptr = OS_NULL;
    m_allocated = m_used = m_pos = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Releases memory allocated for the buffer.

****************************************************************************************************
*/
eBuffer::~eBuffer()
{
    clear();
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
void eBuffer::setupclass()
{
    const os_int cls = ECLASSID_BUFFER;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eBuffer");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Clone object

  The eBuffer::clone function clones an eBuffer.

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eBuffer::clone(
    eObject *parent,
    e_oid id,
    os_int aflags)
{
    eBuffer *clonedobj;

    clonedobj = new eBuffer(parent, id == EOID_CHILD ? oid() : id, flags());

    clonedobj->allocate(m_allocated);
    if (m_ptr) os_memcpy(clonedobj->m_ptr, m_ptr, m_allocated);
    clonedobj->setused(used());

    clonegeneric(clonedobj, aflags);
    return clonedobj;
}


/**
****************************************************************************************************

  @brief Write buffer to stream.

  The eBuffer::writer() function serializes the eBuffer to stream. This writes only the
  content, use eObject::write() to save also class information, attachements, etc.

  @param  stream The stream to write to.
  @param  sflags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eBuffer::writer(
    eStream *stream,
    os_int sflags)
{
    os_memsz nwritten;

    /* Version number. Increment if new serialized items are added to the object,
       and check for new version's items in read() function.
     */
    const os_int version = 0;

    /* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Write number of used bytes.
     */
    if (stream->putl(m_used)) goto failed;

    /* Write used buffer content.
     */
    if (m_used > 0)
    {
        stream->write(m_ptr, m_used, &nwritten);
        if (nwritten != m_used) goto failed;
    }

    /* End the object.
     */
    if (stream->write_end_block()) goto failed;

    /* Object succesfully written.
     */
    return ESTATUS_SUCCESS;

    /* Writing object failed.
     */
failed:
    return ESTATUS_WRITING_OBJ_FAILED;
}


/**
****************************************************************************************************

  @brief Read the buffer from stream.

  The eBuffer::reader() function reads serialized eBuffer from stream. This function
  reads only the object content. To read whole object including attachments, names, etc,
  use eObject::read().

  @param  stream The stream to read from.
  @param  sflags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eBuffer::reader(
    eStream *stream,
    os_int sflags)
{
    /* Version number. Used to check which versions item's are in serialized data.
     */
    os_long tmp;
    os_memsz nbytes, nread;
    os_int version;

    /* If we have old data, delete it.
     */
    clear();

    /* Read object start mark and version number.
     */
    if (stream->read_begin_block(&version)) goto failed;

    /* Read number of bytes.
     */
    if (stream->getl(&tmp)) goto failed;
    nbytes = (os_memsz)tmp;

    /* Allocate buffer and set used size.
     */
    allocate(nbytes);
    setused(nbytes);

    /* Read buffer content.
     */
    if (nbytes > 0)
    {
        stream->read(m_ptr, nbytes, &nread);
        if (nread != nbytes) goto failed;
    }

    /* End the object.
     */
    if (stream->read_end_block()) goto failed;

    /* Object succesfully read.
     */
    return ESTATUS_SUCCESS;

    /* Reading object failed.
     */
failed:
    return ESTATUS_READING_OBJ_FAILED;
}


#if E_SUPPROT_JSON
/**
****************************************************************************************************

  @brief Write set to stream as JSON.

  The eBuffer::json_writer() function writes class specific object content to stream as JSON.

  @param  stream The stream to write to.
  @param  sflags Serialization flags. Typically EOBJ_SERIALIZE_DEFAULT.
  @param  indent Indentation depth, 0, 1... Writes 2x this spaces at beginning of a line.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eBuffer::json_writer(
    eStream *stream,
    os_int sflags,
    os_int indent)
{
    if (json_putl(stream, m_used)) goto failed;

    return ESTATUS_SUCCESS;

failed:
    return ESTATUS_FAILED;
}
#endif



/**
****************************************************************************************************

  @brief Open the buffer as stream.
  The open() function sets read position to zero.

  @param  parameters Ignored by eBuffer.
  @param  flags Ignored by eBuffer.
  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eBuffer::open(
	os_char *parameters,
    os_int flags) 
{
    m_pos = 0;
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Close the stream.
  The close() function does nothing for eBuffer.

  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eBuffer::close()
{
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to buffer.

  The write() function appends data into buffer.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nwritten Pointer to integer where to store number of bytes written to queue. This is
          always same as byte_sz. Can be set to OS_NULL, if not needed.

  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eBuffer::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    /* If new data does not fit into current buffer allocation, allocate more space.
     * Allocate about 1.5 times more space than we have.
     */
    if (m_used + buf_sz > m_allocated)
    {
        allocate(3*(m_used + buf_sz) / 2 + 8);
    }

    /* Copy the data into buffer and add to number of used bytes.
     */
    os_memcpy(m_ptr + m_used, buf, buf_sz);
    m_used += buf_sz;

    /* Return number of bytes written and success code.
     */
    if (nwritten != OS_NULL) *nwritten = buf_sz; 
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read data from buffer.

  The read function reads data from buffer.

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
eStatus eBuffer::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    os_memsz
        available;

    eStatus
        s = ESTATUS_STREAM_END;

    /* Check for maximum number of bytes to read. If we have data available,
       then copy it and set return value to success.
     */
    available = m_used - m_pos;
    if (buf_sz > available) buf_sz = available;
    if (available > 0)
    {
        os_memcpy(buf, m_ptr+m_pos, buf_sz);
        s = ESTATUS_SUCCESS;
    }

    /* Return number of bytes read and success code.
     */
    if (nread != OS_NULL) *nread = buf_sz;
    return s;
}


/**
****************************************************************************************************

  @brief Write character to buffer.

  The writechar function appends one character to buffer.

  @param  c Character to write.
  @return Always ESTATUS_SUCCESS.

****************************************************************************************************
*/
eStatus eBuffer::writechar(
    os_int c)
{
    /* If the character does not fit into current buffer allocation, allocate more space.
     * Allocate about 1.5 times more space than we have.
     */
    if (m_used >= m_allocated)
    {
        allocate(3*m_used / 2 + 8);
    }

    /* Save the character.
     */
    m_ptr[m_used] = (os_char)c;

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read character from buffer.

  The readchar function reads one character from the current read position of the buffer.

  @return If no more data available, the function returns E_STREM_END_OF_DATA.
          Otherwise if we have data the function returns next character to read.

****************************************************************************************************
*/
os_int eBuffer::readchar()
{
    /* If no data available.
     */
    if (m_pos == m_used) return E_STREM_END_OF_DATA;

    /* Return the characted.
     */
    return (os_uchar)m_ptr[m_pos++];
}


/**
****************************************************************************************************

  @brief Allocate/reallocate memory buffer.

  The allocate function allocates buffer to hold at least sz bytes. Old data in buffer, if any,
  is preserved. Allocated buffer is initialized with zeroes.

  @param  sz Minimum size for buffer to allocate.
  @param  bflags Ignored, set 0 for now.
  @return Pointer to buffer, as returned by ptr() function. OS_NULL if sz is negative or zero.

****************************************************************************************************
*/
os_char *eBuffer::allocate(
    os_memsz sz,
    os_int bflags)
{
    os_char
        *newbuf;

    os_memsz
        newallocation,
        copysz,
        clearsz;

    /* If we want to delete the buffer.
     */
    if (sz <= 0)
    {
        clear();
        return OS_NULL;
    }

    /* Allocate new buffer
     */
    newbuf = os_malloc(sz, &newallocation);

    /* Decide on number of bytes to copy and number to clear.
     */
    copysz = sz;
    if (copysz > m_allocated) copysz = m_allocated;
    clearsz = newallocation - copysz;

    /* Copy and clear.
     */
    if (copysz) os_memcpy(newbuf, m_ptr, copysz);
    if (clearsz) os_memclear(newbuf+copysz, clearsz);

    /* Free old buffer.
     */
    if (m_ptr)
    {
        os_free(m_ptr, m_allocated);
        m_ptr = OS_NULL;
    }

    /* Take new buffer to use.
     */
    m_ptr = newbuf;
    m_allocated = newallocation;
    if (m_used > copysz) m_used = copysz;
    if (m_pos > m_used) m_pos = m_used;

    /* Return pointer to buffer.
     */
    return m_ptr;
}


/**
****************************************************************************************************

  @brief Free allocated buffer.

  The clear function releases memory allocated for the buffer, clears read position and
  used size.

  @return None.

****************************************************************************************************
*/
void eBuffer::clear()
{
    if (m_ptr)
    {
        os_free(m_ptr, m_allocated);
        m_ptr = OS_NULL;
    }

    m_allocated = m_used = m_pos = 0;
}
