/**

  @file    etimer.h
  @brief   Timer implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Object can enable or disable receiving ECMD_TIMER by calling base class'es eObject::timer()
  function. Timer base precision is 40ms, which is intended to be cast enough for animating 
  user interface objects at rate of 25Hz. When more precise timing is needed, it should be
  implemented by other means (for example by using loop containing os_sleep and alive() calls). 
  
  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ETIMER_INCLUDED
#define ETIMER_INCLUDED


/**
****************************************************************************************************

  @brief End point class.

  The eTimer is socket end point listening to specific TCP port for new connections.

****************************************************************************************************
*/
class eTimer : public eThread
{
public:
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */

	/* Constructor.
     */
	eTimer(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eTimer();

    /* Casting eObject pointer to eTimer pointer.
     */
	inline static eTimer *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_TIMER)
		return (eTimer*)o;
	}

	/* Get class identifier.
	 */
	virtual os_int classid() 
    { 
        return ECLASSID_TIMER; 
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
	static eTimer *newobj(
		eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
        return new eTimer(parent, id, flags);
	}

    /* Function to process incoming messages. 
     */
    void onmessage(
        eEnvelope *envelope);

    /* Enable/disable timer.
     */
    void settimer(
        os_long period_ms,
        os_char *name);

    /* Run the connection.
     */
    virtual void run();

protected:
};



#endif
