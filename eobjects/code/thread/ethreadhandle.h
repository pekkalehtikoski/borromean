/**

  @file    ethreadhandle.h
  @brief   Thread handle class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  Thread handle is used for controlling threads from another thread.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ETHREAD_HANDLE_INCLUDED
#define ETHREAD_HANDLE_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eThreadHandle : public eObject
{
    friend class eThread;

public:
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/

    /* Constructor.
	 */
	eThreadHandle(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eThreadHandle();

    /* Casting eObject pointer to eThreadHandle pointer.
     */
	inline static eThreadHandle *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_THREAD_HANDLE)
		return (eThreadHandle*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_THREAD_HANDLE;}

    /* Static constructor function for generating instance by class list.
     */
    /* static eThreadHandle *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eThreadHandle(parent, oid, flags);
    } */

    /*@}*/

	/** 
	************************************************************************************************

	  @name X...

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    /* Request to terminate a thread.
     */
    void terminate();

    /* Wait until thread has terminated.
     */
    void join();

    inline os_char *uniquename() 
    {
        return m_unique_thread_name;
    }

    /*@}*/

private:
    inline void set_osal_handle(
        osalThreadHandle *h) 
    {
        m_osal_handle = h;
    }

    void save_unique_thread_name(
        eThread *thread);

	osalThreadHandle *m_osal_handle;

    os_char m_unique_thread_name[E_OIXSTR_BUF_SZ];
};

#endif
