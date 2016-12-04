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


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eStream::eStream(
	eObject *parent,
	e_oid oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eStream::~eStream()
{
}


/* Store long integer value to stream.
	*/
eStatus eStream::setl(
    const os_long x)
{
    return ESTATUS_SUCCESS;
}

/* Store float value to stream.
    */
eStatus eStream::setf(
	const os_double x)
{
    return ESTATUS_SUCCESS;
}


/* Store double value to stream.
    */
eStatus eStream::setd(
	const os_double x)
{
    return ESTATUS_SUCCESS;
}

/* Set string value to variable.
    */
eStatus eStream::sets(
	const os_char *x)
{
    return ESTATUS_SUCCESS;
}

/* Set variable value as string.
	*/
eStatus eStream::sets(
	eVariable *x)
{
    return ESTATUS_SUCCESS;
}

/* Get long integer value from stream.
	*/
eStatus eStream::getl(
    os_long *x)
{
    return ESTATUS_SUCCESS;
}

/* Get float value from stream.
	*/
eStatus eStream::getf(
    os_float *x)
{
    return ESTATUS_SUCCESS;
}

/* Get double value from stream.
	*/
eStatus eStream::getd(
    os_double *x)
{
    return ESTATUS_SUCCESS;
}

/* Get string value from stream into variable.
	*/
eStatus eStream::gets(
    eVariable *x)
{
    return ESTATUS_SUCCESS;
}
