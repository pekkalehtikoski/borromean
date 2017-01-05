/**

  @file    eset.h
  @brief   Simple set.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The set stores enumerated collection of values.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ESET_INCLUDED
#define ESET_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eSet : public eObject
{
	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
    /** Constructor.
	 */
	eSet(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eSet();

    /* Clone an obejct.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eSet pointer.
        */
	inline static eSet *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_SET)
		return (eSet*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_SET;}

    /* Static constructor function for generating instance by class list.
     */
    static eSet *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eSet(parent, oid, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name eObject virtual function implementations

	  X... 

	************************************************************************************************
	*/
	/*@{*/
    /* Write set content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read set content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /*@}*/

};

#endif
