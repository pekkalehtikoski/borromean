/**

  @file    ebinding.h
  @brief   Binding properties, DB tables and files.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The binding object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EBINDING_INCLUDED
#define EBINDING_INCLUDED


/* Flags for bind()
 */
#define EBIND_DEFAULT 0
#define EBIND_CLIENTINIT 1
#define EBIND_NOFLOWCLT 2
#define EBIND_ATTRIBUTES 4


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eBinding : public eObject
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
	eBinding(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eBinding();

    /* Clone an obejct.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eBinding pointer.
        */
	inline static eBinding *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_BINDING)
		return (eBinding*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_BINDING;}

    /* Static constructor function for generating instance by class list.
     */
    static eBinding *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eBinding(parent, oid, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name eObject virtual function implementations

	  X... 

	************************************************************************************************
	*/
	/*@{*/
    /* Write binding content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read binding content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /*@}*/

};

#endif
