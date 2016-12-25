/**

  @file    ename.h
  @brief   Name class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The names map to name space. These are used to represent tree structure of named objects.
  The eName class is derived from eVariable class.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ENAME_INCLUDED
#define ENAME_INCLUDED



/**
****************************************************************************************************

  @brief eName class.

  The eName is a class derived from eVariable. It adds capanility to map to eNameSpace. 

****************************************************************************************************
*/
class eName : public eVariable
{
	friend class eNameSpace;

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
	eName(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eName();

    /* Cast eObject pointer to eName pointer. 
     */
	inline static eName *cast(
		eObject *o) 
	{ 
		return (eName*)o;
	}

    /* Get class identifier.
     */
    virtual os_int getclassid() {return ECLASSID_NAME;}

    /* Static constructor function for generating instance by class list.
     */
    static eName *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eName(parent, oid, flags);
    }

	/* Get next object identified by oid.
     */
	eName *ixgetnext(
		os_boolean name_match = OS_TRUE);

	/** Same as ixgetnext but used with names.
     */
	inline eName *getnextname()
	{
		return ixgetnext();
	}

    /** Get name space to which this name is mapped to.
     */
    inline eNameSpace *getnamespace() 
	{
		return m_index;
	}

    /*@}*/


	/** 
	************************************************************************************************

	  @name eObject virtual function implementations

	  X... 

	************************************************************************************************
	*/
	/*@{*/
    /* Write name to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read name from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);
    /*@}*/

protected:
	/** Pointer to left child in index'es red/black tree.
     */
    eName *m_ileft;

	/** Pointer to right child in index'es red/black tree.
     */
    eName *m_iright;

	/** Pointer to parent in index'es red/black tree.
     */
    eName *m_iup;

	/** Pointer to index.
     */
    eNameSpace *m_index;

	/** Tag this object as "red".
     */
    inline void ixsetred()
    {
        m_vflags |= EVAR_IS_RED;
    }

	/** Tag this object as "black".
     */
    inline void ixsetblack()
    {
        m_vflags &= ~EVAR_IS_RED;
    }
};

#endif
