/**

  @file    eobjects_property_example3.cpp
  @brief   Example code object properties.
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

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID (ECLASSID_APP_BASE + 1)

/* Enumeration of eMyClass properties. Normally these would be in header file.
 */
#define EMYCLASSP_CELCIUS 10
#define EMYCLASSP_FAHRENHEIT 12
#define EMYCLASSP_OPINION 14

static os_char emyclassp_celcius[] = "C";
static os_char emyclassp_fahrenheit[] = "F";
static os_char emyclassp_opinion[] = "opinion";


/**
****************************************************************************************************

  @brief Example property class.

  X...

****************************************************************************************************
*/
class eMyClass : public eObject
{
public:
    /* Constructor.
     */
    eMyClass(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
        : eObject(parent, id, flags)
    {
        initproperties();
    }

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID;
    }

    /* Add eMyClass'es properties to class'es property set.
    */
    static void setupclass()
    {
        const os_int cls = MY_CLASS_ID;
        eVariable *p;

        os_lock();
        p = addpropertyd(cls, EMYCLASSP_CELCIUS, emyclassp_celcius, EPRO_PERSISTENT, "value", 20.0);
        p->setpropertys(EVARP_UNIT, "C");
        
        p = addpropertyd(cls, EMYCLASSP_FAHRENHEIT, emyclassp_fahrenheit, EPRO_NOONPRCH, "default");
        p->setpropertys(EVARP_UNIT, "F");
        p->setpropertyl(EVARP_DIGS, 5);

        addpropertys(cls, EMYCLASSP_OPINION, emyclassp_opinion, EPRO_NOONPRCH, "default");
        os_unlock();
    }

    /* This gets called when property value changes
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags)
    {
        os_double c, f;

        switch (propertynr)
        {
            case EMYCLASSP_CELCIUS:
                c = x->getd();
                printf ("calculating C -> F\n");

                f = c * 9.0 / 5.0 + 32.0;
                setpropertyd(EMYCLASSP_FAHRENHEIT, f);
                if (f < 70) setpropertys(EMYCLASSP_OPINION, "cold");
                else if (f < 80) setpropertys(EMYCLASSP_OPINION, "ok");
                else setpropertys(EMYCLASSP_OPINION, "hot");
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
void property_example_3()
{
    eMyClass *converter;
    eVariable v, u;
    os_double f;

    /* Adds the eMyClass to class list and creates property set for the class. 
     */
    eMyClass::setupclass(); 

    converter = new eMyClass();
   
    f = converter->propertyd(EMYCLASSP_FAHRENHEIT);
    converter->propertyv(EMYCLASSP_OPINION, &v);
    printf ("initial F = %f, opinion = %s\n", f, v.gets());
   
    converter->setpropertyd(EMYCLASSP_CELCIUS, 40.0);
    f = converter->propertyd(EMYCLASSP_FAHRENHEIT);
    converter->propertyv(EMYCLASSP_OPINION, &v);
    printf ("initial F = %f, opinion = %s\n", f, v.gets());

    delete converter;
}
