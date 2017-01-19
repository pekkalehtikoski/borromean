/**

  @file    eobjects_thread_example.cpp
  @brief   Example code about naming objects.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  This example demonstrates how to create threads.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"
#include "eobjects_thread_example.h"

/* Purpose of a message is specified by 32 bit command. Negative command identifiers are
   reserved for the eobject library related, but positive ones can be used freely.
   Here commands are:
 */
#define MYCMD_WAKE_UP 10
#define MYCMD_HI_COMRADE 11
#define MYCMD_HI_OTHER_COMRADE 12

/* Every class needs to have unique class identifier (classid). Class identifier is is 32 bit
   integer. Class identifiers starting from ECLASSID_APP_BASE are reserved for the application.
 */
#define MY_CLASS_ID_1 (ECLASSID_APP_BASE + 1)
#define MY_CLASS_ID_2 (ECLASSID_APP_BASE + 2)


/**
****************************************************************************************************

  @brief Example thread class.

  X...

****************************************************************************************************
*/
class eMyThread1 : public eThread
{
    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_1;
    }

    virtual void onmessage(
        eEnvelope *envelope) 
    {
        eVariable *txt, *v;

        /* If at final destination for the message.
         */
        if (*envelope->target()=='\0')
        {
            switch (envelope->command())
            {
                case MYCMD_WAKE_UP:
                    v = eVariable::cast(envelope->content());
                    osal_console_write(v ? v->gets() : "NULL");
                    osal_console_write(" *** WAKE UP (t1)\n");

                    txt = new eVariable(this);
                    txt->sets("hi, anyone there?");
                    message (MYCMD_HI_COMRADE, "//thread2", OS_NULL, txt, EMSG_DEL_CONTENT);
                    return;

                case MYCMD_HI_OTHER_COMRADE:
                     v = eVariable::cast(envelope->content());
                    osal_console_write(v ? v->gets() : "NULL");
                    osal_console_write(" *** HI OTHER COMRADE (t1)\n");
                    return;

                case ECMD_NO_TARGET:
                    osal_console_write(envelope->source());
                    osal_console_write(" *** NO TARGET (t1)\n");
                    return;
            }
        }

        eThread::onmessage(envelope);
    }
};


/**
****************************************************************************************************

  @brief Example thread class.

  X...

****************************************************************************************************
*/
class eMyThread2 : public eThread
{
    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return MY_CLASS_ID_2;
    }

    virtual void onmessage(
        eEnvelope *envelope) 
    {
        eVariable *txt, *v;

        /* If at final destination for the message.
         */
        if (*envelope->target()=='\0')
        {
            switch (envelope->command())
            {
                case MYCMD_HI_COMRADE:
                    v = eVariable::cast(envelope->content());
                    osal_console_write(v ? v->gets() : "NULL");
                    osal_console_write(" *** HI_COMRADE (t2)\n");

                    txt = new eVariable(this);
                    txt->sets("hi, I am here?");
                    message (MYCMD_HI_OTHER_COMRADE, envelope->source(), 
                        OS_NULL, txt, EMSG_DEL_CONTENT, envelope->context());
                    return;
            }
        }

        eThread::onmessage(envelope);
    }
};


/**
****************************************************************************************************

  @brief Thread example 1.

  The thread_example_2() function...

  @return  None.

****************************************************************************************************
*/
void thread_example_2()
{
	eContainer
		root;

    eVariable
        *txt;

    eThread
        *t;

    eThreadHandle 
        thandle1,
        thandle2;

    /* Create and start thread 1 named "thread1".
     */
    t = new eMyThread1();
	t->addname("thread1", ENAME_PROCESS_NS);
//    t->setpriority();
    t->start(&thandle1); /* After this t pointer is useless */

    /* Create and start thread 2 named "thread2".
     */
    t = new eMyThread2();
	t->addname("thread2", ENAME_PROCESS_NS);
    t->start(&thandle2); /* After this t pointer is useless */

    for (os_int i = 0; i<2; i++)
    {
        osal_console_write("master running\n");

        txt = new eVariable(&root);
        txt->sets("message content");
        root.message (MYCMD_WAKE_UP, "//thread1", OS_NULL, txt, EMSG_DEL_CONTENT|EMSG_NO_REPLIES);

        os_sleep(2000);
    }

    /* Wait for thread to terminate
     */
    thandle1.terminate();
    thandle2.terminate();
    thandle1.join();
    thandle2.join();
}
