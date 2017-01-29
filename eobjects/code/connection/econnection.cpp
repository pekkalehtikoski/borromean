/**

  @file    econnection.cpp
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
#include "eobjects/eobjects.h"

/* Connection property names.
 */
os_char
    econnp_classid[] = "classid",
    econnp_ipaddr[] = "ipaddr",
    econnp_isopen[] = "isopen";


/**
****************************************************************************************************

  @brief Constructor.

  Clear member variables and allocate eVariable for IP address and container for first
  initialization bufffer.

  @return  None.

****************************************************************************************************
*/
eConnection::eConnection(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eThread(parent, oid, flags)
{
    m_stream_classid = ECLASSID_SOCKET;
    m_ipaddr = new eVariable(this);
    m_stream = OS_NULL;
    m_initbuffer = new eContainer(this);;
    m_initialized = OS_FALSE;
    m_connected = OS_FALSE;
    m_connectetion_failed_once = OS_FALSE;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eConnection::~eConnection()
{
    close();
}


/**
****************************************************************************************************

  @brief Add eVariable to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds eVariable to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization reader functions. The property stet stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eConnection::setupclass()
{
    const os_int cls = ECLASSID_CONNECTION;
    eVariable *p;

    /* Synchwonize, add the class to class list and properties to property set.
     */
    osal_mutex_system_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj);
    addproperty(cls, ECONNP_CLASSID, econnp_classid, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "class ID");
    addproperty(cls, ECONNP_IPADDR, econnp_ipaddr, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "IP");
    p = addpropertyl(cls, ECONNP_ISOPEN, econnp_isopen, 
        EPRO_NOONPRCH, "is open", OS_FALSE);
    p->setpropertys(EVARP_ATTR, "rdonly;chkbox");
    osal_mutex_system_unlock();
}


/**
****************************************************************************************************

  @brief Called to inform the class about property value change (override).

  The onpropertychange() function is called when class'es property changes, unless the
  property is flagged with EPRO_NOONPRCH. 
  If property is flagged as EPRO_SIMPLE, this function shuold save the property value
  in class members and and return it when simpleproperty() is called.

  Notice for change logging: Previous value is still valid when this function is called.
  You can get the old value by calling property() function inside onpropertychange()
  function.

  @param   propertynr Property number of changed property.
  @param   x Variable containing the new value.
  @param   flags
  @return  None.

****************************************************************************************************
*/
void eConnection::onpropertychange(
    os_int propertynr, 
    eVariable *x, 
    os_int flags)
{
    switch (propertynr)
    {
        case ECONNP_CLASSID:
            m_stream_classid = (os_int)x->getl();
            close();
            break;

        case ECONNP_IPADDR:
            if (x->compare(m_ipaddr))
            {
                m_ipaddr->setv(x);
                close();
            }
            break;

        default:
            eThread::onpropertychange(propertynr, x, flags); 
            break;
    }
}


/**
****************************************************************************************************

  @brief Get value of simple property (override).

  The simpleproperty() function stores current value of simple property into variable x.

  @param   propertynr Property number to get.
  @param   x Variable info which to store the property value.
  @return  If fproperty with property number was stored in x, the function returns 
           ESTATUS_SUCCESS (0). Nonzero return values indicate that property with
           given number was not among simple properties.

****************************************************************************************************
*/
eStatus eConnection::simpleproperty(
    os_int propertynr, 
    eVariable *x)
{
    switch (propertynr)
    {
        case EENDPP_CLASSID:
            x->setl(m_stream_classid);
            break;

        case ECONNP_IPADDR:
            x->setv(m_ipaddr);
            break;
   
        default:
            return eThread::simpleproperty(propertynr, x);
    }
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Function to process incoming messages. 

  The onmessage function handles messages received by object. 
  
  @param   envelope Message envelope. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eConnection::onmessage(
    eEnvelope *envelope)
{
    os_char c;

    /* If this is envelope to be routed trough connection
     */
    c = *envelope->target();
    if (c != '_' && c != '\0') 
    {
        /* Check for binding related messages, memorize bindings through this connection.
         */

        /* If currently connected, write envelope immediately.
         */
        if (m_connected)
        {
            if (write(envelope)) close();
        }

        /* Not connected.
         */
        else
        {
            /* If connection has not failed yet, buffer messages.
             */
            if (!m_connectetion_failed_once)
            {
                if (envelope->flags() & EMSG_CAN_BE_ADOPTED)
                {
                    m_initbuffer->adopt(envelope);
                }
                else
                {
                    envelope->clone(m_initbuffer);
                }
            }

            /* Otherwise connection has failed already, reply with 
               notarget.
             */
            else
            {
                notarget(envelope);
            }
        }

        return;
    }
 
    eThread::onmessage(envelope);
}


/**
****************************************************************************************************

  @brief Initialize the object.

  The initialize() function is called when new object is fully constructed.
  It marks end point object initialized, and opens listening end point if ip address
  for it is already set.

  @param   params Parameters for the new thread.
  @return  None.

****************************************************************************************************
*/
void eConnection::initialize(
    eContainer *params)
{
    osal_console_write("initializing worker\n");

    m_initialized = OS_TRUE;
}


/**
****************************************************************************************************

  @brief Run the connection.

  The eConnection::run() function...

  @return  None.

****************************************************************************************************
*/
void eConnection::run()
{
    eStatus s;
    osalSelectData selectdata;
    os_int64 start_t = 0;
    os_long try_again_ms = osal_rand(3000, 4000);

    osal_timer_get(&start_t);

    while (!exitnow())
    {
        /* If we have listening socket, wait for socket or thread event. 
           Call alive() to process thread events.
         */
        if (m_stream)
        {
            s = m_stream->select(&m_stream, 1, trigger(), &selectdata, OSAL_STREAM_DEFAULT);

            if (s) 
            {
	            osal_console_write("osal_stream_select failed\n");
            }

            if (selectdata.eventflags & OSAL_STREAM_CUSTOM_EVENT)
            {
                /* Process messages.
                 */
                alive(EALIVE_RETURN_IMMEDIATELY);

                /* If message queue for incoming messages is empty, flush writes.
                 */
                if (m_message_queue->first() == OS_NULL)
                {
                    m_stream->flush();
                }
            }

            if (selectdata.eventflags & OSAL_STREAM_CLOSE_EVENT)
            {
                osal_console_write("close event\n");
                close();
                continue;
            }

            if (selectdata.eventflags & OSAL_STREAM_CONNECT_EVENT)
            {

                if (selectdata.errorcode)
                {
                    osal_console_write("connect failed\n");
                    close();
                    continue;
                }
                else
                {
                    osal_console_write("connect event\n");
                    if (connected()) 
                    {
                        close();
                        continue;
                    }
                }
            }

            if (selectdata.eventflags & OSAL_STREAM_READ_EVENT)
            {
                osal_console_write("read event\n");
                
                /* Read object */
                read();
            }

            /* if (selectdata.eventflags & OSAL_STREAM_WRITE_EVENT)
            {
                osal_console_write("write event\n");
            } */
        }

        /* Otherwise wait for thread events and process them.
         */
        else
        {
            /* WE SHOULD USE EALIVE_WAIT_FOR_EVENT, but EALIVE_RETURN_IMMEDIATELY is used
               until timers are implemented in eobjects.
             */
            /* alive(EALIVE_WAIT_FOR_EVENT); */

            alive(EALIVE_RETURN_IMMEDIATELY);
            os_sleep(100);

            /* If we need to open connection. THIS SHOULD BE DONE BY TIMER EVENT, NOT BY POLLING
             */
            if (start_t == 0 || osal_timer_elapsed(&start_t, try_again_ms))
            {
                open();
                osal_timer_get(&start_t);
            }
        }

        osal_console_write("worker running\n");
    }
}


/**
****************************************************************************************************

  @brief Incoming connection has been accepted.

  The eConnection::accepted() function adopts connected incoming stream and starts communicating
  through it.

  @return  None.

****************************************************************************************************
*/
void eConnection::accepted(
    eStream *stream) 
{
    if (m_stream) delete m_stream;

    m_stream = stream;
    adopt(stream);
}


/**
****************************************************************************************************

  @brief Open the connechtion.

  The eConnection::open() connects underlying stream.

  @return  None.

****************************************************************************************************
*/
void eConnection::open()
{
    eStatus s;

    if (m_stream || !m_initialized || m_ipaddr->isempty()) return;

    /* New by class ID.
     */
    m_stream = (eStream*)newchild(m_stream_classid);

    s = m_stream->open(m_ipaddr->gets(), OSAL_STREAM_CONNECT);
    if (s)
    {
	    osal_console_write("osal_stream_open failed\n");
        delete m_stream;
        m_stream = OS_NULL;
    }
}


/**
****************************************************************************************************

  @brief Close the connection.

  The eConnection::close() function calls disconnected() to inform bindings and set
  connection state, then closes underlying stream and clears all member veriables
  for current connection state.

  @return  None.

****************************************************************************************************
*/
void eConnection::close()
{
    if (m_stream == OS_NULL) return;

    disconnected();

    setpropertyl(EENDPP_ISOPEN, OS_FALSE);

    if (m_stream)
    {
        m_stream->close();
        delete m_stream;
        m_stream = OS_NULL;
    }
}


/**
****************************************************************************************************

  @brief Connection established event detected, act on it.

  The eConnection::connected() function:
  - Writes all queued data forward to connection.
  - Inform client bindings that theu can be reestablished.
  - Marks connection connected.

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values indicate
          an error and stream is to be closed.

****************************************************************************************************
*/
eStatus eConnection::connected()
{
    eEnvelope *envelope;

    while ((envelope = eEnvelope::cast(m_initbuffer->first())))
    {
        if (write(envelope)) return ESTATUS_FAILED;
        delete envelope;
    }

    /* Inform client bindings that the binding can be reestablished.
     */

    m_connected = OS_TRUE;
    setpropertyl(ECONNP_ISOPEN, OS_TRUE);
   return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Closing connection (connection failed, disconnected event, etc), act on it.

  The eConnection::connected() function:
  - Send notarget to all items in initialization queue?
  - Inform all bindings that that there is no connection.
  - Marks connection disconnected and that connection has failed once.

  @return  None.

****************************************************************************************************
*/
void eConnection::disconnected()
{
    eEnvelope *envelope;

    while ((envelope = eEnvelope::cast(m_initbuffer->first())))
    {
        notarget(envelope);
        delete envelope;
    }

    /* Inform all bindings that the connection is lost.
     */


    m_connected = OS_FALSE;
    setpropertyl(ECONNP_ISOPEN, OS_FALSE);
    m_connectetion_failed_once = OS_TRUE;
    m_initbuffer->clear();
}


/**
****************************************************************************************************

  @brief Write data to connection.

  The eConnection::write() function...

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values indicate
          an error and stream is to be closed.

****************************************************************************************************
*/
eStatus eConnection::write(
    eEnvelope *envelope)
{
    eStatus s;

    if (m_stream == OS_NULL) return ESTATUS_FAILED;

    s = envelope->writer(m_stream, EOBJ_SERIALIZE_DEFAULT);
    return s;
}


/**
****************************************************************************************************

  @brief Read data from connection.

  The eConnection::read() function...

  @return If successfull, the function returns ESTATUS_SUCCESS. Other return values indicate
          an error and stream is to be closed.

****************************************************************************************************
*/
eStatus eConnection::read()
{
    eStatus s;
    eEnvelope *envelope;

    if (m_stream == OS_NULL) return ESTATUS_FAILED;;

    envelope = new eEnvelope(this);
    s = envelope->reader(m_stream, EOBJ_SERIALIZE_DEFAULT);
    if (s) 
    {
        delete(envelope);
        return s;
    }
    
    envelope->prependtarget("/");
    message(envelope);
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Not connected and connection has failed once, reply with notarget.

  The eConnection::notarget() function sends notarget message

  @return  None.

****************************************************************************************************
*/
void eConnection::notarget(
    eEnvelope *envelope)
{
    if ((envelope->flags() & EMSG_NO_REPLIES) == 0)
    {
        message(ECMD_NO_TARGET, envelope->source(), OS_NULL,
            OS_NULL, EMSG_NO_REPLIES, envelope->context());
    }
}


