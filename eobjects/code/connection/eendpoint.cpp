/**

  @file    eendpoint.cpp
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
#include "eobjects/eobjects.h"

/* End point property names.
 */
os_char
    eendpp_classid[] = "classid",
    eendpp_ipaddr[] = "ipaddr",
    eendpp_isopen[] = "isopen";


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEndPoint::eEndPoint(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eThread(parent, id, flags)
{
    /** Clear member variables and allocate eVariable for IP address.
     */
    m_stream = OS_NULL;
    m_initialized = OS_FALSE;
    m_stream_classid = ECLASSID_SOCKET;
    m_ipaddr = new eVariable(this);
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eEndPoint::~eEndPoint()
{
    close();
}


/**
****************************************************************************************************

  @brief Add eEndPoint to class list and class'es properties to it's property set.

  The eEndPoint::setupclass function adds eEndPoint to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier, 
  which is used for serialization functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eEndPoint::setupclass()
{
    const os_int cls = ECLASSID_ENDPOINT;
    eVariable *p;

    /* Synchronize, add the class to class list and properties to property set.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eEndPoint");
    addproperty(cls, EENDPP_CLASSID, eendpp_classid, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "class ID");
    addproperty(cls, EENDPP_IPADDR, eendpp_ipaddr, 
        EPRO_PERSISTENT|EPRO_SIMPLE, "IP");
    p = addpropertyl(cls, EENDPP_ISOPEN, eendpp_isopen, 
        EPRO_NOONPRCH, "is open", OS_FALSE);
    p->setpropertys(EVARP_ATTR, "rdonly;chkbox");
    os_unlock();
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
void eEndPoint::onpropertychange(
    os_int propertynr, 
    eVariable *x, 
    os_int flags)
{
    switch (propertynr)
    {
        case EENDPP_CLASSID:
            m_stream_classid = (os_int)x->getl();
            close();
            open();
            break;

        case EENDPP_IPADDR:
            if (x->compare(m_ipaddr))
            {
                m_ipaddr->setv(x);
                close();
                open();
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
  @param   x Variable into which to store the property value.
  @return  If property with property number was stored in x, the function returns
           ESTATUS_SUCCESS (0). Nonzero return values indicate that property with
           given number was not among simple properties.

****************************************************************************************************
*/
eStatus eEndPoint::simpleproperty(
    os_int propertynr, 
    eVariable *x)
{
    switch (propertynr)
    {
        case EENDPP_CLASSID:
            x->setl(m_stream_classid);
            break;

        case EENDPP_IPADDR:
            x->setv(m_ipaddr);
            break;
   
        default:
            return eThread::simpleproperty(propertynr, x);
    }
    return ESTATUS_SUCCESS;
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
void eEndPoint::initialize(
    eContainer *params)
{
    osal_console_write("initializing worker\n");

    m_initialized = OS_TRUE;
    open();
}


/**
****************************************************************************************************

  @brief Run the connection.

  The eEndPoint::run() function...

  @return  None.

****************************************************************************************************
*/
void eEndPoint::run()
{
    eStatus s;
    osalSelectData selectdata;
    eStream *newstream;
    eConnection *c;

    while (!exitnow())
    {
        /* If we have listening socket, wait for socket or thread event. 
           Call alive() to process thread events.
         */
        if (m_stream)
        {
            m_stream->select(&m_stream, 1, trigger(), &selectdata, OSAL_STREAM_DEFAULT);

            alive(EALIVE_RETURN_IMMEDIATELY);

            if (selectdata.errorcode)
            {
	            osal_console_write("osal_stream_select failed\n");
            }

            else if (selectdata.eventflags & OSAL_STREAM_ACCEPT_EVENT)
            {
                osal_console_write("accept event\n");

                /* New by class ID.
                 */
                newstream = (eStream*)newchild(m_stream_classid);
            
            	s = m_stream->accept(newstream, OSAL_STREAM_DEFAULT);

                if (s == ESTATUS_SUCCESS)
                {
                    c = new eConnection();
	                c->addname("//connection");
                    c->accepted(newstream);
                    c->start(); /* After this c pointer is useless */
                }
                else
                {
                    delete newstream;
	                osal_console_write("osal_stream_accept failed\n");
                }
            }
        }

        /* Otherwise wait for thread events and process them.
         */
        else
        {
            alive(EALIVE_WAIT_FOR_EVENT);
        }

        osal_console_write("worker running\n");
    }
}


/**
****************************************************************************************************

  @brief Open the end point.

  The eEndPoint::open() starts listening end point.

  @return  None.

****************************************************************************************************
*/
void eEndPoint::open()
{
    eStatus s;

    if (m_stream || !m_initialized || m_ipaddr->isempty()) return;

    /* New by class ID.
     */
    m_stream = (eStream*)newchild(m_stream_classid);

    s = m_stream->open(m_ipaddr->gets(), OSAL_STREAM_LISTEN|OSAL_STREAM_SELECT);
    if (s)
    {
	    osal_console_write("osal_stream_open failed\n");
        delete m_stream;
        m_stream = OS_NULL;
    }
    else
    {
        setpropertyl(EENDPP_ISOPEN, OS_TRUE);
    }
}


/**
****************************************************************************************************

  @brief Close the end point.

  The eEndPoint::close() function closes listening end point.

  @return  None.

****************************************************************************************************
*/
void eEndPoint::close()
{
    if (m_stream == OS_NULL) return;

    setpropertyl(EENDPP_ISOPEN, OS_FALSE);

//    m_stream->close();
    delete m_stream;
    m_stream = OS_NULL;
}
