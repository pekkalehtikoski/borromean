/**

  @file    ewhere.cpp
  @brief   Where clause interprenter.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Where clause interpenter.

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
eWhere::eWhere(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{

    /* Create container for variables and enable name space for it.
     */
    m_vars = new eContainer(this);
    m_vars->ns_create();
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eWhere::~eWhere()
{
}

