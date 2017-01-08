/**

  @file    epropertybinding.h
  @brief   Simple object propertybinding.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The propertybinding object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EPROPERTYBINDING_INCLUDED
#define EPROPERTYBINDING_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class ePropertyBinding : public eBinding
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
	ePropertyBinding(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~ePropertyBinding();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to ePropertyBinding pointer.
     */
	inline static ePropertyBinding *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_PROPERTY_BINDING)
		return (ePropertyBinding*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_PROPERTY_BINDING;
    }

    /* Static constructor function for generating instance by class list.
     */
    static ePropertyBinding *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new ePropertyBinding(parent, oid, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name eObject virtual function implementations

	  X... 

	************************************************************************************************
	*/
	/*@{*/
    /* Write propertybinding content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read propertybinding content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Binding functions

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    /* Bind property.
     */
    void bind(
        os_int localpropertynr,
        os_char *remotepath,
        os_char *remoteproperty,
        os_int bflags);

    /*@}*/

};

#endif
