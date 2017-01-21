/**

  @file    eendpoint.h
  @brief   EndPoint base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  EndPoint base class sets up general way to interace with different types of endpoints.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EENDPOINT_INCLUDED
#define EENDPOINT_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eEndPoint : public eObject
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
	eEndPoint(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eEndPoint();

    /* Casting eObject pointer to eEndPoint pointer.
     */
	inline static eEndPoint *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_ENDPOINT)
		return (eEndPoint*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_ENDPOINT; 
    }

	/* Static constructor function.
	*/
	static eEndPoint *newobj(
		eObject *parent,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new eEndPoint(parent, oid, flags);
	}

	eStatus select(
		eEndPoint *endpoints,
        os_int nendpoints,
		osalEvent evnt,
		osalSelectData *data,
		os_int flags)
    {
        return ESTATUS_SUCCESS;
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name Functions for writing to and reading from endpoint.

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
