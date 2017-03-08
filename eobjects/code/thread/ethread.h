/**

  @file    ethread.h
  @brief   Thread class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  The thread object is the root of thread's object tree.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ETHREAD_INCLUDED
#define ETHREAD_INCLUDED

/* Flags for alive() function.
 */
#define EALIVE_WAIT_FOR_EVENT 1
#define EALIVE_RETURN_IMMEDIATELY 0


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eThread : public eObject
{
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
	eThread(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eThread();

    /* Casting eObject pointer to eThread pointer.
        */
	inline static eThread *cast(
		eObject *o) 
	{ 
        if (o) 
        {
            if (o->isthread()) return (eThread*)o;
            osal_debug_assert(0);
        }
        return OS_NULL;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_THREAD;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();


    /* Return OS_TRUE if object is thread (derived). 
     */
    virtual os_boolean isthread() 
    {
        return OS_TRUE;
    }

    /* Static constructor function for generating instance by class list.
     */
    static eThread *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eThread(parent, id, flags);
    } 

    virtual void onmessage(
        eEnvelope *envelope);

    inline osalEvent trigger() {return m_trigger;}

    /*@}*/

	/** 
	************************************************************************************************

	  @name Thread message buffer

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    /* Create operating system thread and start running 
     */
    void start(
        eThreadHandle *thandle = OS_NULL,
        eContainer *params = OS_NULL);

    virtual void initialize(
        eContainer *params = OS_NULL) {};

    virtual void run();

    virtual void finish() {};

    /* Check if thread exit is requested.
     */
    inline os_boolean exitnow() 
    {
        return m_exit_requested;
    }

    /* Get next message to thread to process.
     */
    void queue(
        eEnvelope *envelope,
        os_boolean delete_envelope = OS_TRUE);

    /* Get next message to thread to process.
     */
    void alive(
        os_int flags = EALIVE_WAIT_FOR_EVENT);

    /*@}*/

protected:
    /* Thread triggger. 
     */
    osalEvent m_trigger;

    /* Message queue for incoming messages.
     */
    eContainer *m_message_queue;

    /* Exit requested
     */
    os_boolean m_exit_requested;
};

#endif
