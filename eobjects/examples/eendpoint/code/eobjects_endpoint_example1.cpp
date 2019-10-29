/**

  @file    eobjects_endpoint_example1.cpp
  @brief   Example code about connecting two processes.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  This demonstrates creating an end point which listens for socket connections.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects/extensions/socket/esocket.h"
#include "eobjects_endpoint_example.h"
#include <stdio.h>

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID_1 (ECLASSID_APP_BASE + 1)

/* Enumeration of eMyClass1 properties. Normally these would be in header file.
 */
#define EMYCLASS1P_A 10
#define EMYCLASS1P_B 20

static os_char emyclass1p_a[] = "A";
static os_char emyclass1p_b[] = "B";


/**
****************************************************************************************************

  @brief Example class.

  X...

****************************************************************************************************
*/
class eMyClass1 : public eThread
{
public:
    /* Constructor.
     */
    eMyClass1(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
        : eThread(parent, id, flags)
    {
        initproperties();
    }

    /* Add eMyClass'es properties to class'es property set.
    */
    static void setupclass()
    {
        const os_int cls = MY_CLASS_ID_1;

        os_lock();
        eclasslist_add(cls, (eNewObjFunc)newobj, "eMyClass1");
        addproperty(cls, EMYCLASS1P_A, emyclass1p_a, EPRO_PERSISTENT, "A");
        addproperty(cls, EMYCLASS1P_B, emyclass1p_b, EPRO_PERSISTENT, "B");
        os_unlock();
    }

    /* Static constructor function for generating instance by class list.
     */
    static eMyClass1 *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eMyClass1(parent, id, flags);
    }

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_1;
    }

    /* Process incoming messages. 
     */
    virtual void onmessage(
        eEnvelope *envelope)
    {
        /* If this is message to me, not to my children?
         */
        if (*envelope->target() == '\0')
        {
            /* If timer message, then increment A?
             */
            if (envelope->command() == ECMD_TIMER)
            {
                setpropertyl(EMYCLASS1P_A, propertyl(EMYCLASS1P_A) + 1);
                return;
            }
        }

        /* Default message procesing.
         */
        eThread::onmessage(envelope);
    }

    /* This gets called when property value changes
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags)
    {
        switch (propertynr)
        {
            case EMYCLASS1P_A:
                printf ("1: GOT A \'%s\'\n", x->gets());
                break;

            case EMYCLASS1P_B:
                printf ("1: GOT B \'%s\'\n", x->gets());
                break;
        }
    }
};


/**
****************************************************************************************************

  @brief End point example 1.

  The endpoint_example_1() function sets up new class eMyClass...

  @return  None.

****************************************************************************************************
*/
void endpoint_example_1()
{
    eThread *t;
    eThreadHandle thandle1, thandle2, endpointthreadhandle;
    eContainer c;

    /* Set up eSocket and my own classes for use.
     */
    eSocket::setupclass(); 
    eMyClass1::setupclass(); 

    /* Create and start class eMyClass1 as thread named "myclass1".
     */
    t = new eMyClass1();
	t->addname("myclass1", ENAME_PROCESS_NS);
t->setpropertys(EMYCLASS1P_A, "Nasse");
    t->timer(4500);
    t->timer(20);
// t->json_write(&econsole);
    t->start(&thandle1); /* After this t pointer is useless */

    /* Create and start end point thread to listen for incoming socket connections,
       name it "myendpoint".
     */
    t = new eEndPoint();
	t->addname("//myendpoint");
// t->json_write(&econsole);
    t->start(&endpointthreadhandle); /* After this t pointer is useless */
    c.setpropertys_msg(endpointthreadhandle.uniquename(),
         ":" IOC_DEFAULT_SOCKET_PORT_STR, eendpp_ipaddr);

    os_sleep(15000000);

    /* Wait for the threads to terminate.
     */
    thandle1.terminate();
    thandle1.join();
    endpointthreadhandle.terminate();
    endpointthreadhandle.join();
}
