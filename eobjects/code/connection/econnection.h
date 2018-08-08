/**

  @file    econnection.h
  @brief   Connection base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  eConnection class, related to message envelope transport betweeen processes. See econnection.cpp
  for more information.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECONNECTION_INCLUDED
#define ECONNECTION_INCLUDED


/* Enumeration of connection's properties.
 */
#define ECONNP_CLASSID 2
#define ECONNP_IPADDR 4
#define ECONNP_ISOPEN 6

/* Connection property names.
 */
extern os_char
    econnp_classid[],
    econnp_ipaddr[],
    econnp_isopen[];


/**
****************************************************************************************************

  @brief Connection class.

  The eConnection passes message to other process trough TCP/IP socket or serial port.

****************************************************************************************************
*/
class eConnection : public eThread
{
public:
    /* Constructor.
     */
	eConnection(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
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
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT)
	{
        return new eConnection(parent, id, flags);
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

    /* Initialize the object.
     */
    virtual void initialize(
        eContainer *params = OS_NULL);

    /* Run the connection.
     */
    virtual void run();

    /*  virtual eStatus open(
        os_char *path, 
        os_int flags=0) 
    {
        return ESTATUS_SUCCESS;
    } */

    virtual void accepted(
        eStream *stream);


protected:
    /* Open the connection (connect)
     */
    void open();

    /* Stop and close the connection.
     */
    void close();

    /* Connection established event detected, act on it.
     */
    eStatus connected();

    /* Closing connection (connection failed, disconnected event, etc), act on it.
     */
    void disconnected();

    /* Monitor messages for bind and unbind.
     */
    void monitor_binds(
        eEnvelope *envelope);

    /* Write an envelope to the connection.
     */
    eStatus write(
        eEnvelope *envelope);

    /* Read an envelope from connection.
     */
    eStatus read();

    /* Not connected and connection has failed once, reply with notarget.
     */
    void notarget(
        eEnvelope *envelope);

    /** Stream class identifier. Specifies stream class to use.
     */
    os_int m_stream_classid;

    /* Timer for last send, used to generate keepalives.
     */
    os_timer m_last_send;

    /** IP address and optionally port number to connect to.
     */
    eVariable *m_ipaddr;

    /** stream handle. OS_NULL if socket is not open.
     */
    eStream *m_stream;

    /** Empty envelope beging read or reserved for next read.
     */
    eEnvelope *m_envelope;

    /** Memorized client bindings.
     */
    eContainer *m_client_bindings;

    /** Memorized server bindings.
     */
    eContainer *m_server_bindings;

    /** Buffer for messages when attempting connection for the first time.
     */
    eContainer *m_initbuffer;

    /** Connection initailized flag.
     */
    os_boolean m_initialized;

    /** Connected at this moment flag.
     */
    os_boolean m_connected;

    /** Connecttion has been attempted and it has failed at least once.
     */
    os_boolean m_connectetion_failed_once;

    /** Reconnect timer enabled. -1 = not set, 0 = slow timer, 1 = fast timer.
     */
    os_char m_fast_timer_enabled;

    /** New data has been written to stream, but the stream has not been
        flushed yet.
     */
    os_boolean m_new_writes;

    /** Delete the connection if case socket fails.
     */
    os_boolean m_delete_on_error;
};

#endif
