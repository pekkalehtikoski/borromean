/**

  @file    equeue.cpp
  @brief   Queue buffer class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Queue buffers data, typically for reading from or writing to stream.

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

  X...

  @return  None.

****************************************************************************************************
*/
eQueue::eQueue(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eStream(parent, oid, flags)
{
    /* Clear member variables.
     */
    m_oldest = m_newest = OS_NULL;
    m_head = m_tail = 0;

    m_prevc = EQUEUE_NO_PREVIOUS_CHAR;
    m_count = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  Releases all memory allocated for the queue.
  @return  None.

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
    b = (eQueueBlock*)osal_memory_allocate(EQUEUE_DEFALT_BLOCK_SZ, &sz);

    /* Attach newly allocated block as newest block.
     */
    b->sz = (os_int)sz - sizeof(eQueueBlock);
    b->older = m_newest;
    b->newer = OS_NULL;
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
    osal_memory_free(b, EQUEUE_DEFALT_BLOCK_SZ);
}


/**
****************************************************************************************************

  @brief Close the queue.

  The close() function releases all memoty blocks but one and marks queue empty.
  @return  None.

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
        m_head = m_tail = 0;
    }
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write data to queue.

  The write () function releases places data into queue. The data can be coded.
  @return  None.

****************************************************************************************************
*/
eStatus eQueue::write(
    const os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nwritten)
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
        if (c == m_prevc && m_count < 31)
        {
            m_count++;
        }

        /* Otherwise write previous character with or without repeats
         */
        else
        {
            if (m_count) /* with repeat count */
            {
                complete_last_write();
            }
            else if (m_prevc != EQUEUE_NO_PREVIOUS_CHAR) /* without repeat count */
            {
                putcharacter((os_char)m_prevc);
            }

            /* If character is control character?
             */
            if (c == E_STREAM_CTRL_CHAR)
            {
                /* If data countains control character, save as control character followed 
                   by ctrl in data mark.
                 */
                putcharacter(E_STREAM_CTRL_CHAR);
                putcharacter(EL_STREAM_CTRLCH_IN_DATA);

                /* no previous character
                 */
                m_prevc = EQUEUE_NO_PREVIOUS_CHAR;
                m_count = 0;
            }
            else
            {
                m_prevc = c;
                m_count = 0;
            }
        }
    }

    if (nwritten != OS_NULL) *nwritten = buf_sz; 
    return ESTATUS_SUCCESS;
}


void eQueue::complete_last_write()
{
    if (m_count == 0) /* without repeat count */
    {
        putcharacter((os_char)m_prevc);
    }
    else if (m_count == 1) /* repeat twice */
    {
        putcharacter((os_char)m_prevc);
        putcharacter((os_char)m_prevc);
    }
    else  /* with repeat count */
    {
        putcharacter(E_STREAM_CTRL_CHAR);
        putcharacter((os_char)m_count);
        putcharacter((os_char)m_prevc);
    }

    /* no previous character
     */
    m_prevc = EQUEUE_NO_PREVIOUS_CHAR;
    m_count = 0;
}

eStatus eQueue::read(
    os_char *buf, 
    os_memsz buf_sz, 
    os_memsz *nread)
{
    if (m_prevc != EQUEUE_NO_PREVIOUS_CHAR) 
    {
        complete_last_write();
    }


    if (nread != OS_NULL) *nread = 0; 
    return ESTATUS_SUCCESS;
}
