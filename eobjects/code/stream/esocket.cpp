/**

  @file    esocket.cpp
  @brief   Socket base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Socket base class sets up general way to interace with different types of sockets.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eSocket::eSocket(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eStream(parent, oid, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eSocket::~eSocket()
{
}

