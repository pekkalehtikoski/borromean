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

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/
public:
    /* Constructor.
	 */
	eSet(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eSet();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
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
    virtual os_int classid()
    {
        return ECLASSID_SET;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eSet *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eSet(parent, id, flags);
    }

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

#if E_SUPPROT_JSON
    /* Write set to stream as JSON.
     */
    eStatus json_writer(
        eStream *stream, 
        os_int sflags,
        os_int indent);
#endif

    /*@}*/

	/** 
	************************************************************************************************

      @name Setting and getting item values.

	  A value can be stored into set using set() function. If the value with this id exists,
      it is overwritten. Values are retrieved by get() function.

	************************************************************************************************
	*/
	/*@{*/

    /* Store value into set.
     */
    /* virtual */ void set(
        os_int id,
        eVariable *x,
        os_int sflags = 0);

    /* Store value into set.
     */
    inline void setl(
        os_int id, 
        os_long x)
    {
        eVariable v;
        v.setl(x);
        set(id, &v); // ??????????? CHECK SHOULD FLAGS BE ADDED
    }

    /* Store value into set.
     */
    inline void setd(
        os_int id, 
        os_double x)
    {
        eVariable v;
        v.setd(x);
        set(id, &v);
    }

    /* Store value into set.
     */
    inline void sets(
        os_int id, 
        const os_char *x)
    {
        eVariable v;
        v.sets(x);
        set(id, &v);
    }

    /* Get value from set.
     */
    /* virtual */ os_boolean get(
        os_int id,
        eVariable *x);

    /* Get value as integer.
     */
    inline os_long getl(
        os_int id)
    {
        eVariable v;
        get(id, &v);
        return v.getl();
    }

    /* Get value as double.
     */
    inline os_double getd(
        os_int id)
    {
        eVariable v;
        get(id, &v);
        return v.getd();
    }

    /* Clear the set.
     */
    void clear();

    /*@}*/

protected:
    /* Buffer containing items
     */
    os_char *m_items;

    /* Buffer used, bytes.
     */
    os_int m_used;

    /* Buffer allocated, bytes.
     */
    os_int m_alloc;
};

#endif
