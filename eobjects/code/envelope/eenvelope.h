/**

  @file    eenvelope.h
  @brief   Envelope class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  Messages are sent as envelope objects. The eEnvelope contains recipient and senders's addresses,
  command, message content and other data.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EENVELOPE_INCLUDED
#define EENVELOPE_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eEnvelope : public eObject
{
	/** 
	************************************************************************************************

	  @name Constructor, destructor, etc.

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
    /** Constructor.
	 */
	eEnvelope(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eEnvelope();

    /* Casting eObject pointer to eEnvelope pointer.
        */
	inline static eEnvelope *cast(
		eObject *o) 
	{ 
		return (eEnvelope*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_ENVELOPE;}

    /* Static constructor function for generating instance by class list.
     */
    static eEnvelope *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eEnvelope(parent, oid, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name Envelope message buffer

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    /* Get next message to envelope to process.
     */
    void settarget();

    /*@}*/

};

#endif
