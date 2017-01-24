/**

  @file    eobjects_property_example1.cpp
  @brief   Example code object properties.
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


/**
****************************************************************************************************

  @brief Set example 1.

  Sets are used to implement properties...
  @return  None.

****************************************************************************************************
*/
void property_example_1()
{
	eSet collection;
    eVariable v;

    collection.sets(10, "Abba12345678901234567890123456789012345678901234567890123456789012345678901234567890!");
    collection.setd(20, 20.72);
    v = 10.72;
    collection.set(5, &v);
    collection.setl(5000, 12);

    collection.get(5, &v);
    osal_console_write("\n5: ");
    osal_console_write(v.gets());
    collection.get(7, &v);
    osal_console_write("\n7: ");
    osal_console_write(v.gets());
    collection.get(10, &v);
    osal_console_write("\n10: ");
    osal_console_write(v.gets());
    collection.get(20, &v);
    osal_console_write("\n20: ");
    osal_console_write(v.gets());
    collection.get(5000, &v);
    osal_console_write("\n5000: ");
    osal_console_write(v.gets());

    osal_console_write(collection.get(10, &v) ?"\n10 HAS VALUE" : "\n10 HAS NO VALUE");
    osal_console_write(collection.get(11, &v) ?"\n11 HAS VALUE" : "\n11 HAS NO VALUE");
    osal_console_write("\n");
}
