/**

  @file    etimer.cpp
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
#include "eobjects/eobjects.h"

static const os_long base_step_ms = 40;

/**
****************************************************************************************************

  @brief Enable or disable timer for this object.

  If the eObject::timer() function is called with nonzero period_ms argument, timer is enabled.
  Enabling timer means that object will receive periodic ECMD_TIMER messages. Calling
  the function with zero argument, will disable the timer. 

  Notice that disabling timer or changing timer frequency doesn't effect immediately. Object
  may still for short while receive run messages after timer has been disabled. Reason for this
  is that period parameter is passed by message to timer thread.
  
  @param  period_ms How often to receive ECMD_RUN message in milliseconds, or zero to disable
          the timer. This will be rounded to 40 ms precision.

  @return None.

****************************************************************************************************
*/
void eObject::timer(
    os_long period_ms) 
{
    eVariable period;

    period = period_ms;
    message(ECMD_SETTIMER, "//_timer", OS_NULL, &period);
}


/**
****************************************************************************************************

  @brief Constructor.

  Name timer object as "_timer" and enable name space for it.

  @return  None.

****************************************************************************************************
*/
eTimer::eTimer(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eThread(parent, id, flags)
{
	addname("//_timer");
    ns_create();
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eTimer::~eTimer()
{
}


/**
****************************************************************************************************

  @brief Add eTimer to class list and class'es properties to it's property set.

  The eTimer::setupclass function adds eTimer to class list. The class list enables creating 
  new objects dynamically by class identifier, which is used for serialization functions.
  It is also used to access class name.

****************************************************************************************************
*/
void eTimer::setupclass()
{
    const os_int cls = ECLASSID_TIMER;

    /* Synchronize, add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eTimer");
    os_unlock();
}


/**
****************************************************************************************************

  @brief Function to process incoming messages. 

  The onmessage function handles messages received by object. 
  
  @param   envelope Message envelope. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eTimer::onmessage(
    eEnvelope *envelope)
{
    eVariable *v, *n;

    /* If this timer setting command to this object.
     */
    if (*envelope->target() == '\0')
    {
        switch (envelope->command())
        {
            case ECMD_SETTIMER:
                v = eVariable::cast(envelope->content());
                settimer(v->getl(), envelope->source());
                return;

            case ECMD_NO_TARGET:
                n = eVariable::cast(envelope->context());
                if (n)
                {
                    v = ns_getv(n->gets(), eobj_this_ns);
                    delete v;
                }
                return;
        }
    }

    /* Call base class'es message processing.
     */
    eThread::onmessage(envelope);
}


/**
****************************************************************************************************

  @brief Enable/disable timer.

  The eTimer::settimer() function...

  @return  None.

****************************************************************************************************
*/
void eTimer::settimer(
    os_long period_ms,
    os_char *name)
{
    e_oid step;
    eVariable *t = OS_NULL;
    eName *n;

    /* Convert period to 40ms steps.
     */
    step = (e_oid)((period_ms + base_step_ms - 1) / base_step_ms);
    if (step < 1) step = 1;

    /* If we have variable for this timer.
     */
    n = ns_first(name, eobj_this_ns);
    if (n) t = eVariable::cast(n->parent());
    if (t)
    {
        /* If we are to delete this.
         */
        if (period_ms == 0)
        {
            delete t;
            return;
        }

#if OSAL_DEBUG
        /* If period has not changed warn user.
         */
        if (step == t->oid()) 
        {
            osal_debug_error("repeated enable timer");
            return;
        }
#endif
        delete t;
    }

#if OSAL_DEBUG
    /* No timer, check if we are repeatedly disabling it.
     */
    else if (period_ms == 0)
    {
        osal_debug_error("repeated disable timer");
    }
#endif

    /* If we are not disabling timer, set it up
     */
    if (period_ms)
    {
        t = new eVariable(this, step);
        t->addname(name, ENAME_PARENT_NS);
    }
}


/**
****************************************************************************************************

  @brief Run the timer thread.

  The eTimer::run() function checks when timer ...

  @return  None.

****************************************************************************************************
*/
void eTimer::run()
{
    eVariable *t, *nextt, context;
    eName *name;
    os_long step = 1;
    e_oid id;

    while (!exitnow())
    {
        alive(EALIVE_RETURN_IMMEDIATELY);

        for (t = firstv(); t; t = nextt)
        {
            nextt = t->nextv();
            id = t->oid();
            if (id < 1) continue;

            if ((step % id) == 0)
            {
                name = t->firstn(EOID_NAME);
                context = *name;
                message(ECMD_TIMER, name->gets(), OS_NULL, OS_NULL, EMSG_KEEP_CONTEXT, &context);
            }
        }

        os_sleep(base_step_ms);
        step++;
    }
}
