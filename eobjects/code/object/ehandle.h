/**

  @file    ehandle.h
  @brief   Object handle base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The eHandle is base class for all objects. 
  
  - Functions to manage object hierarchy and idenfify objects.
  - Functions to clone objects.
  - Serialization functions.
  - Messaging.
  - Functions to access object properties.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EHANDLE_INCLUDED
#define EHANDLE_INCLUDED

/* Do we want to debug object tree
 */
#define EOBJECT_DBTREE_DEBUG 1

class eObject;
class eHandleTable;
class eStream;
class eVariable;
class eNameSpace;
class eName;
class eRoot;
class ePointer;


/**
****************************************************************************************************

@name Object flags

X...


****************************************************************************************************
*/
/*@{*/
#define EOBJ_DEFAULT       0x00000000
#define EOBJ_IS_ATTACHMENT 0x00000001
#define EOBJ_HAS_NAMESPACE 0x00000002
#define EOBJ_NOT_CLONABLE  0x00000004
#define EOBJ_NOT_SERIALIZABLE  0x00000008

#define EOBJ_CUST_FLAG1    0x00000010
#define EOBJ_CUST_FLAG2    0x00000020
#define EOBJ_CUST_FLAG3    0x00000040
#define EOBJ_CUST_FLAG4    0x00000080
#define EOBJ_CUST_FLAG5    0x00000100

/** Flag EOBJ_FAST_DELETE is used internallly to speed up deletion: When an object is being deleted,
there is no need to detach it's children from red/black of the object. The EOBJ_FAST_DELETE
flag is set by delete_children() to child object, so child object doesn't need to preserve
the treee structure after deletion. This changes deleting object trees from NlogN to N algorithm.
 */
#define EOBJ_FAST_DELETE   0x20000000

/** Red/black tree's red or black node bit.
 */
#define EOBJ_IS_RED        0x40000000

/** Serialization mask, which bits to save.
 */
#define EOBJ_SERIALIZATION_MASK 0x0000FFFF

/** Custom flags mask.
 */
#define EOBJ_CUST_FLAGS_MASK 0x0001F0

/** Clone mask, which bits to clone.
 */
#define EOBJ_CLONE_MASK EOBJ_SERIALIZATION_MASK

/*@}*/



/**
****************************************************************************************************

  @brief Handle class.

  The eHandle is class for indexing the thread's object tree.
  networked objects.

****************************************************************************************************
*/
class eHandle
{
	friend class eObject;
	friend class eHandleTable;
	friend class eRoot;
	friend class ePointer;

public:

    /** 
    ************************************************************************************************

      @name Object flags

      X..

    ************************************************************************************************
    */
    /*@{*/

    /** Get object flags.
     */
    inline os_int flags()
    {
        return m_oflags;
    }

    /** Set specified object flags.
     */
    inline void setflags(
		os_int flags)
    {
        m_oflags |= flags;
    }

    /** Clear specified object flags.
     */
    inline void clearflags(
		os_int flags)
    {
        m_oflags &= ~flags;
    }

    /** If object can be cloned?
     */
    inline os_boolean isclonable()
    {
        return (m_oflags & EOBJ_NOT_CLONABLE) == 0;
    }

    /** Check if object is an attachment. Returns nonzero if object is an attachment.
     */
    inline os_boolean isattachment()
    {
        return m_oflags & EOBJ_IS_ATTACHMENT;
    }

    /** Check if object is a serializable attachment. 
     */
    inline os_boolean isserattachment()
    {
        return (m_oflags & (EOBJ_IS_ATTACHMENT|EOBJ_NOT_SERIALIZABLE)) 
                == EOBJ_IS_ATTACHMENT;
    } 


    /*@}*/


    /** 
    ************************************************************************************************

      @name Object hierarcy

      X...

    ************************************************************************************************
    */
    /*@{*/

    /** Get object identifier.
     */
    inline os_int oid() 
    {
        return m_oid;
    }

	/** Get object index. 
     */
    inline os_int oix() 
    {
        return m_oix;
    }
	
	/** Get reuse counter. If reuse counter is unused (negative), mark used and icrement.
     */
    inline os_int ucnt() 
    {
        if (m_ucnt<=0) m_ucnt = -m_ucnt + 1; 
        return m_ucnt;
    }

	/** Sets reuse counter to negative. This marks that reuse counter needs to be incremented
        next time it is used.
     */
    inline void ucnt_mark_unused() 
    {
        if (m_ucnt>0) m_ucnt = -m_ucnt; 
    }

    /** Get object pointer.
     */
    inline eObject *object() 
    {
        return m_object;
    }

    /** Get root handle.
     */
    inline eRoot *root() 
    {
        return m_root;
    }

    /* Get number of childern.
     */
    os_long childcount(
        e_oid id = EOID_CHILD);

	/* Get first child object identified by oid.
     */
    eHandle *first(
        e_oid id = EOID_CHILD);

	/* Get last child object identified by oid.
     */
    eHandle *last(
        e_oid id = EOID_CHILD);

	/* Get next object identified by oid.
     */
    eHandle *next(
        e_oid id = EOID_CHILD);

	/* Get previous object identified by oid.
     */
    eHandle *prev(
        e_oid id = EOID_CHILD);

	/* Right pointer is used to manage linked lists of reserved but unused handles.
     */
    inline eHandle *right()
    {
        return m_right;
    }

    inline void setright(eHandle *h)
    {
        m_right = h;
    }

	/** Save object identifier, clear flags, mark new node as red,
		not part of object hierarcy, nor no children yet.
     */
    inline void clear(
        eObject *obj,
        e_oid id,
        os_int flags)
	{
        m_oid = id;
		m_oflags = EOBJ_IS_RED | flags;
		m_object = obj;
    }

    /* Verify whole object tree.
     */
#if EOBJECT_DBTREE_DEBUG
    void verify_whole_tree();
#endif

protected:


	/** Check if object is "red". The function checks if the object n is tagged as "red"
		in red/black tree.
     */
    inline os_int isred(
        eHandle *n)
    {
        if (n == OS_NULL) return OS_FALSE;
        return n->m_oflags & EOBJ_IS_RED;
    }

	/** Check if object is "black". The function checks if the object n is tagged as 
		"black" in red/black tree.
     */
    inline os_int isblack(
        eHandle *n)
    {
        if (n == OS_NULL) return OS_TRUE;
        return (n->m_oflags & EOBJ_IS_RED) == 0;
    }

	/** Tag this object as "red".
     */
    inline void setred()
    {
        m_oflags |= EOBJ_IS_RED;
    }

	/** Tag this object as "black".
     */
    inline void setblack()
    {
        m_oflags &= ~EOBJ_IS_RED;
    }

#if EOBJECT_DBTREE_DEBUG
	/* Red/Black tree: Get grandparent.
     */
    eHandle *rb_grandparent(
        eHandle *n); 

	/* Red/Black tree: Get sibling.
     */
    eHandle *sibling(
        eHandle *n); 

	/* Red/Black tree: Get uncle.
     */
    eHandle *uncle(
        eHandle *n); 

	/* Red/Black tree: Verify tree integrity.
     */
    void verify_properties(); 

	/* Red/Black tree: Verify that root node is black.
     */
    void verify_property_2(); 

	/* Red/Black tree: Verify that every red node has two black children.
     */
    void verify_property_4(
        eHandle *n); 

	/* Red/Black tree: Verify that number of black nodes on match.
     */
    void verify_property_5(); 

	/* Red/Black tree: Helper function for verify_property_5().
     */
    void verify_property_5_helper(
        eHandle *n, 
        int black_count, 
        int *path_black_count); 

    /* Verify one node of object tree.
     */
    void verify_node(
        eRoot *root);

    /* Verify all child objects.
     */
    void verify_children(
        eRoot *root);

#else
    /** Red/Black tree: Get grand parent.
     */
    inline static eHandle *rb_grandparent(
        eHandle *n)
    {
        return n->m_up->m_up;
    }

	/** Red/Black tree: Get sibling.
     */
    inline eHandle *sibling(
        eHandle *n)
    {
        return (n == n->m_up->m_left) 
            ? n->m_up->m_right 
            : n->m_up->m_left;
    }

	/** Red/Black tree: Get uncle.
     */
    inline eHandle *uncle(
        eHandle *n)
    {
        return sibling(n->m_up);
    }

#endif
	/* Delete all child objects.
     */
    void delete_children();

	/* Red/Black tree: Rotate tree left.
     */
    void rotate_left(
		eHandle *n);

	/* Red/Black tree: Rotate tree right.
     */
    void rotate_right(
		eHandle *n);

	/* Red/Black tree: Replace a node by another node.
     */
    void replace_node(
		eHandle *oldn,
		eHandle *newn);

	/* Red/Black tree: Insert a node to red black tree.
     */
    void rbtree_insert(
        eHandle *inserted_node); 

	/* Red/Black tree: Balance red/black tree after inserting node.
     */
    void insert_case1(
        eHandle *n); 

	/* Red/Black tree: Balance red/black tree after inserting node.
     */
    void insert_case4(
        eHandle *n); 

	/* Red/Black tree: Remove node from red/black.
     */
    void rbtree_remove(
    	eHandle *n);

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void delete_case2(
		eHandle *n);

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void delete_case4(
		eHandle *n);

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void delete_case5(
		eHandle *n);

	/* Red/Black tree: Balance red/black tree after removing node.
     */
    void delete_case6(
		eHandle *n);

    /** Object index. (other theads can access this)
     */
	e_oix m_oix;
	
    /** Reuse counter (other theads can access this).
     */
	os_int m_ucnt;

	/** Object identifier.
     */
    e_oid m_oid;

    /** Object flags.
     */
    os_int m_oflags;

	/** Left child in red/black tree.
     */
    eHandle *m_left;

	/** Right child in red/black tree.
     */
    eHandle *m_right;

	/** Parent in red/black tree.
     */
    eHandle *m_up;

    /** Pointer to the C++ object (If this object is thread
        (has message queue) other theads can access this).
     */
    eObject *m_object; 

    /** Root of the object of the index tree (other theads can access this).
     */
    eRoot *m_root;

	/** Root child object handle in red/black tree.
     */
    eHandle *m_children;
};

#endif



