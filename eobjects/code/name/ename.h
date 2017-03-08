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

/* Flags for addname()
 */
#define ENAME_PERSISTENT 0
#define ENAME_TEMPORARY 1
#define ENAME_PROCESS_NS 2
#define ENAME_THREAD_NS 4
#define ENAME_PARENT_NS 8
#define ENAME_THIS_NS 16
#define ENAME_NO_MAP 32

/* Enumeration of name space types.
 */
typedef enum
{
   E_PARENT_NS_TYPE = 0,
   E_PROCESS_NS_TYPE,
   E_THREAD_NS_TYPE,
   E_THIS_NS_TYPE,
   E_SPECIFIED_NS_TYPE
} 
eNameSpaceTypeEnum; 


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

      @name Generic object functionality.

      These functions enable using objects of this class as a generic eObject.

    ************************************************************************************************
    */
    /*@{*/
public:
	/* Constructor.
     */
	eName(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eName();

    /* Cast eObject pointer to eName pointer. 
     */
	inline static eName *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_NAME)
		return (eName*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_NAME;}

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eName *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eName(parent, id, flags);
    }

    /* Get next child name identified by oid.
     */
    eName *nextn(
        e_oid id = EOID_NAME);

	/* Get next name in namespace.
     */
	eName *ns_next(
		os_boolean name_match = OS_TRUE);

    /** Get name space to which this name is mapped to.
     */
    inline eNameSpace *nspace() 
	{
		return m_namespace;
	}

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


	/** 
	************************************************************************************************

      @name Name specific fuunctions

	  X... 

	************************************************************************************************
	*/
    
    /* Get name space identifier, if any, for the name.
     */
    os_char *namespaceid();

    /* Set name space identifier, if any, for the name.
     */
    void setnamespaceid(
        const os_char *namespace_id);

    /* Map the name to a name space.
     */
    eStatus mapname();

    /* Map the name to a name space given as argument.
     */
    eStatus mapname2(
        eNameSpace *ns,
        os_int info);

    /* Detach name from name space.
     */
    void detach();

protected:
    void clear_members();

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


    /** Namespace type.
     */
    eNameSpaceTypeEnum m_ns_type;

    /** True if te name is mapped to process name space.
     */
    os_boolean m_is_process_ns;

    /* Name space identifier string when m_ns_type is E_SPECIFIED_NS_TYPE.
     */
    eVariable *m_namespace_id;

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
    eNameSpace *m_namespace;
};

#endif
