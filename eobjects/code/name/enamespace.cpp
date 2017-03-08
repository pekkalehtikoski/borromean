/**

  @file    enamespace.cpp
  @brief   Name space class implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.10.2011

  This file implements eNameSpace class to which indexed variables of eName can map to.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************

****************************************************************************************************

  Red black tree code was adopted from en.literateprograms.org:

  Copyright (c) 2010 the authors listed at the following URL, and/or
  the authors of referenced articles or incorporated external code:
  http://en.literateprograms.org/Red-black_tree_(C)?action=history&offset=20090121005050

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Retrieved from: http://en.literateprograms.org/Red-black_tree_(C)?oldid=16016


****************************************************************************************************
*/

/** 
****************************************************************************************************

  A red-black tree is a type of self-balancing binary search tree typically used to implement 
  associative arrays. It has O(log n) worst-case time for each operation and is quite efficient 
  in practice. Unfortunately, it's also quite complex to implement, requiring a number of subtle 
  cases for both insertion and deletion.


****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Object constructor.

  The eObject constructor creates an empty object. This constructor is called when object of
  any eobject library is constructed.

  @param   parent Pointer to parent object, or OS_NULL if the object is an orphan. If parent
           object is specified, the object will be deleted when parent is deleted.
  @param   oid Object identifier for new object. If not specified, defauls to EOID_ITEM (-1), 
		   which is generic list item.

  @return  None.

****************************************************************************************************
*/
eNameSpace::eNameSpace(
    eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
	/* If this is name space.
	 */
    if (id == EOID_NAMESPACE)
	{
		/* Flag this as attachment.
		 */
		setflags(EOBJ_IS_ATTACHMENT /* |EOBJ_NOT_SERIALIZABLE ?? */);

		/* If we have parent object, flag parent that it has name space.
		 */
		if (parent) parent->setflags(EOBJ_HAS_NAMESPACE);
	}

    m_namespace_id = OS_NULL;
    m_ixroot = OS_NULL;
}


/**
****************************************************************************************************

  @brief Object destructor.

  The eNameSpace destructor releases all child objects and removes object from parent's child
  structure,

  @return  None.

****************************************************************************************************
*/
eNameSpace::~eNameSpace()
{
	eObject
		*p;

    eName
        *n;

    /* Unmap indexed variables.
     */
	// if (m_root) delete_children();

    /* Detach all names from name space.
     */
    while ((n = findname()))
    {
        if (n->nspace()) n->detach();
    }

	/* If this is name space.
	 */
	if (oid() == EOID_NAMESPACE)
	{
		p = parent();

		/* If we have parent object, flag parent that it has name space.
		 */
		if (p) p->clearflags(EOBJ_HAS_NAMESPACE);
	}
}


/**
****************************************************************************************************

  @brief Clone name space

  The clone function clones the name space object and clonable attachments. 
  Names will be left detached in clone if EOBJ_NO_MAP flag is given.

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names. 
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eNameSpace::clone(
    eObject *parent, 
    e_oid id,
    os_int aflags)
{
    eNameSpace *clonedobj;
    clonedobj = new eNameSpace(parent, id == EOID_CHILD ? oid() : id, flags());

    if (m_namespace_id)
    {
        clonedobj->m_namespace_id 
            = eVariable::cast(m_namespace_id->clone(clonedobj, EOID_CHILD, EOBJ_NO_MAP));
    }
  
    /* Copy clonable attachments.
     */
    clonegeneric(clonedobj, aflags);
    return clonedobj;
}


/**
****************************************************************************************************

  @brief Delete all child objects.

  The eNameSpace::unmap_all() function deletes all children of this object. This is faster code
  to delete all children, balancing of the red/black tree is not maintained while deleting.

  @return  None.

****************************************************************************************************
*/
void eNameSpace::unmap_all()
{
	eName
		*n,
		*p;

	n = m_ixroot;

	while (OS_TRUE)
	{
		while (OS_TRUE)
		{
			p = n->m_ileft;
			if (p == OS_NULL) 
			{
				p = n->m_iright;
				if (p == OS_NULL) break;
			}
			n = p;
		}

		p = n->m_iup;

		if (p) 
		{
			if (p->m_ileft == n) p->m_ileft = OS_NULL;
			else p->m_iright = OS_NULL;
		}
		else
		{
			break;
		}

		n = p;
	}
}


/**
****************************************************************************************************

  @brief Get first child object identified by oid.

  The eNameSpace::findname() function returns pointer to the first child object selected by object 
  identifier oid given as argument.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to get a child object, 
		   which is not flagged as an attachment. Value EOID_ALL specifies to get pointer to
		   the first child object, regardless wether it is attachment or not. Other values
		   specify object identifier, only pointer to object with specified object
		   identifier is returned. 

  @return  Pointer to child object, or OS_NULL if no matching child object was found. 

****************************************************************************************************
*/
eName *eNameSpace::findname(
    eVariable *x)
{
    eName
        *n,
        *m;

	os_int
		c;

    /* Set n to point root of child object's red/black tree.
     */
    n = m_ixroot;

    /* Handle special object identifiers to get first child object
       of any id, with or without attachments.
     */
    if (x == OS_NULL) 
    {
        /* If no child objects, just return null.
         */
        if (n == OS_NULL) return OS_NULL;

        /* Move to first child object.
         */
        while (n->m_ileft) n = n->m_ileft;

        return n;
    }
    
    /* Handle normal case where child object is searched by exactly
       matching object identifier.
     */
    while (n != OS_NULL) 
    {
		c = x->compare(n);

        /* If object identifier matches, check if there
           is earlier match.
         */
        if (c == 0) 
        {
            m = n->m_ileft;
            if (m == OS_NULL) break;
			if (x->compare(m)) break;
            n = m;
        } 

        /* Smaller, search to the left.
         */
        else if (c < 0) 
        {
            n = n->m_ileft;
        } 

        /* Bigger, search to to right.
         */
        else 
        {
            n = n->m_iright;
        }
    }

    /* Return object pointer or OS_NULL if none found.
     */
    return n;
}


#if EINDEX_DBTREE_DEBUG
/**
****************************************************************************************************

  @brief Red/Black tree: Get ixgrandparent.

  The eNameSpace::ixgrandparent() function returns pointer to object's ixgrandparent in red/black tree.
  The ixgrandparent of a node, its parent's parent. We use assertions to make sure that 
  we don't attempt to access the ixgrandparent of a node that doesn't have one, such 
  as the root node or its children: 

  @param   n Pointer to object whose grand parent to get.
  @return  Pointer to grand parent object.

****************************************************************************************************
*/
eName *eNameSpace::ixgrandparent(
    eName *n) 
{
    osal_debug_assert(n != OS_NULL);

	/* Not the root node.
	 */
    osal_debug_assert(n->m_iup != OS_NULL); 

	/* Not child of root.
	 */
    osal_debug_assert(n->m_iup->m_iup != OS_NULL); 
    return n->m_iup->m_iup;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Get ixsibling.

  The eNameSpace::ixsibling() function returns pointer to object's ixsibling in red/black tree.
  The ixsibling of a node, defined as the other child of its parent. Note that the ixsibling may 
  be OS_NULL, if the parent has only one child. 

  @param   n Pointer to object whose ixsibling to get.
  @return  Pointer to ixsibling object in red/black tree, or OS_NULL if object has no ixsibling.

****************************************************************************************************
*/
eName *eNameSpace::ixsibling(
    eName *n) 
{
    osal_debug_assert(n != OS_NULL);

	/* Root node has no ixsibling.
	 */
    osal_debug_assert(n->m_iup != OS_NULL); 

    if (n == n->m_iup->m_ileft)
        return n->m_iup->m_iright;
    else
        return n->m_iup->m_ileft;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Get ixuncle.

  The eNameSpace::ixuncle() function returns pointer to object's ixuncle in red/black tree.
  The ixuncle of a node, defined as the ixsibling of its parent. The ixuncle may also be OS_NULL, 
  if the ixgrandparent has only one child. 

  @param   n Pointer to object whose ixuncle to get.
  @return  Pointer to ixsibling object in red/black tree, or OS_NULL if object has no ixuncle.

****************************************************************************************************
*/
eName *eNameSpace::ixuncle(
    eName *n) 
{
    osal_debug_assert(n != OS_NULL);

	/* Root node has no ixuncle.
	 */
    osal_debug_assert(n->m_iup != OS_NULL); 

	/* Children of root have no ixuncle.
	 */
    osal_debug_assert(n->m_iup->m_iup != OS_NULL); 

    return ixsibling(n->m_iup);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify tree integrity.

  The eNameSpace::ixverify_properties() function is for red/black tree implementation debugging only.
  The function checks integrity of the tree and calls osal_debug_assert() if an error is detected.

  We will at all times enforce the following five properties, which provide a theoretical 
  guarantee that the tree remains balanced. We will have a helper function ixverify_properties() 
  that asserts all five properties in a debug build, to help verify the correctness of our 
  implementation and formally demonstrate their meaning. Note that many of these tests walk 
  the tree, making them very expensive - for this reason we require the symbol EINDEX_DBTREE_DEBUG
  to be defined to turn them on.

  As shown, the tree terminates in NIL leaves, which we represent using OS_NULL (we set the 
  child pointers of their parents to OS_NULL). In an empty tree, the root pointer is OS_NULL. 
  This saves substantial space compared to explicit representation of leaves. 

  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixverify_properties() 
{
    ixverify_property_2();
    ixverify_property_4(m_ixroot);
    ixverify_property_5();
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that root node is black.

  The eNameSpace::verify_property_2() function verifies that the root node is black.
  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixverify_property_2() 
{
    osal_debug_assert(ixisblack(m_ixroot));
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that every red node has two black children.

  The eNameSpace::ixverify_property_4() function verifies that every red node has two children, 
  and both are black (or equivalently, the parent of every red node is black). 

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixverify_property_4(
    eName *n) 
{
    if (ixisred(n)) 
    {
        osal_debug_assert(ixisblack(n->m_ileft));
        osal_debug_assert(ixisblack(n->m_iright));
        osal_debug_assert(ixisblack(n->m_iup));
    }
    if (n == OS_NULL) return;
    ixverify_property_4(n->m_ileft);
    ixverify_property_4(n->m_iright);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that number of black nodes on match.

  The eNameSpace::ixverify_property_5() function verifies that all paths from any given node to 
  its leaf nodes contain the same number of black nodes. This one is the trickiest to verify; 
  we do it by traversing the tree, incrementing a black node count as we go. 
  The first time we reach a leaf we save the count. When we subsequently reach other leaves, 
  we compare the count to this saved count. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixverify_property_5() 
{
    int black_count_path = -1;
    ixverify_property_5_helper(m_ixroot, 0, &black_count_path);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Helper function for verify_property_5().

  The eNameSpace::ixverify_property_5_helper() function is called by eNameSpace::verify_property_5() 
  to do actual verifiction. See eNameSpace::verify_property_5() for comments.

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixverify_property_5_helper(
    eName *n, 
    int black_count, 
    int *path_black_count) 
{
    if (ixisblack(n)) 
	{
        black_count++;
    }
    if (n == OS_NULL) 
    {
        if (*path_black_count == -1) 
        {
            *path_black_count = black_count;
        } 
        else 
        {
            osal_debug_assert (black_count == *path_black_count);
        }
        return;
    }
    ixverify_property_5_helper(n->m_ileft,  black_count, path_black_count);
    ixverify_property_5_helper(n->m_iright, black_count, path_black_count);
}
#endif


/**
****************************************************************************************************

  @brief Red/Black tree: Rotate tree left.

  The eNameSpace::ixrotate_left() function rotates tree one step left at current node n.

  Both insertion and deletion rely on a fundamental operation for reducing tree height called 
  a rotation. A rotation locally changes the structure of the tree without changing the in-order 
  order of the sequence of values that it stores. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixrotate_left(
    eName *n) 
{
    eName
        *r;

    r = n->m_iright;
    ixreplace_node(n, r);
    n->m_iright = r->m_ileft;
    if (r->m_ileft) 
    {
        r->m_ileft->m_iup = n;
    }
    r->m_ileft = n;
    n->m_iup = r;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Rotate tree right.

  The eNameSpace::ixrotate_right() function rotates tree one step right at current node n.

  Both insertion and deletion rely on a fundamental operation for reducing tree height called 
  a rotation. A rotation locally changes the structure of the tree without changing the in-order 
  order of the sequence of values that it stores. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixrotate_right(
    eName *n) 
{
    eName
        *l;

    l = n->m_ileft;
    ixreplace_node(n, l);
    n->m_ileft = l->m_iright;
    if (l->m_iright)
    {
        l->m_iright->m_iup = n;
    }
    l->m_iright = n;
    n->m_iup = l;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Replace a node by another node.

  The eNameSpace::ixreplace_node() function replaces node "oldn" by node "newn".
  Here, ixreplace_node() is a helper function that cuts a node away from its parent, substituting 
  a new node (or OS_NULL) in its place. It simplifies consistent updating of parent and child 
  pointers. It needs the tree passed in because it may change which node is the root. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   oldn Node to be replaced.
  @param   newn Pointer to node to put in oldn's place. Can be OS_NULL.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixreplace_node(
    eName *oldn, 
    eName *newn) 
{
    if (oldn->m_iup) 
    {
        if (oldn == oldn->m_iup->m_ileft)
            oldn->m_iup->m_ileft = newn;

        else
            oldn->m_iup->m_iright = newn;
    }

	else
    {
        m_ixroot = newn;
    } 

    if (newn) 
    {
        newn->m_iup = oldn->m_iup;
    }
}


/**
****************************************************************************************************

  @brief Red/Black tree: Insert a node to red black tree.

  The eNameSpace::rbtree_insert() function inserts a node to red/black tree. Before calling this 
  function, make sure that node's m_oflags EVAR_IS_RED bit is set and m_ileft, m_iright and m_iup
  are all set to OS_NULL.

  When inserting a new node, we first insert it into the tree as we would into an ordinary 
  binary search tree. We find the place in the tree where the new node belongs, then attach 
  a new red node containing the value: 

  The problem is that the resulting tree may not satify our five red-black tree properties. 
  The call to ixinsert_case1() above begins the process of correcting the tree so that it 
  satifies the properties once more.

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   inserted_node Pointer to node to be inserted.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixrbtree_insert(
    eName *inserted_node) 
{
    if (m_ixroot == OS_NULL) 
    {
        m_ixroot = inserted_node;
    } 
    else 
    {
        eName *n = m_ixroot;
        while (1) 
        {
            if (inserted_node->compare(n) < 0) 
            {
                if (n->m_ileft == OS_NULL) 
                {
                    n->m_ileft = inserted_node;
                    break;
                }
                else 
                {
                    n = n->m_ileft;
                }
            } 
            else 
            {
                if (n->m_iright == OS_NULL) 
                {
                    n->m_iright = inserted_node;
                    break;
                } 
                else 
                {
                    n = n->m_iright;
                }
            }
        }
        inserted_node->m_iup = n;
    }
    ixinsert_case1(inserted_node);

#if EINDEX_DBTREE_DEBUG
    ixverify_properties();
#endif
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after inserting node.

  The eNameSpace::ixinsert_case1() function combines original ixinsert_case1, insert_case2 and
  insert_case3 functions.

  Case 1: In this case, the new node is now the root node of the tree. Since the root node must 
  be black, and changing its color adds the same number of black nodes to every path, we simply 
  recolor it black. Because only the root node has no parent, we can assume henceforth that the 
  node has a parent. 

  Case 2: In this case, the new node has a black parent. All the properties are still satisfied 
  and we return. 

  Case 3: In this case, the ixuncle node is red. We recolor the parent and ixuncle black and the 
  ixgrandparent red. However, the red ixgrandparent node may now violate the red-black tree 
  properties; we recursively invoke this procedure on it from case 1 to deal with this. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the inserted node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixinsert_case1(
    eName *n) 
{
    if (n->m_iup == OS_NULL)
    {
        n->ixsetblack();
    }
    else if (ixisred(n->m_iup))
    {
		if (ixisred(ixuncle(n))) 
		{
			n->m_iup->ixsetblack();
			ixuncle(n)->ixsetblack();
			ixgrandparent(n)->ixsetred();
			ixinsert_case1(ixgrandparent(n));
		} 
		else 
		{
			ixinsert_case4(n);
		}
	}
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after inserting node.

  The eNameSpace::ixinsert_case4() function combines original ixinsert_case4 and insert_case5 functions.

  Case 4: In this case, we deal with two cases that are mirror images of one another:
  - The new node is the right child of its parent and the parent is the left child of 
    the ixgrandparent. In this case we rotate left about the parent.
  - The new node is the left child of its parent and the parent is the right child of 
    the ixgrandparent. In this case we rotate right about the parent. 

  Neither of these fixes the properties, but they put the tree in the correct form to apply case 5. 

  Case 5: In this final case, we deal with two cases that are mirror images of one another:
  - The new node is the left child of its parent and the parent is the left child of the 
    ixgrandparent. In this case we rotate right about the ixgrandparent.
  -  The new node is the right child of its parent and the parent is the right child of 
  the ixgrandparent. In this case we rotate left about the ixgrandparent. 

  Now the properties are satisfied and all cases have been covered.   

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the inserted node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixinsert_case4(
    eName *n) 
{
    if (n == n->m_iup->m_iright && n->m_iup == ixgrandparent(n)->m_ileft) 
    {
        ixrotate_left(n->m_iup);
        n = n->m_ileft;
    } 
    else if (n == n->m_iup->m_ileft && n->m_iup == ixgrandparent(n)->m_iright) 
    {
        ixrotate_right(n->m_iup);
        n = n->m_iright;
    }

    n->m_iup->ixsetblack();
    ixgrandparent(n)->ixsetred();
    if (n == n->m_iup->m_ileft && n->m_iup == ixgrandparent(n)->m_ileft) 
    {
        ixrotate_right(ixgrandparent(n));
    } 
    else 
    {
#if EINDEX_DBTREE_DEBUG
        osal_debug_assert (n == n->m_iup->m_iright &&
            n->m_iup == ixgrandparent(n)->m_iright);
#endif
        ixrotate_left(ixgrandparent(n));
    }
}


/**
****************************************************************************************************

  @brief Red/Black tree: Remove node from red/black.

  The eNameSpace::rbtree_remove() function removes an object from red/black tree. The this pointer 
  points to the parent object in eobjects object hierarcy, and after this call the object n
  is no longer child of this object. After this call removed node's m_oflags EVAR_IS_RED bit 
  may have any value, as can m_ileft, m_iright and m_iup pointers.

  There are two cases for removal, depending on whether the node to be deleted has at most one, 
  or two non-leaf children. A node with at most one non-leaf child can simply be replaced with 
  its non-leaf child. When deleting a node with two non-leaf children, we copy the value from 
  the in-order predecessor (the maximum or rightmost element in the left subtree) into the node 
  to be deleted, and then we then delete the predecessor node, which has only one non-leaf child. 
  This same procedure also works in a red-black tree without affecting any properties. 

  @param   n Pointer to the node to remove.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixrbtree_remove(
    eName *n)
{
    eName
        *child,
        *pred;

    if (n->m_ileft != OS_NULL && n->m_iright != OS_NULL) 
    {
        /* Swap pred and n.
         */
		pred = n->m_ileft;
		while (pred->m_iright != OS_NULL) 
		{
			pred = pred->m_iright;
		} 

		if (n->m_iup)
		{
            if (n->m_iup->m_ileft == n) n->m_iup->m_ileft = pred;
            else n->m_iup->m_iright = pred;
		}
		else
		{
			m_ixroot = pred;
		}

		if (pred == n->m_ileft)
		{
			n->m_ileft = pred->m_ileft;
			pred->m_iup = n->m_iup;
			n->m_iup = pred;
			pred->m_ileft = n;
		}
		else
		{
			if (pred->m_iup->m_ileft == pred) pred->m_iup->m_ileft = n;
            else pred->m_iup->m_iright = n;
			
			child = n->m_iup; n->m_iup = pred->m_iup; pred->m_iup = child; 
			child = n->m_ileft; n->m_ileft = pred->m_ileft; pred->m_ileft = child; 
			pred->m_ileft->m_iup = pred;
		}

        pred->m_iright = n->m_iright; 
		n->m_iright = OS_NULL;

		if (n->m_ileft) n->m_ileft->m_iup = n;
		if (pred->m_iright) pred->m_iright->m_iup = pred;

        /* If red flags are different, swap flags.
         */
        if ((n->m_vflags ^ pred->m_vflags) & EVAR_IS_RED)
        {
            n->m_vflags ^= EVAR_IS_RED;
            pred->m_vflags ^= EVAR_IS_RED;
        }  
    }

#if EINDEX_DBTREE_DEBUG
    osal_debug_assert(n->m_ileft == OS_NULL || n->m_iright == OS_NULL);
#endif

    child = (n->m_iright == OS_NULL) ? n->m_ileft : n->m_iright;
    if (ixisblack(n)) 
    {
        if (ixisblack(child)) n->ixsetblack();
        else n->ixsetred();

		if (n->m_iup) ixdelete_case2(n);
    }
    ixreplace_node(n, child);

    if (n->m_iup == OS_NULL && child != OS_NULL)
        child->ixsetblack();

#if EINDEX_DBTREE_DEBUG
    ixverify_properties();
#endif
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eNameSpace::ixdelete_case2() function combines original ixdelete_case2 and delete_case3 functions.

  Case 2: N has a red ixsibling. In this case we exchange the colors of the parent and ixsibling, 
  then rotate about the parent so that the ixsibling becomes the parent of its former parent. 
  This does not restore the tree properties, but reduces the problem to one of the remaining cases. 

  Case 3: In this case N's parent, ixsibling, and ixsibling's children are black. In this case we 
  paint the ixsibling red. Now all paths passing through N's parent have one less black node 
  than before the deletion, so we must recursively run this procedure from case 1 on N's parent. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixdelete_case2(
    eName *n) 
{
    if (ixisred(ixsibling(n))) 
    {
        n->m_iup->ixsetred();
        ixsibling(n)->ixsetblack();

        if (n == n->m_iup->m_ileft)
            ixrotate_left(n->m_iup);
        else
            ixrotate_right(n->m_iup);
    }

    if (ixisblack(n->m_iup) &&
        ixisblack(ixsibling(n)) &&
        ixisblack(ixsibling(n)->m_ileft) &&
        ixisblack(ixsibling(n)->m_iright))
    {
        ixsibling(n)->ixsetred();
		if (n->m_iup->m_iup) ixdelete_case2(n->m_iup);
    }
    else
	{
        ixdelete_case4(n);
	}
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eNameSpace::ixdelete_case4()...

  Case 4: N's ixsibling and ixsibling's children are black, but its parent is red. We exchange 
  the colors of the ixsibling and parent; this restores the tree properties. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixdelete_case4(
    eName *n) 
{
    if (ixisred(n->m_iup) &&
        ixisblack(ixsibling(n)) &&
        ixisblack(ixsibling(n)->m_ileft) &&
        ixisblack(ixsibling(n)->m_iright))
    {
        ixsibling(n)->ixsetred();
        n->m_iup->ixsetblack();
    }
    else
        ixdelete_case5(n);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eNameSpace::ixdelete_case5()...

  Case 5: There are two cases handled here which are mirror images of one another:
  - N's ixsibling S is black, S's left child is red, S's right child is black, and N is the left 
    child of its parent. We exchange the colors of S and its left ixsibling and rotate right at S.
  - N's ixsibling S is black, S's right child is red, S's left child is black, and N is the right 
    child of its parent. We exchange the colors of S and its right ixsibling and rotate left at S. 

  Both of these function to reduce us to the situation described in case 6. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixdelete_case5(
    eName *n) 
{
    if (n == n->m_iup->m_ileft &&
        ixisblack(ixsibling(n)) &&
        ixisred(ixsibling(n)->m_ileft) &&
        ixisblack(ixsibling(n)->m_iright))
    {
        ixsibling(n)->ixsetred();
        ixsibling(n)->m_ileft->ixsetblack();
        ixrotate_right(ixsibling(n));
    }
    else if (n == n->m_iup->m_iright &&
        ixisblack(ixsibling(n)) &&
        ixisred(ixsibling(n)->m_iright) &&
        ixisblack(ixsibling(n)->m_ileft))
    {
        ixsibling(n)->ixsetred();
        ixsibling(n)->m_iright->ixsetblack();
        ixrotate_left(ixsibling(n));
    }
    ixdelete_case6(n);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eNameSpace::ixdelete_case6()...

  Case 6: There are two cases handled here which are mirror images of one another:
  - N's ixsibling S is black, S's right child is red, and N is the left child of its parent. 
    We exchange the colors of N's parent and ixsibling, make S's right child black, then 
	rotate left at N's parent.
  - N's ixsibling S is black, S's left child is red, and N is the right child of its parent. 
    We exchange the colors of N's parent and ixsibling, make S's left child black, then rotate 
	right at N's parent. 

  This accomplishes three things at once:
  - We add a black node to all paths through N, either by adding a black S to those paths 
    or by recoloring N's parent black.
  - We remove a black node from all paths through S's red child, either by removing P from 
    those paths or by recoloring S.
  - We recolor S's red child black, adding a black node back to all paths through S's red child. 

  S's left child has become a child of N's parent during the rotation and so is unaffected. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eNameSpace::ixdelete_case6(
    eName *n) 
{
    if (ixisblack(n->m_iup)) 
        ixsibling(n)->ixsetblack();
    else
        ixsibling(n)->ixsetred();

    n->m_iup->ixsetblack();
    if (n == n->m_iup->m_ileft) 
    {
#if EINDEX_DBTREE_DEBUG
        osal_debug_assert(ixisred(ixsibling(n)->m_iright));
#endif
        ixsibling(n)->m_iright->ixsetblack();
        ixrotate_left(n->m_iup);
    }
    else
    {
#if EINDEX_DBTREE_DEBUG
        osal_debug_assert(ixisred(ixsibling(n)->m_ileft));
#endif
        ixsibling(n)->m_ileft->ixsetblack();
        ixrotate_right(n->m_iup);
    }
}

