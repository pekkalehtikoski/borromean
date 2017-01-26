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


/* Enumeration of connection properties.
 */
#define ECONNP_IPADDR 2

/* Connection property names.
 */
extern os_char
    econnp_ipaddr[];


/**
****************************************************************************************************

  @brief Connection class.

  The eConnection passes message to other process trough TCP/IP socket.

****************************************************************************************************
*/
class eConnection : public eThread
{
	/** 
	************************************************************************************************

	  @name Generic object functionality.

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

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
	static eConnection *newobj(
		eObject *parent,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new eConnection(parent, oid, flags);
	}

    /* Called when property value changes.
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags);

    /* Get value of simple property.
     */
    virtual eStatus simpleproperty(
        os_int propertynr, 
        eVariable *x);

    /* Function to process messages to this object. 
     */
    virtual void onmessage(
        eEnvelope *envelope);

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

protected:
    eVariable *m_ipaddr;
};

#endif
