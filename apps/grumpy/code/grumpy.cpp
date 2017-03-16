/**

  @file    econtainer.cpp
  @brief   Simple object container.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The container object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "grumpy.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
Grumpy::Grumpy(
    eObject *parent,
    e_oid id,
    os_int flags)
    : eThread(parent, id, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
Grumpy::~Grumpy()
{
}


/**
****************************************************************************************************

  @brief Run the application.

  The Grumpy::run() function...

  @return  None.

****************************************************************************************************
*/
void Grumpy::run()
{
    while (!exitnow())
    {
        alive();
    }
}
