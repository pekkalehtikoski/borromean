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
        os_int flags=0) {return ESTATUS_SUCCESS;}

    virtual eStatus close();

    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL);

    virtual eStatus read(
        os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nread = OS_NULL);

	/** Begin an object, etc. block. This is for versioning, block size may be changed.
     */
    virtual eStatus write_begin_block(
        os_int version) {return ESTATUS_SUCCESS;}

	/** End an object, etc. block. This skips data added by later versions of object.
     */
    virtual eStatus write_end_block() {return ESTATUS_SUCCESS;}

    virtual eStatus read_begin_block(
        os_int& version) {return ESTATUS_SUCCESS;}

	/** End an object, etc. block. This skips data added by later versions of object.
     */
    virtual eStatus read_end_block() {return ESTATUS_SUCCESS;}

    /*@}*/

private:
    /* Allocate new block and join it to queue.
     */
    void newblock();

    /* Detach oldest block from queue and free memory allocatd for it.
     */
    void delblock();

    /* Put character to queue
     */
    inline void putcharacter(
        os_int c)
    {
        if (m_head >= m_newest->sz)
        {
            newblock();
            m_head = 0;
        }

        *(((os_char*)m_newest) + sizeof(eQueueBlock) + m_head) = (os_char)c;
    }

    /* Finish with last write so also previous character has been
       processed.
     */
    void complete_last_write();

    /** Oldest block in the queue.
     */
    eQueueBlock *m_oldest;

    /** Latest block added to queue.
     */
    eQueueBlock *m_newest;

    /** Queue head index (inside newest)
     */
    os_int m_head;

    /** Queue tail index (inside oldest)
     */
    os_int m_tail;

    /* Previous character
     */
    os_int m_prevc;

    /* Number of times same has repeated afterwards (0 = character has occurred once).
     */
    os_int m_count;
};

#endif
