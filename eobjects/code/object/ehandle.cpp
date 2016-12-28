/**

  @file    eobject.cpp
  @brief   Object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The eHandle is base class for all objects. 
  
  - Functions to manage object hierarchy and idenfify objects.


  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

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

  @brief Get number of children.

  The eHandle::childcount() function returns pointer number of children. Argument oid specified
  wether to count attachment in or to count children only with specific id.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are counted.

  @return  Number of child objects.

****************************************************************************************************
*/
os_long eHandle::childcount(
    e_oid oid)
{
    os_long 
        count;

    eHandle
        *child;

    count = 0;

    for (child = first(oid);
         child;
         child = child->next(oid))
    {
        count++;
    }

    return count;
}


/**
****************************************************************************************************

  @brief Get first child object identified by oid.

  The eHandle::first() function returns pointer to the first child object selected by object 
  identifier oid given as argument. 

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to get a child object, 
		   which is not flagged as an attachment. Value EOID_ALL specifies to get pointer to
		   the first child object, regardless wether it is attachment or not. Other values
		   specify object identifier, only pointer to object with specified object
		   identifier is returned. 

  @return  Pointer to child object, or OS_NULL if no matching child object was found. 

****************************************************************************************************
*/
eHandle *eHandle::first(
    e_oid oid)
{
    eHandle 
        *n,
        *m;

    /* Set n to point root of child object's red/black tree.
     */
    n = m_children;

    /* Handle special object identifiers to get first child object
       of any id, with or without attachments.
     */
    if (oid == EOID_CHILD || oid == EOID_ALL) 
    {
		/* If no child objects, just return null.
		*/
		if (n == OS_NULL) return OS_NULL;
		
		/* Move to first child object.
         */
        while (n->m_left) n = n->m_left;

        /* If we are including attachments, or first object is
           not attachment, return pointer to the first object.
         */
        if (oid == EOID_ALL || !n->isattachment()) return n;
        
        /* Attachments not included and first object is attachment. 
           Call next to get first child object which is not 
           an attachment.
         */
        return n->next(oid);
    }
    
    /* Handle normal case where child object is searched by exactly
       matching object identifier.
     */
    while (n != OS_NULL) 
    {
        /* If object identifier matches, check if there
           is earlier match.
         */
        if (oid == n->m_oid) 
        {
            m = n->m_left;
            if (m == OS_NULL) break;
            if (m->m_oid != oid) break;
            n = m;
        } 

        /* Smaller, search to the left.
         */
        else if (oid < n->m_oid) 
        {
            n = n->m_left;
        } 

        /* Bigger, search to to right.
         */
        else 
        {
            n = n->m_right;
        }
    }

    /* Return object pointer or OS_NULL if none found.
     */
    return n;
}


/**
****************************************************************************************************

  @brief Get last child object identified by oid.

  The eHandle::last() function returns pointer to the last child object selected by object 
  identifier oid given as argument. 

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to get a child object, 
		   which is not flagged as an attachment. Value EOID_ALL specifies to get pointer to
		   the last child object, regardless wether it is attachment or not. Other values
		   specify object identifier, only pointer to object with specified object
		   identifier is returned. 

  @return  Pointer to child object, or OS_NULL if no matching child object was found. 

****************************************************************************************************
*/
eHandle *eHandle::last(
    e_oid oid)
{
    eHandle 
        *n,
        *m;

	/* Set n to point root of child object's red/black tree.
	*/
	n = m_children;

    /* Handle special object identifiers to get first child object
       of any id, with or without attachments.
     */
    if (oid == EOID_CHILD || oid == EOID_ALL) 
    {
		/* If no child objects, just return null.
		*/
		if (n == OS_NULL) return OS_NULL;
		
		/* Move to last child object.
         */
        while (n->m_right) n = n->m_right;

        /* If we are including attachments, or last object is
           not attachment, return pointer to the first object.
         */
        if (oid == EOID_ALL || !n->isattachment()) return n;

        /* Attachments not included and the last object is an attachment. 
           Call prev() to get previous child object which is not 
           an attachment.
         */
        return n->prev(oid);
    }

    /* Handle normal case where child object is searched by exactly
       matching object identifier.
     */
    while (n != OS_NULL) 
    {
        /* If object identifier matches, check if there
           is later match.
         */
        if (oid == n->m_oid) 
        {
            m = n->m_right;
            if (m == OS_NULL) break;
            if (m->m_oid != oid) break;
            n = m;
        } 

        /* Smaller, search to the left.
         */
        else if (oid < n->m_oid) 
        {
            n = n->m_left;
        } 

        /* Bigger, search to to right.
         */
        else 
        {
            n = n->m_right;
        }
    }

    /* Return object pointer or OS_NULL if none found.
     */
    return n;
}


/**
****************************************************************************************************

  @brief Get next child of same parent object.

  The eHandle::next() function returns pointer to the next child of the same parent object
  as this object (it could be called sibling). The object is selected by object identifier oid 
  given as argument.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to get a child object, 
		   which is not flagged as an attachment. Value EOID_ALL specifies to get pointer to
		   the next child object, regardless wether it is attachment or not. Other values
		   specify object identifier, only pointer to object with specified object
		   identifier is returned. 

  @return  Pointer to next object, or OS_NULL if no matching object was found. 

****************************************************************************************************
*/
eHandle *eHandle::next(
    e_oid oid)
{
    eHandle
        *n,
        *m;

    n = this;

try_again:
    if (n->m_right)
    {
        n = n->m_right;
        while (n->m_left) n = n->m_left;

        if (oid == EOID_ALL) return n;
        if (oid == EOID_CHILD) 
        {
            if (!n->isattachment()) return n;
            goto try_again;
        }
        return (n->m_oid == oid) ? n : OS_NULL;
    }
    else
    {
        m = n->m_up;

        while (OS_TRUE) 
        {
            if (m == OS_NULL) return OS_NULL;
            if (m->m_left == n) break;
            n = m;
            m = n->m_up;
        }

        if (oid == EOID_ALL) return m;
        if (oid == EOID_CHILD)
        {
            if (!m->isattachment()) return m;
            goto try_again;
        }
        
        return (m->m_oid == oid) ? m : OS_NULL;
    }
}


/**
****************************************************************************************************

  @brief Get previous child of same parent object.

  The eHandle::prev() function returns pointer to the previous child of the same parent object
  as this object (it could be called sibling). The object is selected by object identifier oid 
  given as argument.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to get a child object, 
		   which is not flagged as an attachment. Value EOID_ALL specifies to get pointer to
		   the previous child object, regardless wether it is attachment or not. Other values
		   specify object identifier, only pointer to object with specified object
		   identifier is returned. 

  @return  Pointer to previous object, or OS_NULL if no matching object was found. 

****************************************************************************************************
*/
eHandle *eHandle::prev(
    e_oid oid)
{
    eHandle
        *n,
        *m;

    n = this;

try_again:
    if (n->m_left)
    {
        n = n->m_left;
        while (n->m_right) n = n->m_right;

        if (oid == EOID_ALL) return n;
        if (oid == EOID_CHILD) 
        {
            if (!n->isattachment()) return n;
            goto try_again;
        }
        return (n->m_oid == oid) ? n : OS_NULL;
    }
    else
    {
        m = n->m_up;

        while (OS_TRUE) 
        {
            if (m == OS_NULL) return OS_NULL;
            if (m->m_right == n) break;
            n = m;
            m = n->m_up;
        }

        if (oid == EOID_ALL) return m;
        if (oid == EOID_CHILD)
        {
            if (!m->isattachment()) return m;
            goto try_again;
        }
        
        return (m->m_oid == oid) ? m : OS_NULL;
    }
}




/**
****************************************************************************************************

  @brief Delete all child objects.

  The eHandle::delete_children() function deletes all children of this object. This is faster code
  to delete all children, balancing of the red/black tree is not maintained while deleting.

  @return  None.

****************************************************************************************************
*/
void eHandle::delete_children()
{
	eHandle
		*n,
		*p;

	n = m_children;
	if (n == OS_NULL) return;

	while (OS_TRUE)
	{
		while (OS_TRUE)
		{
			p = n->m_left;
			if (p == OS_NULL) 
			{
				p = n->m_right;
				if (p == OS_NULL) break;
			}
			n = p;
		}

		p = n->m_up;

		n->m_oflags |= EOBJ_FAST_DELETE;
		delete n->m_object;

		if (p) 
		{
			if (p->m_left == n) p->m_left = OS_NULL;
			else p->m_right = OS_NULL;
		}
		else
		{
			break;
		}

		n = p;
	}
}


#if EOBJECT_DBTREE_DEBUG
/**
****************************************************************************************************

  @brief Red/Black tree: Get grandparent.

  The eHandle::rb_grandparent() function returns pointer to object's grandparent in red/black tree.
  The grandparent of a node, its parent's parent. We use assertions to make sure that 
  we don't attempt to access the grandparent of a node that doesn't have one, such 
  as the root node or its children: 

  @param   n Pointer to object whose grand parent to get.
  @return  Pointer to grand parent object.

****************************************************************************************************
*/
eHandle *eHandle::rb_grandparent(
    eHandle *n) 
{
    edebug_assert(n != OS_NULL);

	/* Not the root node.
	 */
    edebug_assert(n->m_up != OS_NULL); 

	/* Not child of root.
	 */
    edebug_assert(n->m_up->m_up != OS_NULL); 
    return n->m_up->m_up;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Get sibling.

  The eHandle::sibling() function returns pointer to object's sibling in red/black tree.
  The sibling of a node, defined as the other child of its parent. Note that the sibling may 
  be OS_NULL, if the parent has only one child. 

  @param   n Pointer to object whose sibling to get.
  @return  Pointer to sibling object in red/black tree, or OS_NULL if object has no sibling.

****************************************************************************************************
*/
eHandle *eHandle::sibling(
    eHandle *n) 
{
    edebug_assert(n != OS_NULL);

	/* Root node has no sibling.
	 */
    edebug_assert(n->m_up != OS_NULL); 

    if (n == n->m_up->m_left)
        return n->m_up->m_right;
    else
        return n->m_up->m_left;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Get uncle.

  The eHandle::uncle() function returns pointer to object's uncle in red/black tree.
  The uncle of a node, defined as the sibling of its parent. The uncle may also be OS_NULL, 
  if the grandparent has only one child. 

  @param   n Pointer to object whose uncle to get.
  @return  Pointer to sibling object in red/black tree, or OS_NULL if object has no uncle.

****************************************************************************************************
*/
eHandle *eHandle::uncle(
    eHandle *n) 
{
    edebug_assert(n != OS_NULL);

	/* Root node has no uncle.
	 */
    edebug_assert(n->m_up != OS_NULL); 

	/* Children of root have no uncle.
	 */
    edebug_assert(n->m_up->m_up != OS_NULL); 

    return sibling(n->m_up);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify tree integrity.

  The eHandle::verify_properties() function is for red/black tree implementation debugging only.
  The function checks integrity of the tree and calls edebug_assert() if an error is detected.

  We will at all times enforce the following five properties, which provide a theoretical 
  guarantee that the tree remains balanced. We will have a helper function verify_properties() 
  that asserts all five properties in a debug build, to help verify the correctness of our 
  implementation and formally demonstrate their meaning. Note that many of these tests walk 
  the tree, making them very expensive - for this reason we require the symbol EOBJECT_DBTREE_DEBUG
  to be defined to turn them on.

  As shown, the tree terminates in NIL leaves, which we represent using OS_NULL (we set the 
  child pointers of their parents to OS_NULL). In an empty tree, the root pointer is OS_NULL. 
  This saves substantial space compared to explicit representation of leaves. 

  @return  None.

****************************************************************************************************
*/
void eHandle::verify_properties() 
{
    verify_property_2();
    verify_property_4(m_children);
    verify_property_5();
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that root node is black.

  The eHandle::verify_property_2() function verifies that the root node is black.
  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eHandle::verify_property_2() 
{
    edebug_assert(isblack(m_children));
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that every red node has two black children.

  The eHandle::verify_property_4() function verifies that every red node has two children, 
  and both are black (or equivalently, the parent of every red node is black). 

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eHandle::verify_property_4(
    eHandle *n) 
{
    if (isred(n)) 
    {
        edebug_assert(isblack(n->m_left));
        edebug_assert(isblack(n->m_right));
        edebug_assert(isblack(n->m_up));
    }
    if (n == OS_NULL) return;
    verify_property_4(n->m_left);
    verify_property_4(n->m_right);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Verify that number of black nodes on match.

  The eHandle::verify_property_5() function verifies that all paths from any given node to 
  its leaf nodes contain the same number of black nodes. This one is the trickiest to verify; 
  we do it by traversing the tree, incrementing a black node count as we go. 
  The first time we reach a leaf we save the count. When we subsequently reach other leaves, 
  we compare the count to this saved count. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eHandle::verify_property_5() 
{
    int black_count_path = -1;
    verify_property_5_helper(m_children, 0, &black_count_path);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Helper function for verify_property_5().

  The eHandle::verify_property_5_helper() function is called by eHandle::verify_property_5() 
  to do actual verifiction. See eHandle::verify_property_5() for comments.

  @param   n Pointer to current node.
  @return  None.

****************************************************************************************************
*/
void eHandle::verify_property_5_helper(
    eHandle *n, 
    int black_count, 
    int *path_black_count) 
{
    if (isblack(n)) 
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
            edebug_assert (black_count == *path_black_count);
        }
        return;
    }
    verify_property_5_helper(n->m_left,  black_count, path_black_count);
    verify_property_5_helper(n->m_right, black_count, path_black_count);
}
#endif


/**
****************************************************************************************************

  @brief Red/Black tree: Rotate tree left.

  The eHandle::rotate_left() function rotates tree one step left at current node (this).

  Both insertion and deletion rely on a fundamental operation for reducing tree height called 
  a rotation. A rotation locally changes the structure of the tree without changing the in-order 
  order of the sequence of values that it stores. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eHandle::rotate_left(
	eHandle *n)
{
    eHandle 
        *r;

	r = n->m_right;
	replace_node(n, r);
	n->m_right = r->m_left;
	if (r->m_left)
	{
		r->m_left->m_up = n;
	}
	r->m_left = n;
	n->m_up = r;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Rotate tree right.

  The eHandle::rotate_right() function rotates tree one step right at current node (this).

  Both insertion and deletion rely on a fundamental operation for reducing tree height called 
  a rotation. A rotation locally changes the structure of the tree without changing the in-order 
  order of the sequence of values that it stores. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @return  None.

****************************************************************************************************
*/
void eHandle::rotate_right(
	eHandle *n)
{
    eHandle 
        *l;

	l = n->m_left;
	replace_node(n, l);
	n->m_left = l->m_right;
	if (l->m_right)
	{
		l->m_right->m_up = n;
	}
	l->m_right = n;
	n->m_up = l;
}


/**
****************************************************************************************************

  @brief Red/Black tree: Replace a node by another node.

  The eHandle::replace_node() function replaces this node by node "newn".
  Here, replace_node() is a helper function that cuts a node away from its parent, substituting 
  a new node (or OS_NULL) in its place. It simplifies consistent updating of parent and child 
  pointers. It needs the tree passed in because it may change which node is the root. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   oldn Node to be replaced.
  @param   newn Pointer to node to put in oldn's place. Can be OS_NULL.
  @return  None.

****************************************************************************************************
*/
void eHandle::replace_node(
	eHandle *oldn,
    eHandle *newn) 
{
	if (oldn->m_up)
	{
		if (oldn == oldn->m_up->m_left)
			oldn->m_up->m_left = newn;

		else
			oldn->m_up->m_right = newn;
	}

	else
	{
		m_children = newn;
	}

	if (newn)
	{
		newn->m_up = oldn->m_up;
	}
}


/**
****************************************************************************************************

  @brief Red/Black tree: Insert a node to red black tree.

  The eHandle::rbtree_insert() function inserts a node to red/black tree. Before calling this 
  function, make sure that node's m_oflags EOBJ_IS_RED bit is set and m_left, m_right and m_up
  are all set to OS_NULL.

  When inserting a new node, we first insert it into the tree as we would into an ordinary 
  binary search tree. We find the place in the tree where the new node belongs, then attach 
  a new red node containing the value: 

  The problem is that the resulting tree may not satify our five red-black tree properties. 
  The call to insert_case1() above begins the process of correcting the tree so that it 
  satifies the properties once more.

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   inserted_node Pointer to node to be inserted.
  @return  None.

****************************************************************************************************
*/
void eHandle::rbtree_insert(
    eHandle *inserted_node) 
{
	eHandle 
			*n;

    if (m_children == OS_NULL)
    {
		m_children = inserted_node;
    } 
    else 
    {
        n = m_children;
        while (1) 
        {
            if (inserted_node->m_oid < n->m_oid) 
            {
                if (n->m_left == OS_NULL) 
                {
                    n->m_left = inserted_node;
                    break;
                }
                else 
                {
                    n = n->m_left;
                }
            } 
            else 
            {
                if (n->m_right == OS_NULL) 
                {
                    n->m_right = inserted_node;
                    break;
                } 
                else 
                {
                    n = n->m_right;
                }
            }
        }
        inserted_node->m_up = n;
    }
    insert_case1(inserted_node);

#if EOBJECT_DBTREE_DEBUG
    verify_properties();
#endif
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after inserting node.

  The eHandle::insert_case1() function combines original insert_case1, insert_case2 and
  insert_case3 functions.

  Case 1: In this case, the new node is now the root node of the tree. Since the root node must 
  be black, and changing its color adds the same number of black nodes to every path, we simply 
  recolor it black. Because only the root node has no parent, we can assume henceforth that the 
  node has a parent. 

  Case 2: In this case, the new node has a black parent. All the properties are still satisfied 
  and we return. 

  Case 3: In this case, the uncle node is red. We recolor the parent and uncle black and the 
  grandparent red. However, the red grandparent node may now violate the red-black tree 
  properties; we recursively invoke this procedure on it from case 1 to deal with this. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the inserted node.
  @return  None.

****************************************************************************************************
*/
void eHandle::insert_case1(
    eHandle *n) 
{
    if (n->m_up == OS_NULL)
    {
        n->setblack();
    }
    else if (isred(n->m_up))
    {
		if (isred(uncle(n))) 
		{
			n->m_up->setblack();
			uncle(n)->setblack();
			rb_grandparent(n)->setred();
			insert_case1(rb_grandparent(n));
		} 
		else 
		{
			insert_case4(n);
		}
	}
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after inserting node.

  The eHandle::insert_case4() function combines original insert_case4 and insert_case5 functions.

  Case 4: In this case, we deal with two cases that are mirror images of one another:
  - The new node is the right child of its parent and the parent is the left child of 
    the grandparent. In this case we rotate left about the parent.
  - The new node is the left child of its parent and the parent is the right child of 
    the grandparent. In this case we rotate right about the parent. 

  Neither of these fixes the properties, but they put the tree in the correct form to apply case 5. 

  Case 5: In this final case, we deal with two cases that are mirror images of one another:
  - The new node is the left child of its parent and the parent is the left child of the 
    grandparent. In this case we rotate right about the grandparent.
  - The new node is the right child of its parent and the parent is the right child of 
    the grandparent. In this case we rotate left about the grandparent. 

  Now the properties are satisfied and all cases have been covered.

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the inserted node.
  @return  None.

****************************************************************************************************
*/
void eHandle::insert_case4(
    eHandle *n) 
{
    if (n == n->m_up->m_right && n->m_up == rb_grandparent(n)->m_left) 
    {
        rotate_left(n->m_up);
        n = n->m_left;
    } 
    else if (n == n->m_up->m_left && n->m_up == rb_grandparent(n)->m_right) 
    {
        rotate_right(n->m_up);
        n = n->m_right;
    }

    n->m_up->setblack();
    rb_grandparent(n)->setred();
    if (n == n->m_up->m_left && n->m_up == rb_grandparent(n)->m_left) 
    {
        rotate_right(rb_grandparent(n));
    } 
    else 
    {
#if EOBJECT_DBTREE_DEBUG
        edebug_assert (n == n->m_up->m_right &&
            n->m_up == rb_grandparent(n)->m_right);
#endif
		rotate_left(rb_grandparent(n));
    }
}


/**
****************************************************************************************************

  @brief Red/Black tree: Remove node from red/black.

  The eHandle::rbtree_remove() function removes an object from red/black tree. The this pointer 
  points to the parent object in eobjects object hierarcy, and after this call the object n
  is no longer child of this object. After this call removed node's m_oflags EOBJ_IS_RED bit 
  may have any value, as can m_left, m_right and m_up pointers.

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
void eHandle::rbtree_remove(
	eHandle *n)
{
    eHandle 
        *child,
        *pred;

	if (n->m_left != OS_NULL && n->m_right != OS_NULL)
	{
		/* Swap pred and n.
		*/
		pred = n->m_left;
		while (pred->m_right != OS_NULL)
		{
			pred = pred->m_right;
		}

		if (n->m_up)
		{
			if (n->m_up->m_left == n) n->m_up->m_left = pred;
			else n->m_up->m_right = pred;
		}
		else
		{
			m_children = pred;
		}

		if (pred == n->m_left)
		{
			n->m_left = pred->m_left;
			pred->m_up = n->m_up;
			n->m_up = pred;
			pred->m_left = n;
		}
		else
		{
			if (pred->m_up->m_left == pred) pred->m_up->m_left = n;
			else pred->m_up->m_right = n;

			child = n->m_up; n->m_up = pred->m_up; pred->m_up = child;
			child = n->m_left; n->m_left = pred->m_left; pred->m_left = child;
			pred->m_left->m_up = pred;
		}

		pred->m_right = n->m_right;
		n->m_right = OS_NULL;

		if (n->m_left) n->m_left->m_up = n;
		if (pred->m_right) pred->m_right->m_up = pred;

		/* If red flags are different, swap flags.
		*/
		if ((n->m_oflags ^ pred->m_oflags) & EOBJ_IS_RED)
		{
			n->m_oflags ^= EOBJ_IS_RED;
			pred->m_oflags ^= EOBJ_IS_RED;
		}
	}

#if EOBJECT_DBTREE_DEBUG
	edebug_assert(n->m_left == OS_NULL || n->m_right == OS_NULL);
#endif

	child = (n->m_right == OS_NULL) ? n->m_left : n->m_right;
	if (isblack(n))
	{
		if (isblack(child)) n->setblack();
		else n->setred();

		if (n->m_up) delete_case2(n);
	}
	replace_node(n, child);

	if (n->m_up == OS_NULL && child != OS_NULL)
		child->setblack();

#if EOBJECT_DBTREE_DEBUG
	verify_properties();
#endif
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eHandle::delete_case2() function combines original delete_case2 and delete_case3 functions.

  Case 2: N has a red sibling. In this case we exchange the colors of the parent and sibling, 
  then rotate about the parent so that the sibling becomes the parent of its former parent. 
  This does not restore the tree properties, but reduces the problem to one of the remaining cases. 

  Case 3: In this case N's parent, sibling, and sibling's children are black. In this case we 
  paint the sibling red. Now all paths passing through N's parent have one less black node 
  than before the deletion, so we must recursively run this procedure from case 1 on N's parent. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eHandle::delete_case2(
	eHandle *n)
{
	if (isred(sibling(n)))
	{
		n->m_up->setred();
		sibling(n)->setblack();

		if (n == n->m_up->m_left)
			rotate_left(n->m_up);
		else
			rotate_right(n->m_up);
	}

	if (isblack(n->m_up) &&
		isblack(sibling(n)) &&
		isblack(sibling(n)->m_left) &&
		isblack(sibling(n)->m_right))
	{
		sibling(n)->setred();
		if (n->m_up->m_up) delete_case2(n->m_up);
	}
	else
	{
		delete_case4(n);
	}
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eHandle::delete_case4()...

  Case 4: N's sibling and sibling's children are black, but its parent is red. We exchange 
  the colors of the sibling and parent; this restores the tree properties. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eHandle::delete_case4(
	eHandle *n)
{
	if (isred(n->m_up) &&
		isblack(sibling(n)) &&
		isblack(sibling(n)->m_left) &&
		isblack(sibling(n)->m_right))
	{
		sibling(n)->setred();
		n->m_up->setblack();
	}
	else
		delete_case5(n);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eHandle::delete_case5()...

  Case 5: There are two cases handled here which are mirror images of one another:
  - N's sibling S is black, S's left child is red, S's right child is black, and N is the left 
    child of its parent. We exchange the colors of S and its left sibling and rotate right at S.
  - N's sibling S is black, S's right child is red, S's left child is black, and N is the right 
    child of its parent. We exchange the colors of S and its right sibling and rotate left at S. 

  Both of these function to reduce us to the situation described in case 6. 

  The this pointer points to the parent object in eobjects object hierarcy. The red/black tree
  manipulated here is tree of child objects.

  @param   n Pointer to the removed node.
  @return  None.

****************************************************************************************************
*/
void eHandle::delete_case5(
	eHandle *n)
{
	if (n == n->m_up->m_left &&
		isblack(sibling(n)) &&
		isred(sibling(n)->m_left) &&
		isblack(sibling(n)->m_right))
	{
		sibling(n)->setred();
		sibling(n)->m_left->setblack();
		rotate_right(sibling(n));
	}
	else if (n == n->m_up->m_right &&
		isblack(sibling(n)) &&
		isred(sibling(n)->m_right) &&
		isblack(sibling(n)->m_left))
	{
		sibling(n)->setred();
		sibling(n)->m_right->setblack();
		rotate_left(sibling(n));
	}
	delete_case6(n);
}


/**
****************************************************************************************************

  @brief Red/Black tree: Balance red/black tree after removing node.

  The eHandle::delete_case6()...

  Case 6: There are two cases handled here which are mirror images of one another:
  - N's sibling S is black, S's right child is red, and N is the left child of its parent. 
    We exchange the colors of N's parent and sibling, make S's right child black, then 
	rotate left at N's parent.
  - N's sibling S is black, S's left child is red, and N is the right child of its parent. 
    We exchange the colors of N's parent and sibling, make S's left child black, then rotate 
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
void eHandle::delete_case6(
	eHandle *n)
{
	if (isblack(n->m_up))
		sibling(n)->setblack();
	else
		sibling(n)->setred();

	n->m_up->setblack();
	if (n == n->m_up->m_left)
	{
#if EOBJECT_DBTREE_DEBUG
		edebug_assert(isred(sibling(n)->m_right));
#endif
		sibling(n)->m_right->setblack();
		rotate_left(n->m_up);
	}
	else
	{
#if EOBJECT_DBTREE_DEBUG
		edebug_assert(isred(sibling(n)->m_left));
#endif
		sibling(n)->m_left->setblack();
		rotate_right(n->m_up);
	}
}


/**
****************************************************************************************************

  @brief Write object to stream.

  The eHandle::write() function writes object with class information, attachments, etc to
  the stream.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
#if 0
eStatus eHandle::write(
    eStream *stream, 
    os_int flags) 
{
    eHandle 
        *child;

    os_long
        n_attachements;

    /* Write class identifier, object identifier and persistant object flags.
     */
    if (*stream << classid()) goto failed;
    if (*stream << oid()) goto failed;
    if (*stream << flags() & (EOBJ_SERIALIZATION_MASK)) goto failed;

    /* Calculate and write number of attachments.
     */
    n_attachements = 0;
    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isserattachment()) n_attachements++;
    }
    if (*stream << n_attachements) goto failed;
    
    /* Write the object content.
     */
    if (writer(stream, flags)) goto failed;

    /* Write attachments.
     */
    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isserattachment()) 
        {
            if (child->write(stream, flags)) goto failed;
        }
    }
    
    /* Object succesfully written.
     */
    return ESTATUS_SUCCESS;

    /* Writing object failed.
     */
failed:
    return ESTATUS_WRITING_OBJ_FAILED;
}
#endif

/**
****************************************************************************************************

  @brief Read object from stream.

  The eHandle::read() function reads class information, etc from the stream, creates new 
  child object and reads child object content and attachments.
  
  @param  stream The stream to write to.
  @param  flags Serialization flags.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
#if 0
eHandle *eHandle::read(
    eStream *stream, 
    os_int flags)
{
    os_int
        cid,
        oid,
        oflags;

    os_long
        n_attachements,
        i;

    eHandle
        *child;

    /* Read class identifier, object identifier, persistant object flags
       and number of attachments.
     */
    if (*stream >> cid) goto failed;
    if (*stream >> oid) goto failed;
    if (*stream >> oflags) goto failed;
    if (*stream >> n_attachements) goto failed;

    /* Generate new object.
     */
    child = newchild(cid, oid);
    if (child == OS_NULL) goto failed;

    /* Set flags.
     */
    child->setflags(oflags);
    
    /* Read the object content.
     */
    if (child->reader(stream, flags)) goto failed;

    /* Read attachments.
     */
    for (i = 0; i<n_attachements; i++)
    {
        if (read(stream, flags) == OS_NULL) goto failed;
    }

    /* Object succesfully read, return pointer to it.
     */
    return child;

    /* Reading object failed.
     */
failed:
    return OS_NULL;
}
#endif
