/**

  @file    equeue.h
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
#ifndef EQUEUE_INCLUDED
#define EQUEUE_INCLUDED

/** Memory is buffered as blocks within queue. Block header structure is:
 */
typedef struct eQueueBlock
{
    /** Pointer to the next older queued block.
     */
    struct eQueueBlock *older;

    /** Pointer to the next newer queued block.
     */
    struct eQueueBlock *newer;

    /** Queue head index (inside newest)
     */
    os_int head;

    /** Queue tail index (inside oldest)
     */
    os_int tail;

    /** Size of queued block excluting the header.
     */
    os_int sz;
} 
eQueueBlock;



/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eQueue : public eStream
{
	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
	/** Constructor.
     */
	eQueue(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eQueue();

    /* Casting eObject pointer to eQueue pointer.
     */
	inline static eQueue *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_QUEUE)
		return (eQueue*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_QUEUE; 
    }

	/* Static constructor function.
	*/
	static eQueue *newobj(
		eObject *parent,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new eQueue(parent, oid, flags);
	}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Queue functions for writing to queue.

	  X...

	************************************************************************************************
	*/
	/*@{*/
    virtual eStatus open(
        os_char *path, 
        os_int flags = 0);

    virtual eStatus close();

    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL);

    virtual eStatus read(
        os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nread = OS_NULL);

	/** Write character, typically control code.
     */
    virtual eStatus writechar(
        os_int c);

    /* Read character or control code.
     */    
    virtual os_int readchar();

    /*@}*/

private:
    /* Allocate new block and join it to queue.
     */
    void newblock();

    /* Detach oldest block from queue and free memory allocatd for it.
     */
    void delblock();

    /* Put character to queue.
     */
    inline void putcharacter(
        os_int c)
    {
        os_int nexthead;

        /* Get next head.
         */
        nexthead = m_newest->head + 1;
        if (nexthead >= m_newest->sz) nexthead = 0;
    
        /* If this block is full, move on to next block
         */
        if (nexthead == m_newest->tail)
        {
            newblock();
            nexthead = 1;
        }

        *(((os_char*)m_newest) + sizeof(eQueueBlock) + m_newest->head) = (os_char)c;
        m_newest->head = nexthead;
    }

    /* Finish with last write so also previous character has been
       processed.
     */
    void complete_last_write();

    /* Check if queue has data.
     */
    inline os_char hasedata()
    {
        if (m_newest != m_oldest) return OS_TRUE;
        return m_oldest->head != m_oldest->tail;
    }

    /* Get character from queue.
     */
    inline os_char getcharacter()
    {
        os_char c;
        os_int nexttail;

        c = *(((os_char*)m_oldest) + sizeof(eQueueBlock) + m_oldest->tail);

        /* Get next head.
         */
        nexttail = m_oldest->tail + 1;
        if (nexttail >= m_oldest->sz) nexttail = 0;
        m_oldest->tail = nexttail;

        /* If this block is now empty, and it is not only block, delete it.
         */
        if (nexttail == m_oldest->head)
        {
            if (m_oldest != m_newest) delblock();
        }

        return c;
    }

    /** Oldest block in the queue.
     */
    eQueueBlock *m_oldest;

    /** Latest block added to queue.
     */
    eQueueBlock *m_newest;

    /** Flags for the queue. Flags OSAL_STREAM_ENCODE_ON_WRITE and
        OSAL_STREAM_DECODE_ON_READ are observed.
     */
    os_int m_flags;

    /** Previous character
     */
    os_int m_wr_prevc;

    /** Number of times same has repeated afterwards (0 = character has occurred once).
     */
    os_int m_wr_count;

    /** Repeat count when unpacking repeated character (RLE).
     */
    os_int m_rd_repeat_count;

    /** Repeated character.
     */
    os_int m_rd_repeat_char;

    /** Previous character, while reading.
     */
    os_int m_rd_prevc;

    /** The character before that one.
     */
    os_int m_rd_prev2c;
};

#endif
