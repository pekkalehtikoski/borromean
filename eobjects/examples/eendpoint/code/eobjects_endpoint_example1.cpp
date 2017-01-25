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
#include "eobjects_endpoint_example.h"
#include <stdio.h>

/* Purpose of a message is specified by 32 bit command. Negative command identifiers are
   reserved for the eobject library related, but positive ones can be used freely.
 */
#define MY_COMMAND 10

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID_1 (ECLASSID_APP_BASE + 1)
#define MY_CLASS_ID_2 (ECLASSID_APP_BASE + 2)

/* Enumeration of eMyClass1 properties. Normally these would be in header file.
 */
#define EMYCLASS1P_A 10
#define EMYCLASS1P_B 20

static os_char emyclass1p_a[] = "A";
static os_char emyclass1p_b[] = "B";

/* Enumeration of eMyClass2 properties. 
 */
#define EMYCLASS2P_X 10
#define EMYCLASS2P_Y 20

static os_char emyclass2p_x[] = "X";
static os_char emyclass2p_y[] = "Y";


/**
****************************************************************************************************

  @brief Example property class.

  X...

****************************************************************************************************
*/
class eMyClass1 : public eThread
{
public:
	/** Constructor. It is here just to initialize properties to default values.s
     */
    eMyClass1(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	    : eThread(parent, oid, flags)
    {
        initproperties();
    }

    /* Add eMyClass'es properties to class'es property set.
    */
    static void setupclass()
    {
        const os_int cls = MY_CLASS_ID_1;

        osal_mutex_system_lock();
        addproperty(cls, EMYCLASS1P_A, emyclass1p_a, EPRO_PERSISTENT, "A");
        addproperty(cls, EMYCLASS1P_B, emyclass1p_b, EPRO_PERSISTENT, "B");
        osal_mutex_system_unlock();
    }

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_1;
    }

    /* This gets called when property value changes
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags)
    {
        os_double a, b;

        switch (propertynr)
        {
            case EMYCLASS1P_A:
                a = x->getd();
                printf ("1: GOT A %f\n", a);
                break;

            case EMYCLASS1P_B:
                b = x->getd();
                printf ("1: GOT B %f\n", b);
                setpropertyd(EMYCLASS1P_A, b * 1.01);
                break;
        }
    }
};



/**
****************************************************************************************************

  @brief Example property class.

  X...

****************************************************************************************************
*/
class eMyClass2 : public eThread
{
public:
	/** Constructor. It is here just to initialize properties to default values.s
     */
    eMyClass2(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	    : eThread(parent, oid, flags)
    {
        initproperties();
    }

    /* Add eMyClass'es properties to class'es property set.
    */
    static void setupclass()
    {
        const os_int cls = MY_CLASS_ID_2;

        osal_mutex_system_lock();
        addproperty(cls, EMYCLASS2P_X, emyclass2p_x, EPRO_PERSISTENT, "X");
        addproperty(cls, EMYCLASS2P_Y, emyclass2p_y, EPRO_PERSISTENT, "Y");
        osal_mutex_system_unlock();
    }

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_2;
    }

    virtual void initialize(
        eContainer *params = OS_NULL)
    {
        bind(EMYCLASS2P_X, "//thread1/_p/A", EBIND_DEFAULT);
setpropertyd(EMYCLASS2P_Y, 3.3);
        bind(EMYCLASS2P_Y, "//thread1/_p/B", EBIND_CLIENTINIT);
setpropertyd(EMYCLASS2P_Y, 4.3);
    }

    /* This gets called when property value changes
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags)
    {
        os_double a, b;

        switch (propertynr)
        {
            case EMYCLASS2P_X:
                a = x->getd();
                printf ("2: GOT X %f\n", a);
                setpropertyd(EMYCLASS2P_Y, a * 0.99);
                break;

            case EMYCLASS2P_Y:
                b = x->getd();
                printf ("1: GOT Y %f\n", b);
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
    eEndPoint *ep;
    eContainer c;

    /* Set up eSocket and my own classes for use.
     */
    eSocket::setupclass(); 
#if 0
    eMyClass1::setupclass(); 
    eMyClass2::setupclass(); 

    /* Create and start thread named "thread1".
     */
    t = new eMyClass1();
	t->addname("thread1", ENAME_PROCESS_NS);
    t->start(&thandle1); /* After this t pointer is useless */

    /* Create and start thread named "thread2".
     */
    t = new eMyClass2();
	t->addname("thread2", ENAME_PROCESS_NS);
    t->start(&thandle2); /* After this t pointer is useless */

    c.setpropertyd_msg("//thread1/_p/A", 11.5);
#endif
    /* Create and start thread to listen for incoming socket connections, 
       name it "endpointthread".
     */
    t = new eThread();
	t->addname("//endpointthread");
    ep = new eEndPoint(t);
	ep->addname("//myendpoint");
    t->start(&endpointthreadhandle); /* After this t pointer is useless */
    c.setpropertys_msg("//myendpoint",
         ":" OSAL_DEFAULT_SOCKET_PORT_STR, eendpp_ipaddr);

    os_sleep(15000);

    /* Wait for the threads to terminate.
     */
/*    thandle1.terminate();
    thandle1.join();
    thandle2.terminate();
    thandle2.join(); */
    endpointthreadhandle.terminate();
    endpointthreadhandle.join();
}
