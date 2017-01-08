/**

  @file    eobjects_property_example3.cpp
  @brief   Example code about naming objects.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  This example demonstrates setting up a new class with properties, and how to react to property
  value changes.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects_property_example.h"
#include <stdio.h>

/* Purpose of a message is specified by 32 bit command. Negative command identifiers are
   reserved for the eobject library related, but positive ones can be used freely.
 */
#define MY_COMMAND 10

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID_1 (ECLASSID_APP_BASE + 1)
#define MY_CLASS_ID_2 (ECLASSID_APP_BASE + 1)

/* Enumeration of eMyClass1 properties. Normally these would be in header file.
 */
#define EMYCLASS1P_A 10
#define EMYCLASS1P_B 20

static os_char emyclass1p_a[] = "A";
static os_char emyclass1p_b[] = "B";

/* Enumeration of eMyClass2 properties. Normally these would be in header file.
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

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_1;
    }

    /* Add eMyClass'es properties to class'es property set.
    */
    static void setupclass()
    {
        const os_int cls = MY_CLASS_ID_1;

        osal_mutex_system_lock();
        addproperty(cls, EMYCLASS1P_A, emyclass1p_a, EPRO_PERSISTENT, "A");
        addproperty(cls, EMYCLASS1P_B, emyclass1p_b, EPRO_PERSISTENT, "A");
        osal_mutex_system_unlock();
    }

    /* This gets called when property value changes
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags)
    {
        os_double a;

        switch (propertynr)
        {
            case EMYCLASS1P_A:
                a = x->getd();
                printf ("GOT A + %f\n", a);

                /* f = c * 9.0 / 5.0 + 32.0;
                setpropertyd(EMYCLASSP_FAHRENHEIT, f);
                if (f < 70) setpropertys(EMYCLASSP_OPINION, "cold");
                else if (f < 80) setpropertys(EMYCLASSP_OPINION, "ok");
                else setpropertys(EMYCLASSP_OPINION, "hot");
            */
                break;
        }
    }
};


/**
****************************************************************************************************

  @brief Property example 3.

  The property_example_3() function sets up new class eMyClass and uses for Celcius
  to Fahrenheit conversion. Purpose of this is to show how class can react to property changes.

  @return  None.

****************************************************************************************************
*/
void property_example_4()
{
    eThread *t;
    eThreadHandle thandle1;
    eContainer c;

    /* Adds the eMyClass to class list and creates property set for the class.
     */
    eMyClass1::setupclass(); 

    /* Create and start thread 1 named "thread1".
     */
    t = new eMyClass1();
	t->addname("thread1", ENAME_PROCESS_NS);
//    t->setpriority();
    t->start(&thandle1); /* After this t pointer is useless */

    c.setpropertyd_msg("//thread1/_p/A", 11.5);

    osal_thread_sleep(10000);

    // f = converter->se>propertyd(EMYCLASSP_FAHRENHEIT);

    /* Wait for thread to terminate
     */
    thandle1.terminate();
    thandle1.join();
}
