/**

  @file    econnection.h
  @brief   Connection base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Connection base class sets up general way to interace with different types of connections.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECONNECTION_INCLUDED
#define ECONNECTION_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eConnection : public eObject
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
	eConnection(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eConnection();

    /* Casting eObject pointer to eConnection pointer.
     */
	inline static eConnection *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_CONNECTION)
		return (eConnection*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_CONNECTION; 
    }

	/* Static constructor function.
	*/
	static eConnection *newobj(
		eObject *parent,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new eConnection(parent, oid, flags);
	}

	eStatus select(
		eConnection *connections,
        os_int nconnections,
		osalEvent evnt,
		osalSelectData *data,
		os_int flags)
    {
        return ESTATUS_SUCCESS;
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name Functions for writing to and reading from connection.

	  X...

	************************************************************************************************
	*/
	/*@{*/
    virtual eStatus open(
        os_char *path, 
        os_int flags=0) 
    {
        return ESTATUS_SUCCESS;
    }


    /*@}*/
};

#endif
