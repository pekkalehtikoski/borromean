/**

  @file    ebuffer.h
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
#ifndef EBUFFER_INCLUDED
#define EBUFFER_INCLUDED


/**
****************************************************************************************************

  @brief Buffer Class.

  The eBuffer class is flat byte buffer, which can also be used as a stream.

****************************************************************************************************
*/
class eBuffer : public eStream
{
public:
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/

    /* Constructor.
     */
    eBuffer(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
    virtual ~eBuffer();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent,
        e_oid id = EOID_CHILD,
        os_int aflags = 0);

    /* Casting eObject pointer to eBuffer pointer.
     */
    inline static eBuffer *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_BUFFER)
        return (eBuffer*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_BUFFER;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
    static eBuffer *newobj(
		eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
        return new eBuffer(parent, id, flags);
	}

    /* Write set content to stream.
     */
    virtual eStatus writer(
        eStream *stream,
        os_int sflags);

    /* Read set content from stream.
     */
    virtual eStatus reader(
        eStream *stream,
        os_int sflags);

#if E_SUPPROT_JSON
    /* Write set to stream as JSON.
     */
    eStatus json_writer(
        eStream *stream,
        os_int sflags,
        os_int indent);
#endif

    /*@}*/


    /**
    ************************************************************************************************

      @name Queue functions.
      Open, close, read, write, select soecke, etc. These implement eStream functionality.

    ************************************************************************************************
    */
    /*@{*/

    /* Open the queue.
     */
    virtual eStatus open(
	    os_char *parameters,
        os_int flags = 0);

    /* Close the queue.
     */
    virtual eStatus close();

    /* Write data to queue.
     */
    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL);

    /* Read data from queue.
     */
    virtual eStatus read(
        os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nread = OS_NULL,
        os_int flags = 0);

    /* Write character to queue, typically control code.
     */
    virtual eStatus writechar(
        os_int c);

    /* Read character or control code.
     */    
    virtual os_int readchar();


    /*@}*/

    /**
    ************************************************************************************************

      @name Buffer functions.
      Functionality as plain buffer.

    ************************************************************************************************
    */
    /*@{*/

    /* Allocate/reallocate memory buffer.
     */
    os_char *allocate(
        os_memsz sz,
        os_int bflags = 0);

    /* Get pointer to buffer
     */
    inline os_char *ptr()
    {
        return m_ptr;
    }

    /* Get allocated size, may be larger than sz given to allocate().
     */
    inline os_memsz allocated()
    {
        return m_allocated;
    }

    /* Get used size, either set bu setused() or by writing to stream.
     */
    inline os_memsz used()
    {
        return m_used;
    }

    /* Set used size.
     */
    inline void setused(
        os_memsz sz)
    {
        m_used = sz;
    }

    /* Free allocated buffer.
     */
    void clear();

    /*@}*/

private:
    /**
    ************************************************************************************************

      @name Internal for the class.
      Member variables and protected functions.

    ************************************************************************************************
    */

    /** Pointer to allocated buffer, OS_NULL if none.
     */
    os_char *m_ptr;

    /** Size of allocated buffer in bytes.
     */
    os_memsz m_allocated;

    /** Number of used bytes in buffer.
     */
    os_memsz m_used;

    /** Current stream read position within buffer.
     */
    os_memsz m_pos;
};

#endif
