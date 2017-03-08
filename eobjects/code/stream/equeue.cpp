/**

  @file    equeue.cpp
  @brief   Queue buffer class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Queue buffers data, typically for reading from or writing to stream.

  When eQueue is used within eSocket class, the queu has a few functions: 1. To buffer incoming and
  outgoing data from/to OS socket. 2. To encode stream in such way, that control codes for 
  begin/end object and disconnect can be embedded within data stream. 3. To do run length
  encoding for data.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"

/* Default queu block size.
 */
#define EQUEUE_DEFALT_BLOCK_SZ 512

/** Value to indicate no previous character.
 */
#define EQUEUE_NO_PREVIOUS_CHAR 256


/**
****************************************************************************************************

  @brief Constructor.
  The constructor clears member variables for an empty eQueue object.

****************************************************************************************************
*/
eQueue::eQueue(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eStream(parent, id, flags)
{
    m_oldest = m_newest = OS_NULL;
    m_wr_prevc = EQUEUE_NO_PREVIOUS_CHAR;
    m_wr_count = 0;
    m_rd_prev2c = m_rd_prevc = m_rd_repeat_char = EQUEUE_NO_PREVIOUS_CHAR;
    m_rd_repeat_count = 0;
    m_flags = 0;
    m_bytes = 0;
    m_flushctrl_last_c = 0;
    m_flush_count = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.
  Releases all memory allocated for the queue.

****************************************************************************************************
*/
eQueue::~eQueue()
{
    /* Delete all blocks
     */
    while (m_oldest)
    {
        delblock();
    }
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
void eQueue::setupclass()
{
    const os_int cls = ECLASSID_QUEUE;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eQueue");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Open the queue.

  The open() function clears the queue into initial state and sets flags for encoding and
  decoding data on read and write.

  @param  parameters Ignored by eQueue.
  @param  flags for eQueue, bit fields:
          - OSAL_STREAM_ENCODE_ON_WRITE: Encode data when writing into queue. 
            If flag not set, data is written to queue as is.
          - OSAL_STREAM_DECODE_ON_READ: Decode data when reading from queue. 
            If not set, data is read from queue as is.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eQueue class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eQueue::open(
	os_char *parameters,
    os_int flags) 
{
    /* Call close() to make sure that queue is empty and all member variables initialized.
     */
    close();

    /* Save flags which indicate weather to 
     */
    m_flags = flags;

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Close the queue.

  The close() function releases all memory blocks but one and marks queue empty.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eQueue class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eQueue::close()
{
    /* Delete all blocks but one.
     */
    while (m_oldest != m_newest)
    {
        delblock();
    }

    /* Set head and tail poin at first byte of of only block. If no blocks, queue has never
       been used, no need to do anything.
     */
    if (m_newest)
    {
        m_newest->head = m_newest->tail = 0;
    }

    m_wr_prevc = EQUEUE_NO_PREVIOUS_CHAR;
    m_wr_count = 0;
    m_rd_prev2c = m_rd_prevc = m_rd_repeat_char = EQUEUE_NO_PREVIOUS_CHAR;
    m_rd_repeat_count = 0;
    m_flags = 0;
    m_bytes = 0;
    m_flushctrl_last_c = 0;
    m_flush_count = 0;

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Allocate new block and join it to queue.

  The newblock() function allocates a new block allocates new block and joins it to linked list
  of blocks as newest block.
  @return  None.

****************************************************************************************************
*/
void eQueue::newblock()
{
    eQueueBlock *b;
    os_memsz sz;

    /* Allocate block.
     */
    b = (eQueueBlock*)os_malloc(EQUEUE_DEFALT_BLOCK_SZ, &sz);

    /* Attach newly allocated block as newest block.
     */
    b->sz = (os_int)sz - sizeof(eQueueBlock);
    b->older = m_newest;
    b->newer = OS_NULL;
    b->head = 0;
    b->tail = 0;
    if (m_newest)
    {
        m_newest->newer = b;
    }
    else
    {
        m_oldest = b;
    }
    m_newest = b;
}


/**
****************************************************************************************************

  @brief Detach oldest block from queue and free memory allocatd for it.

  The delblock() function detaches oldest block from linked list of queue's memory blocks
  and releases memory allocated for the block.
  @return  None.

****************************************************************************************************
*/
void eQueue::delblock()
{
    eQueueBlock *b;

    /* Detach the oldest block from linked list.
     */
    b = m_oldest;
    if (b->newer)
    {
        b->newer->older = OS_NULL;
    }
    else
    {
        m_newest = OS_NULL;
    }
    m_oldest = b->newer;

    /* Free memory allocate for the block.
     */
    os_free(b, EQUEUE_DEFALT_BLOCK_SZ);
}


/**
****************************************************************************************************

  @brief Write data to queue.

  The write() function appends data to queue. The data can be encoded, if flag
  OSAL_STREAM_ENCODE_ON_WRITE was given to open().

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nwritten Pointer to integer where to store number of bytes written to queue. This is
          always same as byte_sz. Can be set to OS_NULL, if not needed.

  @return  If successfull, the function returns ESTATUS_SUCCESS. Other return values
           indicate an error. eQueue class cannot fail, so return value is always 
           ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eQueue::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
{
    /* Make sure that we have at least one block.
     */
    if (m_newest == OS_NULL) newblock();

    /* Write data either encoded or "as is".
     */
    if (m_flags & OSAL_STREAM_ENCODE_ON_WRITE)
    {
        write_encoded(buf, buf_sz);
    }
    else 
    {
        write_plain(buf, buf_sz);
    }

    if (nwritten != OS_NULL) *nwritten = buf_sz; 
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write encoded data to queue.

  The write_encoded() function encodes data while writing into queue. TThe encoding means packing
  control characters with data (here just appending E_STREAM_CTRLCH_IN_DATA after control
  character E_STREAM_CTRL_CHAR.
  This write function is used, if OSAL_STREAM_ENCODE_ON_WRITE flag was given to open() function,
  when the queue was opened.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @return None

****************************************************************************************************
*/
void eQueue::write_encoded(
    const os_char *buf, 
    os_memsz buf_sz)
{
    os_char c;
    os_int i;


    for (i = 0; i < buf_sz; i++)
    {
        /* Get current character
         */
        c = buf[i];

        /* If c is same as previous character, and we haven reached maximum number
           of characters to combine together, just increment the count
         */
        if (c == m_wr_prevc && m_wr_count < 31)
        {
            m_wr_count++;
        }

        /* Otherwise write previous character with or without repeats
         */
        else
        {
            if (m_wr_count) /* with repeat count */
            {
                complete_last_write();
            }
            else if (m_wr_prevc != EQUEUE_NO_PREVIOUS_CHAR) /* without repeat count */
            {
                putcharacter(m_wr_prevc);
                m_bytes++;
            }

            /* If character is control character?
             */
            if (c == E_STREAM_CTRL_CHAR)
            {
                /* If data countains control character, save as control character followed 
                   by ctrl in data mark.
                 */
                putcharacter(E_STREAM_CTRL_CHAR);
                putcharacter(E_STREAM_CTRLCH_IN_DATA);
                m_bytes += 2;

                /* no previous character
                 */
                m_wr_prevc = EQUEUE_NO_PREVIOUS_CHAR;
            }
            else
            {
                m_wr_prevc = (os_uchar)c;
            }
            m_wr_count = 0;
        }
    }
}


/**
****************************************************************************************************

  @brief Write plain data to queue.

  The write_plain() function stores data to queue as is, without encoding. This write function
  is used, if OSAL_STREAM_ENCODE_ON_WRITE flag was NOT given to open().

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @return None

****************************************************************************************************
*/
void eQueue::write_plain(
    const os_char *buf, 
    os_memsz buf_sz)
{
    os_int n;
    os_memsz count;
    os_uchar *u;

    m_bytes += buf_sz;

    /* If we need to calculate incoming flush controls.
     */
    if (buf_sz > 0 && (m_flags & OSAL_FLUSH_CTRL_COUNT))
    {
        u = (os_uchar*)buf;

        if (m_flushctrl_last_c == E_STREAM_CTRL_CHAR)
        {
            if (*u == E_STREAM_CTRLCH_FLUSH)
            {
                m_flush_count++;
            }
        }

        count = buf_sz - 1;
        while (count--)
        {
            if (*(u++) == E_STREAM_CTRL_CHAR)
            {
                if (*u == E_STREAM_CTRLCH_FLUSH)
                {
                    m_flush_count++;
                }
            }
        }

        m_flushctrl_last_c = (os_uchar)buf[buf_sz - 1];
    }

    while (buf_sz > 0)
    {
        /* If we need to allocate more block (newest block is full)?
         */
        if (m_newest->head + 1 == m_newest->tail ||
            (m_newest->head == m_newest->sz - 1 && m_newest->tail == 0))
        {
            newblock();
        }
        
        /* Write to end part of current block as much as we can.
         */
        if (m_newest->head >= m_newest->tail)
        {
            n = m_newest->sz - m_newest->head;
            if (m_newest->tail == 0) n--;
            if (n > buf_sz) n = (os_int)buf_sz;
            os_memcpy( ((os_char*)m_newest) + sizeof(eQueueBlock) + m_newest->head, buf, n);
            m_newest->head += n;
            if (m_newest->head == m_newest->sz) m_newest->head = 0;
            buf += n;
            buf_sz -= n;
            if (buf_sz == 0) break;
        }

        /* Write to beginning part of current block as much as we can.
         */
        if (m_newest->head < m_newest->tail)
        {
            n = m_newest->tail - m_newest->head - 1;

            if (n > buf_sz) n = (os_int)buf_sz;
            os_memcpy( ((os_char*)m_newest) + sizeof(eQueueBlock) + m_newest->head, buf, n);
            m_newest->head += n;
            buf += n;
            buf_sz -= n;
        }
    }
}


/**
****************************************************************************************************

  @brief Finish with writes so that all data is in queue buffers.

  When data is beeing written to queue, last character and it's repeat count are not necessarily
  moved immediately to queue buffer. This is done to allow run length encoding on the 
  fly. This function completes last write, so after this call all data is in buffer blocks. 

****************************************************************************************************
*/
void eQueue::complete_last_write()
{
    /* If writing without encoding, there is nothing to do.
     */
    if ((m_flags & OSAL_STREAM_ENCODE_ON_WRITE) == 0) return;

    if (m_wr_count == 0) /* without repeat count */
    {
        putcharacter(m_wr_prevc);
        m_bytes++;
    }
    else if (m_wr_count == 1) /* repeat twice */
    {
        putcharacter(m_wr_prevc);
        putcharacter((os_char)m_wr_prevc);
        m_bytes+=2;
    }
    else  /* with repeat count */
    {
        putcharacter(E_STREAM_CTRL_CHAR);
        putcharacter(m_wr_count);
        putcharacter(m_wr_prevc);
        m_bytes+=3;
    }

    /* no previous character
     */
    m_wr_prevc = EQUEUE_NO_PREVIOUS_CHAR;
    m_wr_count = 0;
}


/**
****************************************************************************************************

  @brief Read data from queue.

  The read function reads data from queue. This is used for actual data, not when control codes
  are expected.

  @param  buf Pointer to buffer into which to read data. If buffer is OS_NULL, up to buf_sz
          bytes are removed from queue but not stored anywhere. Null buffer can be used only
          when reading plain data (no OSAL_STREAM_DECODE_ON_READ flag)
  @param  buf_sz Size of buffer in bytes.
  @param  nread Pointer to integer where to store number of bytes read from queue. This may be
          less than buffer size if the function runs out of data in queue. Can be set to 
          OS_NULL, if not needed. 
  @param  Zero for default operation. Flag OSAL_STREAM_PEEK causes data to be read, but not removed
          from queue. This flag works only when reading plain data as is (no 
          OSAL_STREAM_DECODE_ON_READ flag given to open() or accept).

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values
          indicate an error. eQueue class cannot fail, so return value is always 
          ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eQueue::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    /* Make sure that all data including last character are in buffer.
     */
    if (m_wr_prevc != EQUEUE_NO_PREVIOUS_CHAR) 
    {
        complete_last_write();
    }

    /* If no buffer.
     */
    if (m_oldest == OS_NULL) 
    {
        if (nread) *nread = 0;
        return ESTATUS_SUCCESS;
    }

    /* If reading without decoding, just read the character.
     */
    if (m_flags & OSAL_STREAM_DECODE_ON_READ)
    {
        read_decoded(buf, buf_sz, nread);
    }
    else
    {
        read_plain(buf, buf_sz, nread, flags);
    }

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Read and decode data from queue.

  The read_decoded() function decodes data while reading from queue. TThe decoding means
  restoring data to plain format, without control character markings nor run length encoding.
  This read function is used, if OSAL_STREAM_DECODE_ON_READ flag was given to open() or
  accept() function.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nread Pointer to integer where to store number of bytes actually read.
          OS_NULL if not needed.
  @return None

****************************************************************************************************
*/
void eQueue::read_decoded(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread)
{
    os_uchar c, cc;
    os_int n;

    n = 0;
    while (OS_TRUE)
    {
        /* If we are repeating character
         */
        if (m_rd_repeat_count)
        {
            buf[n++] = (os_char)m_rd_repeat_char;
            m_rd_repeat_count--;
            if (n >= buf_sz) break;
            continue;
        }

        /* If we run out of data .
         */
        if (!hasedata()) break;

        /* Get character.
         */
        c = getcharacter();
        m_bytes--;

        /* If previous character is control we are processing repeat count
         */
        if (m_rd_prev2c == E_STREAM_CTRL_CHAR)
            if ((m_rd_prevc & E_STREAM_CTRLCH_MASK) == 0)
        {
            m_rd_repeat_char = c;
            m_rd_repeat_count = m_rd_prevc;
            m_rd_prevc = m_rd_prev2c = EQUEUE_NO_PREVIOUS_CHAR;
            buf[n++] = c;
            if (n >= buf_sz) break;
            continue;
        }

        if (m_rd_prevc == E_STREAM_CTRL_CHAR) 
        {
            /* If this is single control char
             */
            cc = (c & E_STREAM_CTRLCH_MASK);
            if (cc)
            {
                m_rd_prevc = m_rd_prev2c = EQUEUE_NO_PREVIOUS_CHAR;
                if (cc == E_STREAM_CTRLCH_IN_DATA)
                {
                    m_rd_repeat_char = E_STREAM_CTRL_CHAR;
                    m_rd_repeat_count = (c & E_STREAM_COUNT_MASK);
                    buf[n++] = c;
                    if (n >= buf_sz) break;
                }
                continue;
            }

            /* Otherwise this is beginnig of repeat count marking.
             */
            m_rd_prev2c = m_rd_prevc;
            m_rd_prevc = c;
            continue;
        }

        if (c == E_STREAM_CTRL_CHAR)
        {
            m_rd_prev2c = m_rd_prevc;
            m_rd_prevc = c;
            continue;
        }

        /* Plain character, nothing to di with control characters
         */
        buf[n++] = c;
        if (n >= buf_sz) break;
    }

    if (nread != OS_NULL) *nread = n; 
}


/**
****************************************************************************************************

  @brief Read data from queue "as is"

  The read_plain() function reads data from queue without modifying the data. This read
  function is used, if OSAL_STREAM_DECODE_ON_READ flag was NOT given to open() or accept()
  function.

  @param  buf Pointer to data to write.
  @param  buf_sz Number of bytes to write.
  @param  nread Pointer to integer where to store number of bytes actually read.
          OS_NULL if not needed.
  @param  flags Zero for default operation. OSAL_STREAM_PEEK to read data without removing it
          from the queue.
  @return None

****************************************************************************************************
*/
void eQueue::read_plain(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread,
    os_int flags)
{
    os_int n, head, tail;
    os_memsz buf_sz0;

    eQueueBlock *oldest, *newest, *oldest2;

    buf_sz0 = buf_sz;
    newest = m_newest;
    oldest = m_oldest;

    while (buf_sz > 0 && oldest)
    {
        oldest2 = oldest->newer;
        head = oldest->head;
        tail = oldest->tail;

        /* Write to end part of current block as much as we can.
         */
        if (tail > head)
        {
            n = newest->sz - tail;
            if (n > buf_sz) n = (os_int)buf_sz;
            if (buf) 
            {
                os_memcpy(buf, ((os_char*)oldest) + sizeof(eQueueBlock) + tail, n);
                buf += n;
                buf_sz -= n;
            }
            tail += n;
            if (tail == newest->sz) tail = 0;
        }

        /* Write to beginning part of current block as much as we can.
         */
        if (head > tail && buf_sz > 0)
        {
            n = head - tail;

            if (n > buf_sz) n = (os_int)buf_sz;
            if (buf) 
            {
                os_memcpy(buf, ((os_char*)oldest) + sizeof(eQueueBlock) + tail, n);
                buf += n;
            }
            tail += n;
            buf_sz -= n;
        }

        /* If we are not only peeking (reading without removing data from buffer)
         */
        if ((flags & OSAL_STREAM_PEEK) == 0)
        {
            if (head == tail) 
            {
                delblock();
            }
            else
            {
                m_oldest->tail = tail;
            }
        }

        oldest = oldest2;
    }

    if (nread != OS_NULL) *nread = buf_sz0 - buf_sz; 
    if ((flags & OSAL_STREAM_PEEK) == 0) m_bytes -= buf_sz0 - buf_sz;
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
          indicate an error. eQueue class cannot fail, so return value is always 
          ESTATUS_SUCCESS. 

****************************************************************************************************
*/
eStatus eQueue::writechar(
    os_int c)
{
    /* Make sure that we have at least one block.
     */
    if (m_newest == OS_NULL) newblock();

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
    if (m_wr_prevc != EQUEUE_NO_PREVIOUS_CHAR) 
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
            c = E_STREAM_CTRLCH_DISCONNECT;
            break;

        case E_STREAM_FLUSH:
            c = E_STREAM_CTRLCH_FLUSH;
            break;

        case E_STREAM_KEEPALIVE:
            c = E_STREAM_CTRLCH_KEEPALIVE;
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

  @brief Read character or control code from queue.

  The readchar function reads either one byte or one control code from queue.

  @return Byte of data or control code. Possible control codes are E_STREAM_BEGIN, 
          E_STREAM_END, OSAL_STREAM_DISCONNECT or E_STREM_END_OF_DATA. 
          Version number if returned with E_STREAM_BEGIN, use E_STREAM_CTRL_MASK to
          get only control code.

****************************************************************************************************
*/
os_int eQueue::readchar()
{
    os_uchar c, cc;

    /* Make sure that all data including last character are in buffer.
     */
    if (m_wr_prevc != EQUEUE_NO_PREVIOUS_CHAR) 
    {
        complete_last_write();
    }

    /* If no buffer.
     */
    if (m_oldest == OS_NULL) return E_STREM_END_OF_DATA;
    
    /* If reading without decoding, just read the character.
     */
    if ((m_flags & OSAL_STREAM_DECODE_ON_READ) == 0) 
    {
        /* If we run out of data.
         */
        if (!hasedata()) return E_STREM_END_OF_DATA;
        m_bytes--;
        return getcharacter();
    }

    while (OS_TRUE)
    {
        /* If we are repeating character
         */
        if (m_rd_repeat_count)
        {
            m_rd_repeat_count--;
            return m_rd_repeat_char;
        }

        /* If we run out of data.
         */
        if (!hasedata()) return E_STREM_END_OF_DATA;

        /* Get character.
         */
        c = getcharacter();
        m_bytes--;

        /* If previous character is control we are processing repeat count
         */
        if (m_rd_prev2c == E_STREAM_CTRL_CHAR) if ((m_rd_prevc & E_STREAM_CTRLCH_MASK) == 0)
        {
            m_rd_repeat_char = c;
            m_rd_repeat_count = m_rd_prevc;
            m_rd_prevc = m_rd_prev2c = EQUEUE_NO_PREVIOUS_CHAR;
            return c;
        }

        if (m_rd_prevc == E_STREAM_CTRL_CHAR) 
        {
            /* If this is single control char
             */
            cc = (c & E_STREAM_CTRLCH_MASK);
            if (cc)
            {
                m_rd_prevc = m_rd_prev2c = EQUEUE_NO_PREVIOUS_CHAR;
                switch (cc)
                {
                    /* Control character in data.
                     */
                    case E_STREAM_CTRLCH_IN_DATA:
                        m_rd_repeat_char = E_STREAM_CTRL_CHAR;
                        m_rd_repeat_count = (c & E_STREAM_COUNT_MASK);
                        return E_STREAM_CTRL_CHAR;

                    /* Flush count character.
                     */
                    case E_STREAM_CTRLCH_FLUSH:
                        m_flush_count--;
                        return E_STREAM_CTRL_BASE + c;

                    /* Completely ignore keepalive characters.
                     */
                    case E_STREAM_CTRLCH_KEEPALIVE:
                        break;
        
                    /* Beginning/end of object or stream has been disconnected.
                     */
                    default:
                        return E_STREAM_CTRL_BASE + c;
                }
                continue;
            }

            /* Otherwise this is beginnig of repeat count marking.
             */
            m_rd_prev2c = m_rd_prevc;
            m_rd_prevc = c;
            continue;
        }

        if (c == E_STREAM_CTRL_CHAR)
        {
            m_rd_prev2c = m_rd_prevc;
            m_rd_prevc = c;
            continue;
        }

        /* Plain character, nothing to do with control characters.
         */
        return c;
    }

    return E_STREM_END_OF_DATA;
}


/**
****************************************************************************************************

  @brief Get number of bytes in queue including what is half written.

  Get how much data there is in the queue.
  @return Number of bytes in queue + what is beging generated.

****************************************************************************************************
*/
os_memsz eQueue::bytes()
{
    os_int missing;
    if (m_wr_prevc == EQUEUE_NO_PREVIOUS_CHAR) 
    {
        missing = 0;
    }
    else if (m_wr_count == 0) /* without repeat count */
    {
        missing = 1;
    }
    else if (m_wr_count == 1) /* repeat twice */
    {
        missing = 2;
    }
    else  /* with repeat count */
    {
        missing = 3;
    }
    return m_bytes + missing;
}
