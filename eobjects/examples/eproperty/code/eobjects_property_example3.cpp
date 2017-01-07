/**

  @file    eobjects_property_example3.cpp
  @brief   Example code about naming objects.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  This example demonstrates how to create a property and sen messages to it.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects_property_example.h"

/* Purpose of a message is specified by 32 bit command. Negative command identifiers are
   reserved for the eobject library related, but positive ones can be used freely.
 */
#define MY_COMMAND 10

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID (ECLASSID_APP_BASE + 1)

/**
****************************************************************************************************

  @brief Example property class.

  X...

****************************************************************************************************
*/
class eMyClass : public eObject
{
    /* Get class identifier.
     */
    virtual os_int classid() {return MY_CLASS_ID;}

    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *variable, 
        os_int flags)
    {
     }
};


/**
****************************************************************************************************

  @brief Property example 2.

  The property_example_2() function...

  @return  None.

****************************************************************************************************
*/
void property_example_2()
{
    eVariable v, u;

    v.setpropertys(EVARP_VALUE, "ulle");
    osal_console_write("\nv.gets() = ");
    osal_console_write(v.gets());

    v.setpropertyd(EVARP_VALUE, 10.22);
    osal_console_write("\nv.gets() = ");
    osal_console_write(v.gets());

    v.setpropertyd(EVARP_DIGS, 5);
    osal_console_write("\nv.gets() = ");
    osal_console_write(v.gets());

    v.setpropertys(EVARP_TEXT, "nasse");
    v.property(EVARP_TEXT, &u);
    osal_console_write("\nv.property(EVARP_TEXT, &u), u.gets() = ");
    osal_console_write(u.gets());

    osal_console_write("\n");
/*

//    MyClass
//		myobj;
*/

}
