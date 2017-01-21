/**

  @file    esocket.h
  @brief   Socket base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Socket base class sets up general way to interace with different types of sockets.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ESOCKET_INCLUDED
#define ESOCKET_INCLUDED

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
    virtual eStatus open(
        os_char *path, 
        os_int flags=0) {return ESTATUS_SUCCESS;}

    virtual eStatus close() {return ESTATUS_SUCCESS;}

    virtual eStatus flush() {return ESTATUS_SUCCESS;}

    virtual eStatus write(const os_char *buf, os_memsz buf_sz, os_memsz *nwritten = OS_NULL)
        {if (nwritten != OS_NULL) *nwritten = 0; return ESTATUS_SUCCESS;}

    virtual eStatus read(os_char *buf, os_memsz buf_sz, os_memsz *nread = OS_NULL)
        {if (nread != OS_NULL) *nread = 0; return ESTATUS_SUCCESS;}

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
};

#endif
