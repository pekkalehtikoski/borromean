/**

  @file    eprocess.h
  @brief   Simple object process.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The process object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EPROCESS_INCLUDED
#define EPROCESS_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eProcess : public eThread
{
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/
public:
    /* Constructor.
	 */
	eProcess(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eProcess();

    /* Casting eObject pointer to eProcess pointer.
     */
	inline static eProcess *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_PROCESS)
		return (eProcess*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_PROCESS;}

    /*@}*/

	/** 
	************************************************************************************************

	  @name Thread functionality

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    virtual void initialize(
        eContainer *params = OS_NULL);

    virtual void run();

    virtual void onmessage(
        eEnvelope *envelope);

    /*@}*/

};


/* Create eProcess object and start a thread to run it.
 */;
void eprocess_create();

/* Terminate eProcess thread and clean up.
 */
void eprocess_close();

#endif
