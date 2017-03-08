/**

  @file    econtainer.h
  @brief   Simple object container.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The container object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECONTAINER_INCLUDED
#define ECONTAINER_INCLUDED

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eContainer : public eObject
{
public:
    /* Constructor.
	 */
	eContainer(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eContainer();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eContainer pointer.
     */
	inline static eContainer *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_CONTAINER)
		return (eContainer*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_CONTAINER;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eContainer *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eContainer(parent, id, flags);
    }

    /* Get next child container identified by oid.
     */
    eContainer *nextc(
        e_oid id = EOID_CHILD);

    /* Write container content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read container content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

#if E_SUPPROT_JSON
    /* Write container specific content to stream as JSON.
     */
    virtual eStatus json_writer(
        eStream *stream, 
        os_int sflags,
        os_int indent);
#endif

    /* Empty the container.
     */
    void clear();
};

#endif
