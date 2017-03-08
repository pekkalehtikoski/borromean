/**

  @file    eendpoint.h
  @brief   End point class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  The eEndPoint is socket end point listening to specific TCP port for new connections.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EENDPOINT_INCLUDED
#define EENDPOINT_INCLUDED

/* Enumeration of end point properties.
 */
#define EENDPP_CLASSID 2
#define EENDPP_IPADDR  4
#define EENDPP_ISOPEN  6

/* End point property names.
 */
extern os_char
    eendpp_classid[],
    eendpp_ipaddr[],
    eendpp_isopen[];


/**
****************************************************************************************************

  @brief End point class.

  The eEndPoint is socket end point listening to specific TCP port for new connections.

****************************************************************************************************
*/
class eEndPoint : public eThread
{
public:
    /* Constructor.
     */
	eEndPoint(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
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

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
	static eEndPoint *newobj(
		eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
        return new eEndPoint(parent, id, flags);
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

    /* Initialize the object.
     */
    virtual void initialize(
        eContainer *params = OS_NULL);

    /* Run the connection.
     */
    virtual void run();

protected:
    void open();
    void close();

    /** Stream class identifier. Specifies stream class to use.
     */
    os_int m_stream_classid;

    /** IP address of the interface and port number to listen to. 
        IP address can be empty to listen for any interface.
     */
    eVariable *m_ipaddr;

    /** Listening stream handle. OS_NULL if listening socket 
        is not open.
     */
    eStream *m_stream;

    /** End point object initailized flag.
     */
    os_boolean m_initialized;
};

#endif
