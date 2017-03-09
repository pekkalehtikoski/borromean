/**

  @file    enamespace.cpp
  @brief   Name space class implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This file implements eNameSpace class to which indexed variables of eName can map to.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ENAMESPACE_INCLUDED
#define ENAMESPACE_INCLUDED

/* Set to 1 to debug red/black tree for the name space.
 */
#define EINDEX_DBTREE_DEBUG 0

/* Name space identifiers. These are followed by '/', thus for example path to thread looks like 
   "/myobject..." or process "//myobject".
 */
#define E_PROCESS_NS "/"
#define E_THREAD_NS ""
#define E_PARENT_NS ".."
#define E_THIS_NS "."

/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.

****************************************************************************************************
*/
class eNameSpace : public eObject
{
	friend class eName;

    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as a generic eObject.

    ************************************************************************************************
    */
    /*@{*/
public:
    /* Construct new eObject.
     */
    eNameSpace(
        eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

    /* Delete eObject, virtual destructor.
     */
    virtual ~eNameSpace();

    /* Clone name space
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id,
        os_int aflags);

    /* Cast eObject pointer to eNameSpace pointer. 
     */
	inline static eNameSpace *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_NAMESPACE)
		return (eNameSpace*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_NAMESPACE;
    }

    /* Static constructor function for generating instance by class list.
     */
    /* static eNameSpace *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eNameSpace(parent, oid, flags);
    } */

    /*@}*/


    /** 
    ************************************************************************************************

      @name Name space specific functions

      X...

    ************************************************************************************************
    */
    /*@{*/

    /* Get name space id, OS_NULL if none.
     */
    inline eVariable *namespaceid() 
    {
        return m_namespace_id;
    }

    /* Set name space id.
     */
    inline void setnamespaceid(
        eVariable *nsid) 
    {
        m_namespace_id = nsid;
    }

	/* Get first child object with specific name.
     */
    eName *findname(
        eVariable *x = OS_NULL);


    /*@}*/

protected:
	/** Pointer to root object of indexed variables.
     */
    eName *m_ixroot;

	/** Check if object is "red". The function checks if the object n is tagged as "red"
		in red/black tree.
     */
    inline os_int ixisred(
        eName *n)
    {
        if (n == OS_NULL) return OS_FALSE;
        return n->m_vflags & EVAR_IS_RED;
    }

	/** Check if object is "black". The function checks if the object n is tagged as 
		"black" in red/black tree.
     */
    inline os_int ixisblack(
        eName *n)
    {
        if (n == OS_NULL) return OS_TRUE;
        return (n->m_vflags & EVAR_IS_RED) == 0;
    }


#if EINDEX_DBTREE_DEBUG
	/* Red/Black tree: Get grandparent.
     */
    eName *ixgrandparent(
        eName *n); 

	/* Red/Black tree: Get sibling.
     */
    eName *ixsibling(
        eName *n); 

	/* Red/Black tree: Get uncle.
     */
    eName *ixuncle(
        eName *n); 

	/* Red/Black tree: Verify tree integrity.
     */
    void ixverify_properties(); 

	/* Red/Black tree: Verify that root node is black.
     */
    void ixverify_property_2(); 

	/* Red/Black tree: Verify that every red node has two black children.
     */
    void ixverify_property_4(
        eName *n); 

	/* Red/Black tree: Verify that number of black nodes on match.
     */
    void ixverify_property_5(); 

	/* Red/Black tree: Helper function for verify_property_5().
     */
    void ixverify_property_5_helper(
        eName *n, 
        int black_count, 
        int *path_black_count); 
#else
	/** Red/Black tree: Get grand parent.
     */
    inline eName *ixgrandparent(
        eName *n)
    {
        return n->m_iup->m_iup;
    }

	/** Red/Black tree: Get sibling.
     */
    inline eName *ixsibling(
        eName *n)
    {
        return (n == n->m_iup->m_ileft) 
            ? n->m_iup->m_iright 
            : n->m_iup->m_ileft;
    }

	/** Red/Black tree: Get uncle.
     */
    inline eName *ixuncle(
        eName *n)
    {
        return ixsibling(n->m_iup);
    }

#endif
	/* Delete all child objects.
     */
    void unmap_all();

	/* Red/Black tree: Rotate tree left.
     */
    void ixrotate_left(
        eName *n); 

	/* Red/Black tree: Rotate tree right.
     */
    void ixrotate_right(
        eName *n); 

	/* Red/Black tree: Replace a node by another node.
     */
    void ixreplace_node(
        eName *oldn, 
        eName *newn); 

	/* Red/Black tree: Insert a node to red black tree.
     */
    void ixrbtree_insert(
        eName *inserted_node); 

	/* Red/Black tree: Balance red/black tree after inserting node.
     */
    void ixinsert_case1(
        eName *n); 

	/* Red/Black tree: Balance red/black tree after inserting node.
     */
    void ixinsert_case4(
        eName *n); 

	/* Red/Black tree: Remove node from red/black.
     */
    void ixrbtree_remove(
        eName *n);

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void ixdelete_case2(
        eName *n); 

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void ixdelete_case4(
        eName *n); 

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void ixdelete_case5(
        eName *n); 

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void ixdelete_case6(
        eName *n); 

    eVariable *m_namespace_id;
};

#endif



