/**

  @file    egrumpy.h
  @brief   Grumpy Borg robot application.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef GRUMPY_INCLUDED
#define GRUMPY_INCLUDED

/**
****************************************************************************************************

  @brief Application class.

  The application class starts the other application threads and runs until exit is requested.

****************************************************************************************************
*/
class Grumpy : public eThread
{
public:
    /* Constructor.
     */
    Grumpy(
        eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
        os_int flags = EOBJ_DEFAULT);

    /* Virtual destructor.
     */
    virtual ~Grumpy();

    /* Run the application.
     */
    virtual void run();
};

#endif
