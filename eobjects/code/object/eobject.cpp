/**

  @file    eobject.cpp
  @brief   Object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The eObject is base class for all objects. 
  
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

  @brief Object constructor.

  The eObject constructor creates an empty object. This constructor is called when object of
  any eobject library is constructed.

  @param   parent Pointer to parent object, or OS_NULL if the object is an orphan. If parent
           object is specified, the object will be deleted when parent is deleted.
  @param   oid Object identifier for new object. If not specified, defauls to EOID_ITEM (-1), 
		   which is generic list item. 
  
  @param   flags 
		   - EOID_PRIMITIVE object is stand primitiva stand alone object, which cannot be 
		     part of object tree. This is typically used for eVariable allocated from stack
			 or as member of a C++ class.

  @return  None.

****************************************************************************************************
*/
eObject::eObject(
    eObject *parent,
    e_oid oid,
	os_int flags)
{
	/* If this if not primitive object? 
	 */
	if (oid != EOID_ITEM || parent != OS_NULL)
	{
		/* No parent, allocate root object?
		 */
		if (parent == OS_NULL)
		{
			/* Allocate root helper object. 
			 */
			mm_root = new eRoot(this, EOID_ROOT_HELPER,
				EOBJ_IS_ATTACHMENT | EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE);

			/* Allocate handle for this object
			 */
			mm_root->newhandle(this, OS_NULL, oid, flags);

			/* Allocate handle for the root helper object.
			*/
			mm_root->newhandle(mm_root, this, EOID_ROOT_HELPER, 
				EOBJ_IS_ATTACHMENT | EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE);
		}

		/* Otherwise if root object constructor?
		 */
		else if (oid == EOID_ROOT_HELPER)
		{
            /* Here we cannot use type checked cast, because object is not fully initialized.
             */
			mm_root = (eRoot*)this;
    		mm_handle = OS_NULL;
		}

		/* Otherwise normal child object.  Copy parent's root object pointer
		   and allocate handle for the new child object object.
		*/
		else
		{
			mm_root = parent->mm_root;
			mm_root->newhandle(this, parent, oid, flags);

// verify_whole_tree();
		}
	}

	/* This is primitive object (typically eVariable).
	 */
	else
	{
		mm_root = OS_NULL;
		mm_handle = OS_NULL;
	}
}


/**
****************************************************************************************************

  @brief Object destructor.

  The eObject destructor releases all child objects and removes object from parent's child
  structure,

  @return  None.

****************************************************************************************************
*/
eObject::~eObject()
{
    /* if (m_parent)
    {
        m_parent->onchilddetach();
	} */

    /* Delete child objects.
     */
	if (mm_handle) if (mm_handle->m_parent) 
    {
        mm_handle->delete_children();
    }

	/* This would be more straight forward code to delete children instead of delete_children(),
	   but quite a bit slower.

    while (m_children)
    {
        delete m_children; or delete first();
    }
	*/

    if ((flags() & EOBJ_FAST_DELETE) == 0)
    {
		if (mm_handle) 
        {
            /* If handle has parent, remove from parent's children.
             */
            if (mm_handle->m_parent) 
            {
			    mm_handle->m_parent->rbtree_remove(mm_handle);
            }
            
            /* Handle no longer needed.
             */
            mm_root->freehandle(mm_handle);
        }
    }
}


/**
****************************************************************************************************

  @brief Clone object

  The eObject::clone function is base class only. Cloning eObject is not supported.

****************************************************************************************************
*/
eObject *eObject::clone(
    eObject *parent, 
    e_oid oid)
{
	osal_debug_error("clone() not supported for the class");
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Allocate new child object of any listed class.

  The eObject::newchild function looks from global class list by class identifier. If static 
  constructor member function corresponding to classid given as argument is found, then an
  object of that class is created as child object if this object.

  @param   cid Class identifier, specifies what kind of object to create.
  @param   oid Object identifier for the new object.
  @return  Pointer to newly created child object, or OS_NULL if none was found.

****************************************************************************************************
*/
eObject *eObject::newchild(
    os_int cid,
    e_oid oid,
	os_int flags) 
{
    eNewObjFunc 
        func;

    /* Look for static constructor by class identifier. If not found, return OS_NULL.
     */
    func = eclasslist_get_func(cid);
    if (func == OS_NULL) return OS_NULL;

    /* Create new object of the class.
     */
    return func(this, oid, flags);
}


#if EOVERLOAD_NEW_AND_DELETE
/**
****************************************************************************************************

  @brief Overloaded new operator.

  The new operator maps object memory allocation to OSAL function osal_memory_allocate().

  @param   size Number of bytes to allocate.
  @return  Pointer to allocated memory block.

****************************************************************************************************
*/
void *eObject::operator new(
	size_t size)
{
	os_char 
		*buf;
		
	size += sizeof(os_memsz);
	buf = (os_char*)osal_memory_allocate((os_memsz)size, OS_NULL);

	*(os_memsz*)buf = (os_memsz)size;

	return buf + sizeof(os_memsz);
}
#endif

#if EOVERLOAD_NEW_AND_DELETE
/**
****************************************************************************************************

  @brief Overloaded delete operator.

  The delete operator maps freeing object memory to OSAL function osal_memory_free().

  @param   buf Pointer to memory block to free.
  @return  None.

****************************************************************************************************
*/
void eObject::operator delete(
	void *buf)
{
	if (buf)
	{
		buf = (os_char*)buf - sizeof(os_memsz);
		osal_memory_free(buf, *(os_memsz*)buf);
	}
}
#endif


/**
****************************************************************************************************

  @brief Get number of children.

  The eObject::childcount() function returns pointer number of children. Argument oid specified
  wether to count attachment in or to count children only with specific id.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are counted.

  @return  Number of child objects.

****************************************************************************************************
*/
	/* os_long childcount(
		e_oid oid = EOID_CHILD)
	{
		if (mm_handle) return mm_handle->childcount(oid);
		return 0;
	} */


/**
****************************************************************************************************

  @brief Get thread object.

  The eObject::thread() function returns pointer to eThread object to which this object belongs
  to. If this object doesn'is not part of thread's tree structure, the function returns OS_NULL.

  @return  Pointer to eThread, or OS_NULL if none.

****************************************************************************************************
*/
eThread *eObject::thread() 
{
	if (mm_root) 
    {
        eObject *o = mm_root->parent();
        if (o->classid() == ECLASSID_THREAD) return eThread::cast(o);
    }
	return OS_NULL;
}

/**
****************************************************************************************************

  @brief Get first child object identified by oid.

  The eObject::first() function returns pointer to the first child object of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child object, or OS_NULL if none found.

****************************************************************************************************
*/
eObject *eObject::first(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->first(oid);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Get the first child variable identified by oid.

  The eObject::firstv() function returns pointer to the first child variable of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child variable. Or OS_NULL if none found.

****************************************************************************************************
*/
eVariable *eObject::firstv(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->first(oid);
    while (h)
    {
        if (h->m_object->classid() == ECLASSID_VARIABLE) 
            return eVariable::cast(h->m_object);

        h = h->next(oid);
    }
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Get the first child container identified by oid.

  The eObject::firstc() function returns pointer to the first child container of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child container. Or OS_NULL if none found.

****************************************************************************************************
*/
eContainer *eObject::firstc(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->first(oid);
    while (h)
    {
        if (h->object()->classid() == ECLASSID_CONTAINER) 
            return eContainer::cast(h->m_object);

        h = h->next(oid);
    }
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Get the first child name identified by oid.

  The eObject::firstn() function returns pointer to the first child name of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child name. Or OS_NULL if none found.

****************************************************************************************************
*/
eName *eObject::firstn(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->first(oid);
    while (h)
    {
        if (h->object()->classid() == ECLASSID_NAME) 
            return eName::cast(h->m_object);

        h = h->next(oid);
    }
    return OS_NULL;
}


/* Get last child object identified by oid.
*/
eObject *eObject::last(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->last(oid);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Get next child object identified by oid.

  The eObject::next() function returns pointer to the next child object of this object.

  @param   oid Object idenfifier. Default value EOID_CHILD specifies to count a child objects, 
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child object, or OS_NULL if none found.

****************************************************************************************************
*/
eObject *eObject::next(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->next(oid);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/* Get previous object identified by oid.
*/
eObject *eObject::prev(
	e_oid oid)
{
	if (mm_handle == OS_NULL) return OS_NULL;
	eHandle *h = mm_handle->prev(oid);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Adopt obeject as child.

  The eObject::adopt() function moves on object from it's position in tree structure to
  an another. 
  
  @param   aflags EOBJ_BEFORE_THIS Adopt before this object. EOBJ_NO_MAP not to map names.
  @param   oid EOID_CHILD object identifier unchanged.
  @return  None.

****************************************************************************************************
*/
void eObject::adopt(
    eObject *child, 
    e_oid oid,
    os_int aflags)
{
    os_boolean
        sync;

    eHandle
        *childh;

    os_int
        mapflags;

    /* Make sure that parent object is already part of tree structure.
     */
	if (mm_handle == OS_NULL) 
    {
        osal_debug_error("adopt(): parent object is not part of tree.");
		return;
    }
    
    if (child->mm_handle == OS_NULL)
    {
        sync = OS_FALSE; // || m_root->is_process ???????????????????????????????????????????????????????????????????????
        if (sync) osal_mutex_system_lock();

        mm_root->newhandle(child, this, oid, 0);

        child->mm_root = mm_root;

        if (sync) osal_mutex_system_unlock();
    }

    else
    {
// child->mm_handle->verify_whole_tree();
// mm_handle->verify_whole_tree();

        // Detach names

        /* Synchronize if adopting from three structure to another.
         */
        sync = (mm_root != child->mm_root);

        childh = child->mm_handle;

        if (sync) 
        {
            osal_mutex_system_lock();
        }

        /* Detach names of child object and it's childen from name spaces 
           above this object in tree structure.
         */
        child->map(E_DETACH_FROM_NAMESPACES_ABOVE);

        if (childh->m_parent)
        {
		    childh->m_parent->rbtree_remove(childh);
// childh->m_parent->verify_whole_tree();
        }

        if (oid != EOID_CHILD) childh->m_oid = oid;
		childh->m_oflags |= EOBJ_IS_RED;
		childh->m_left = childh->m_right = childh->m_up = OS_NULL;
		mm_handle->rbtree_insert(childh);
        childh->m_parent = mm_handle;

        /* Map names back: If not disabled by user flag EOBJ_NO_MAP, then attach all names of 
           child object (this) and it's childen to name spaces. If a name is already mapped, 
           it is not remapped.
           If we are adoprion from a=one tree structure to another (sync is on), we need to set 
           m_root pointer (pointer to eRoot of a tree structure)to all child objects.
         */
        mapflags = sync ? E_SET_ROOT_POINTER : 0;
        if ((aflags & EOBJ_NO_MAP) == 0) 
        {
            mapflags |= E_ATTACH_NAMES;
        }

        if (mapflags)
        {        
            child->mm_root = mm_root;
            child->map(E_ATTACH_NAMES|E_SET_ROOT_POINTER);
        }

// mm_root->mm_handle->verify_whole_tree();

    
        if (sync) osal_mutex_system_unlock();
    }
}


/**
****************************************************************************************************

  @brief Create name space for this object.

  The eObject::ns_create() function creates a name space for object. If object already
  has name space...
  
  @param   namespace_id Identifier for the name space.
  @return  None.

****************************************************************************************************
*/
void eObject::ns_create(
	os_char *namespace_id)
{
	eNameSpace
		*ns;

	/* If object has already name space.
	 */
	ns = eNameSpace::cast(first(EOID_NAMESPACE));
	if (ns)
	{
		/* If namespace identifier matches, just return.
		 */
        if (ns->m_namespace_id)
        {
            if (!os_strcmp(namespace_id, ns->m_namespace_id->gets()))
                return;
        }

		/* Delete old name space.
           We should keep ot if we want to have multiple name spaces???
		 */
		delete ns;
	}

	/* Create name space.
	 */
	ns = eNameSpace::newobj(this, EOID_NAMESPACE);
    if (namespace_id)
    {
        ns->m_namespace_id = new eVariable(ns);
        ns->m_namespace_id->sets(namespace_id);
    }

	/* Remap names in child objects ??? Do we need this. In practise name space is created 
	   before placing children in?
	 */
}


/**
****************************************************************************************************

  @brief Delete name space of this object.

  The eObject::ns_delete() function deletes name space of this object.
  
  @return  None.

****************************************************************************************************
*/
void eObject::ns_delete()
{
	delete first(EOID_NAMESPACE);
}


/**
****************************************************************************************************

  @brief Find eName by name value and namespace identifier.

  The eObject::ns_first() function finds the first eName object matching to name. If name
  is OS_NULL, the function returns the first name in namespace (if any).
  Name may contain name space identifier, for example "myid/myname" in which case the name 
  space identifier given as argumenr is ignored.
  
  @param   name Pointer to eVariable or eName, holding name to search for. Name can be prefixed
           by name space identifier. If OS_NULL, then pointer to first name in name space
           is returned.
  @param   Name space identifier string. OS_NULL to specify name space of this object.
  @return  Pointer to name. OS_NULL if none found.

****************************************************************************************************
*/
eName *eObject::ns_first(
    os_char *name,
    os_char *namespace_id)
{
    if (name == OS_NULL)
    {
        return ns_firstv(OS_NULL, namespace_id);
    }
    else
    {
        eVariable n;
        n = name;
        return ns_firstv(&n, namespace_id);
    }
}


/**
****************************************************************************************************

  @brief Find eName by name and name space.

  The eObject::ns_firstv() function finds the first eName object matching to name. If name
  is OS_NULL, the function returns the first name in namespace (if any).
  Name may contain name space identifier, for example "myid/myname" in which case the name 
  space identifier given as argumenr is ignored.
  
  @param   name String name to search for. Name can be prefixed by name space identifier. 
           If OS_NULL, then pointer to first name in name space is returned.
  @param   Name space identifier string. OS_NULL to specify name space of this object.
  @return  Pointer to name. OS_NULL if none found.

****************************************************************************************************
*/
eName *eObject::ns_firstv(
    eVariable *name,
    os_char *namespace_id)
{
    eNameSpace 
        *ns;

    eName 
        *n;

    os_char 
        *p, 
        *q;

    eVariable 
        *tmp_name = OS_NULL,
        *tmp_id = OS_NULL;

    /* String type may contain name space prefix, check for it.
     */
    if (name) if (name->type() == OS_STRING) 
    {
        p = name->gets();
        q = os_strechr(p, '/');
        if (q)
        {
            tmp_id = new eVariable;
            tmp_id->sets(p, q-p);
            namespace_id = tmp_id->gets();

            tmp_name = new eVariable;
            tmp_name->sets(q+1);
            name = tmp_name;
        }
    }

    /* Find name space.
     */    
    ns = findnamespace(namespace_id);
    if (ns == OS_NULL) return OS_NULL;

    /* Find name in name space.
     */
    n = ns->findname(name);

    /* Cleanup and return.
     */
    if (tmp_name) 
    {
        delete tmp_name;
        delete tmp_id;
    }
    return n;
}


/**
****************************************************************************************************

  @brief Find object by name.

  The eObject::ns_get() function finds the first named object matching to name in speficied 
  namespace. If name is OS_NULL, the function returns the first name in namespace
  (if any).  Name may contain name space identifier, for example "myid/myname" in which case 
  the name space identifier given as argumenr is ignored.
  
  @param   name Pointer to eVariable or eName, holding name to search for. Name can be prefixed
           by name space identifier. If OS_NULL, then pointer to first name in name space
           is returned.
  @param   Name space identifier string. OS_NULL to specify name space of this object.
  @return  Pointer to name. OS_NULL if none found.

****************************************************************************************************
*/
eObject *eObject::ns_get(
    os_char *name,
    os_char *namespace_id,
    os_int classid)
{
    eName 
        *n;

    eObject
        *p;

    n  = ns_first(name, namespace_id);
    while (n)
    {
        p = n->parent();
        if (classid == ECLASSID_OBJECT || p->classid() == classid) return p;
        n = n->ns_next();
    }

    return OS_NULL;
}


eVariable *eObject::ns_getv(
    os_char *name,
    os_char *namespace_id)
{
    return eVariable::cast(ns_get(name, namespace_id, ECLASSID_VARIABLE));
}


eContainer *eObject::ns_getc(
    os_char *name,
    os_char *namespace_id)
{
    return eContainer::cast(ns_get(name, namespace_id, ECLASSID_CONTAINER));
}


/**
****************************************************************************************************

  @brief Find name space by naespace identifier.

  The eObject::findnamespace() function adds name to this object and maps it to name space.

  Notice: ".." Refers to name space of this object or closest parent.
  
  @param  namespace_id Identifier for the name space. OS_NULL refers to first parent name space,
          regardless of name space identifier.
  @param  info Pointer where to set information bits. OS_NULL if not needed. 
          E_INFO_PROCES_NS bit indicates that the name space is process namespace. 
          E_INFO_ABOVE_CHECKPOINT bit indicates that name space is above check point in tree 
          structure.
  @param  checkpoint Pointer to object in tree structure to check if name space is above this one.
          OS_NULL if not needed.
  @return Pointer to name space, eNameSpace class. OS_NULL if none found.

****************************************************************************************************
*/
eNameSpace *eObject::findnamespace(
	os_char *namespace_id,
    os_int *info,
    eObject *checkpoint)
{
	eNameSpace
		*ns;

    eHandle
        *h,
        *ns_h;

    os_boolean
        getparent;

    /* No information yet.
     */
    if (info) *info = 0;

	/* If name space id NULL, it is same as parent name space.
	 */
    if (namespace_id == OS_NULL)
    {
        namespace_id = E_PARENT_NS;
    }

    switch (*namespace_id)
    {
	    /* If name space id refers to process name space, just return pointer to it.
	     */
        case '/':
            if (info) *info = E_INFO_PROCES_NS|E_INFO_ABOVE_CHECKPOINT;
            return eglobal->process_ns;

	    /* If thread name space, just return pointer to the name space.
	     */
        case '\0':
            if (mm_root == OS_NULL) return OS_NULL;
            if (info) *info = E_INFO_ABOVE_CHECKPOINT;
            return eNameSpace::cast(mm_root->first(EOID_NAMESPACE));

        default:
            if (!os_strcmp(namespace_id, "."))
            {
                if ((flags() & EOBJ_HAS_NAMESPACE) == 0) return OS_NULL;
                return eNameSpace::cast(first(EOID_NAMESPACE));
            }
            else 
            {
                getparent = (os_boolean)!os_strcmp(namespace_id, "..");
            }
            break;
    }

    /* Look upwards for parent or matching name space.
     */
    h = mm_handle;
    while (h)
    {
        if (h->flags() & EOBJ_HAS_NAMESPACE)
        {
            /* Support multiple name spaces per object.
             */
            ns_h = h->first(EOID_NAMESPACE);
            while (ns_h)
            {
                if (getparent)
                    return eNameSpace::cast(ns_h->object());

                /* If name space has identifier, and it matches?
                 */
                if (namespace_id)
                {
                    ns = eNameSpace::cast(ns_h->object());
                    if (ns) if (ns->m_namespace_id)
                    {
                        if (os_strcmp(namespace_id, ns->m_namespace_id->gets()))
                            return ns;
                    }
                }
                ns_h = ns_h->next(EOID_NAMESPACE);
            }
        }

        if (info) if (h->m_object == checkpoint) *info |= E_INFO_ABOVE_CHECKPOINT;
        h = h->parent();        
    }

	return OS_NULL;
}


/**
****************************************************************************************************

  @brief Name this object.

  The eObject::addname() function adds name to this object and maps it to name space.
  
  @param   name Name for this object. Name may start with name space identifier separated with
           slash from actual name. If name space is identified as part of name, the namespace_id
		   argument will be ignored.
  @param   namespace_id Identifier for the name space. OS_NULL is first parent's name space.
  @param   flags 
  @return  Pointer to newly created name, eName class. 

****************************************************************************************************
*/
eName *eObject::addname(
	os_char *name,
    os_int flags,
	os_char *namespace_id)
{
	eName
		*n;

	/* Create name object.
	 */
	n = new eName(this, EOID_NAME);

	/* Set name string, if any.
	 */
	if (name) n->sets(name);

    /* Set flags for name, like persistancy.
     */
    if (flags & ENAME_TEMPORARY)
    {
        n->setflags(EOBJ_NOT_CLONABLE|EOBJ_NOT_SERIALIZABLE);
    }
  
    /* If name space is not given as argument, decide by flags.
     */
    if (namespace_id == OS_NULL)
    {
        if (flags & ENAME_PROCESS_NS)
        {
            namespace_id = E_PROCESS_NS;
        }
        else if (flags & ENAME_THREAD_NS)
        {
            namespace_id = E_THREAD_NS;
        }
        else if (flags & ENAME_THIS_NS)
        {
            namespace_id = E_THIS_NS;
        }
    }

    /* Set name space identifier.
     */
    n->setnamespaceid(namespace_id);
	
	/* Map name to namespace, unless disabled for now.
	 */
	if ((flags & ENAME_NO_MAP) == 0) 
    {
        n->mapname();
    }

	/* Return pointer to name.
	 */
	return n;
}


/**
****************************************************************************************************

  @brief Attach/detach names in tree sturcture to name spaces. Set eRoot pointers.

  The eObject::map() function is attaches/detaches names from name spaces, and sets used
  eRoot object pointer for child objects. The functionality is controlled by flags.
  This is function is mostly used to remap names when an object (tree structure) is adopted 
  from one thread to another. And when queuing messages for threads and outgoing connections.
  
  @param   flags
           - E_ATTACH_NAMES: Attach all names of child object (this) and it's childen to 
             name spaces. If a name is already mapped, it is not remapped.

           - E_SET_ROOT_POINTER: Copy m_root pointer (pointer to eRoot of a tree structure)
             from child object (this) to all child objects of it.

           - E_DETACH_FROM_NAMESPACES_ABOVE: Detach names of child object (this) and it's 
             childen from name spaces above this object in tree structure.

  @return  None 

****************************************************************************************************
*/
void eObject::map(
    os_int mflags)
{
    osal_debug_assert(mm_handle != OS_NULL);

    /* Handle special case when this is name
     */
    if (mm_handle->oid() == EOID_NAME && 
       (mflags & (E_ATTACH_NAMES|E_DETACH_FROM_NAMESPACES_ABOVE)))
    {
        mapone(mm_handle, mflags);
    }

    /* Map all child objects.
     */
    map2(mm_handle, mflags);
}

void eObject::map2(
    eHandle *handle,
    os_int mflags)
{
    eHandle
        *childh;

    for (childh = handle->first(EOID_ALL);
         childh;
         childh = childh->next(EOID_ALL))
    {
        if (mflags & E_SET_ROOT_POINTER)
        {
            childh->m_object->mm_root = mm_root;
        }

        /* If this is name which needs to be attaached or detached, do it.
         */
        if (childh->oid() == EOID_NAME && 
           (mflags & (E_ATTACH_NAMES|E_DETACH_FROM_NAMESPACES_ABOVE)))
        {
            mapone(childh, mflags);
        }

        /* If this has children, process those.
         */
        if (childh->m_children) 
        {
            map2(childh, mflags);
        }
    }
}


/**
****************************************************************************************************

  @brief Attach or detach a name to a name space.

  The eObject::mapone() function attaches or detaches a name to/from name space.

  @return  None. 

****************************************************************************************************
*/
void eObject::mapone(
    eHandle *handle, 
    os_int mflags)
{
    eName
        *name;

    eNameSpace 
        *ns;

    os_int
        info;

    name = eName::cast(handle->m_object);

    ns = handle->m_object->findnamespace(name->namespaceid(), &info, this);

    if (mflags & E_ATTACH_NAMES)
    {
        name->mapname2(ns, info);
    }

    if (mflags & E_DETACH_FROM_NAMESPACES_ABOVE)
    {
        if (info & E_INFO_ABOVE_CHECKPOINT)
        {
            name->detach();
        }
    }
}


/**
****************************************************************************************************

  @brief Send message.

  The eObject::message() function sends message. The message will be recieved as onmessage call 
  by another object.
  
  @param   command
  @param   target
  @param   source
  @param   content
  @param   mflags 
  @param   context
  @return  None. 

****************************************************************************************************
*/
void eObject::message(
    os_int command, 
    os_char *target,
    os_char *source,
    eObject *content,
    os_int mflags,
    eObject *context)
{
    eEnvelope
        *envelope;

    envelope = new eEnvelope(this, EOBJ_IS_ATTACHMENT); // ?????????????????????????????????????????????????????????????????????????????????

    envelope->setcommand(command);
    envelope->setmflags(mflags & ~(EMSG_DEL_CONTENT|EMSG_DEL_CONTEXT));
    envelope->settarget(target);
    envelope->appendsource(source);
    envelope->setcontent(content, mflags);
    envelope->setcontext(context, mflags);
    message(envelope);
}


/**
****************************************************************************************************

  @brief Send message.

  The eObject::message() function sends message. The message will be recieved as onmessage call 
  by another object.
  
  @param   envelope
  @return  None. 

****************************************************************************************************
*/
void eObject::message(
    eEnvelope *envelope)
{
    os_char
        *target;

    /* Resolve path.
     */
    if ((envelope->mflags() & EMSG_NO_RESOLVE) == 0)
    {
        envelope->addmflags(EMSG_NO_RESOLVE);
    }

    /* Add oix to source path when needed.
     */
    if ((envelope->mflags() & (EMGS_NO_REPLIES|EMSG_NO_NEW_SOURCE_OIX)) == 0)
    {
        envelope->appendsourceoix(this);
        envelope->addmflags(EMSG_NO_NEW_SOURCE_OIX);
    }

    target = envelope->target();

    switch (*target)
    {
      /* Process or thread name space.
       */
      case '/':
        /* If process name space.
         */
        if (target[1] == '/') 
        {
            envelope->move_target_pos(2);
            process_ns_message(envelope);
        } 

        /* Otherwise thread name space.
         */
        else 
        {
            envelope->move_target_pos(1);
        }
        return;
            
      /* Parent or this object's name space
       */
      case '.':  
        /* If this object's name space.
         */
        if (target[1] == '/' || target[1] == '\0') 
        {
            return;
        } 

        /* Otherwise thread name space.
         */
        else if (target[1] == '.') 
             if (target[2] == '/' || target[2] == '\0')
        {
            return;
        }
        break;
    }

    /* Name or user specified name space.
     */
}

void eObject::process_ns_message(
    eEnvelope *envelope)
{
    eVariable
        *objname;

    eNameSpace
        *process_ns;

    eName
        *name;

    os_memsz 
        sz;

    eThread 
        *thread;

    process_ns = eglobal_process_ns();
    if (process_ns == OS_NULL) 
    {
        osal_debug_error("No process name space");
        goto getout;
    }

    /* If this is message to process ?
     */
    if (*envelope->target() == '\0')
    {
//        thread = processobj;
        goto getout;
    }

    /* Otherwise message to named object.
     */
    else
    {
        /* Get next object name in target path. 
           Remember length of object name.
         */
        objname = new eVariable(this);
        envelope->nexttarget(objname);
        objname->gets(&sz);

        /* Synchronize.
         */
        osal_mutex_system_lock();

        /* Find the name in process name space. Done with objname.
         */
        name = process_ns->findname(objname);

        /* If not found
         */
        if (name == OS_NULL)
        {
            /* Post notarget message !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

            /* End synchronization
             */
            osal_mutex_system_unlock();
            delete objname;
            goto getout;
        }

        /* Get thread to which the named object belongs to.
         */
        thread = name->thread();

        /* Remove object name from envelope's target path and 
           move the envelope to thread's message queue.
         */
        envelope->move_target_over_objname((os_short)sz - 1);
        thread->queue(envelope);

        /* End synchronization
         */
        osal_mutex_system_unlock();

        delete objname;
    }

    return;

getout:
    delete envelope;
}

eStatus eObject::onmessage(
    eEnvelope *envelope)
{
    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Write object to stream.

  The eObject::write() function writes object with class information, attachments, etc to
  the stream.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eObject::write(
    eStream *stream, 
    os_int sflags) 
{
    eObject 
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
    if (writer(stream, sflags)) goto failed;

    /* Write attachments.
     */
    for (child = first(EOID_ALL); child; child = child->next(EOID_ALL))
    {
        if (child->isserattachment()) 
        {
            if (child->write(stream, sflags)) goto failed;
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


/**
****************************************************************************************************

  @brief Read object from stream.

  The eObject::read() function reads class information, etc from the stream, creates new 
  child object and reads child object content and attachments.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags.

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eObject *eObject::read(
    eStream *stream, 
    os_int sflags)
{
    os_int
        cid,
        oid,
        oflags;

    os_long
        n_attachements,
        i;

    eObject
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
    if (child->reader(stream, sflags)) goto failed;

    /* Read attachments.
     */
    for (i = 0; i<n_attachements; i++)
    {
        if (read(stream, sflags) == OS_NULL) goto failed;
    }

    /* Object succesfully read, return pointer to it.
     */
    return child;

    /* Reading object failed.
     */
failed:
    return OS_NULL;
}
