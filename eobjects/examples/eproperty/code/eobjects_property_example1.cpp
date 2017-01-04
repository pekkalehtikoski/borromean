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
    virtual eStatus onpropertychange(
        os_int propertynr, 
        eVariable *variable, 
        os_int flags)
    {
        return ESTATUS_SUCCESS;
    }
};


/**
****************************************************************************************************

  @brief Thread example 1.

  The property_example_1() function...

  @return  None.

****************************************************************************************************
*/
void property_example_1()
{
	eMyClass
		myobj;

}
