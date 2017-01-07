/**

  @file    eobjects_property_example.cpp
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

#define MY_COMMAND 10

/**
****************************************************************************************************

  @brief Example property class.

  X...

****************************************************************************************************
*/
class eMyClass : public eObject
{
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

    v.setpropertys(EVARP_TEXT, "ulle");

    v.property(EVARP_TEXT, &u);
    osal_console_write(u.gets());
    osal_console_write("\n");

    osal_console_write("\n");
/*
    v = "Abba12345678901234567890123456789012345678901234567890123456789012345678901234567890!";
    collection.set(10, &v);

    v = 20.72;
    collection.set(20, &v);

    collection.set(5, &v);

    if (collection.get(10, &v)) osal_console_write("HAS VALUE");
    osal_console_write(v.gets());
    osal_console_write("\n");

    collection.get(20, &v);
    osal_console_write(v.gets());
    osal_console_write("\n");

    collection.get(5, &v);
    osal_console_write(v.gets());
    osal_console_write("\n");

//    MyClass
//		myobj;
*/

}
