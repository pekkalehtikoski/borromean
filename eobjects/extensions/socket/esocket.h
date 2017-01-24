/**

  @file    esocket.h
  @brief   TCP socket class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  TCP socket class encodes and buffers data and calls OSAL's stream functions to read/write the
  socket.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ESOCKET_INCLUDED
#define ESOCKET_INCLUDED

class eQueue;

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eSocket : public eStream
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
	eSocket(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eSocket();

    /* Casting eObject pointer to eSocket pointer.
     */
	inline static eSocket *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_SOCKET)
		return (eSocket*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_SOCKET; 
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
	static eSocket *newobj(
		eObject *parent,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new eSocket(parent, oid, flags);
	}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Socket functions for writing to socket.

	  X...

	************************************************************************************************
	*/
	/*@{*/

    /* Open a socket.
     */
    virtual eStatus open(
	    os_char *parameters,
        os_int flags = 0);

    virtual eStatus close();

    virtual eStatus flush() {return ESTATUS_SUCCESS;}

    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL)
        {if (nwritten != OS_NULL) *nwritten = 0; return ESTATUS_SUCCESS;}

    virtual eStatus read(
        os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nread = OS_NULL,
        os_int flags = 0)
        {if (nread != OS_NULL) *nread = 0; return ESTATUS_SUCCESS;}

    /*@}*/

protected:
        /* Input queue (buffer)
         */
        eQueue *m_in;

        /* Output queue (buffer)
         */
        eQueue *m_out;

        /* Osal socket handle.
         */
        osalStream m_socket;
};

#endif
