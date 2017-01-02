/**

  @file    ehandletable.h
  @brief   Handle table
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The handle table contains always EHANDLE_HANDLE_MAX+1 handles. Each handles is either in
  linked list of global friee handles, or reserved by thread.
  
  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EHANDLETABLE_INCLUDED
#define EHANDLETABLE_INCLUDED


/** Number of bits in object index specifying handle index. This and EHANDLE_HANDLE_MAX must match.
 */
#define EHANDLE_HANDLE_BITS 14

/** Maximum index for eHandle's m_handle array. s
 */
#define EHANDLE_HANDLE_MAX 0x3FFF


/**
****************************************************************************************************

  @brief Handle table class.

  The eHandleTable contains fixed length array of eHandle object.

****************************************************************************************************
*/
class eHandleTable
{
public:
    eHandleTable(e_oix oix);

    inline eHandle *firsthandle() {return m_handle;}

    /* Handle table xontent.
     */
    eHandle m_handle[EHANDLE_HANDLE_MAX + 1];
};

#endif

