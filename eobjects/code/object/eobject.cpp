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

/* Name space identifiers as static strings. eobj_this_ns is default
   for ns_first and ns_firstv functions()
 */
os_char eobj_process_ns[] = E_PROCESS_NS;
os_char eobj_thread_ns[] = E_THREAD_NS;
os_char eobj_parent_ns[] = E_PARENT_NS;
os_char eobj_this_ns[] = E_THIS_NS;


/**
****************************************************************************************************

  @brief Object constructor.

  The eObject constructor creates an empty object. This constructor is called when object of
  any eobject library is constructed.

  @param   parent Pointer to parent object, or OS_NULL if the object is an orphan. If parent
           object is specified, the object will be deleted when parent is deleted.
  @param   id Object identifier for new object. If not specified, defauls to EOID_ITEM (-1),
		   which is generic list item. 
  
  @param   flags 

  @return  None.

****************************************************************************************************
*/
eObject::eObject(
    eObject *parent,
    e_oid id,
	os_int flags)
{
    eRoot *root;

    mm_handle = OS_NULL;
    flags &= EOBJ_CLONE_MASK;

    mm_parent = parent;

	/* If this if not primitive object? 
	 */
    if (id != EOID_ITEM || parent != OS_NULL)
	{
		/* No parent, allocate root object?
		 */
		if (parent == OS_NULL)
		{
			/* Allocate root helper object hand two handles. 
			 */
            makeroot(id, flags);
		}

		/* If not root object constructor?
		   Otherwise normal child object.  Copy parent's root object pointer
		   and allocate handle for the new child object object.
		*/
        else if (id != EOID_ROOT_HELPER)
		{
            /* If parent has no root helper object, create one
             */
            if (parent->mm_handle == OS_NULL)
            {
                parent->makeroot(EOID_ITEM, EOBJ_DEFAULT);
            }

			root = parent->mm_handle->m_root;
            root->newhandle(this, parent, id, flags);
		}
	}
}


/**
****************************************************************************************************

  @brief Create root helper object and handles for root and root helper.

  The eObject::makeroot() function...
  This object is pointer to tree root (not helper).
 
  @return  None.

****************************************************************************************************
*/
void eObject::makeroot(
    e_oid id,
	os_int flags)
{
    eRoot *root;

	/* Allocate root helper object. 
	*/
	root = new eRoot(this, EOID_ROOT_HELPER,
		EOBJ_IS_ATTACHMENT | EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE);

	/* Allocate handle for this object
	 */
    root->newhandle(this, OS_NULL, id, flags);

	/* Allocate handle for the root helper object.
	 */
	root->newhandle(root, this, EOID_ROOT_HELPER, 
		EOBJ_IS_ATTACHMENT | EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE);
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
	if (mm_handle) 
    {
        os_lock();

        /* if (mm_handle->m_parent)
        if (mm_parent)
            */
        if (mm_handle)
        {
            mm_handle->delete_children();
        }

        if ((flags() & EOBJ_FAST_DELETE) == 0)
        {
            /* If handle has parent, remove from parent's children.
             */
            /* if (mm_handle->m_parent)
            {
			    mm_handle->m_parent->rbtree_remove(mm_handle);
            } */
            if (mm_parent)
            {
                mm_parent->mm_handle->rbtree_remove(mm_handle);
            }

            /* Handle no longer needed.
             */
            mm_handle->m_root->freehandle(mm_handle);

        }
        os_unlock();
    }
}


/**
****************************************************************************************************

  @brief Clone object

  The eObject::clone function is base class only. Cloning eObject base class is not supported.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.

****************************************************************************************************
*/
eObject *eObject::clone(
    eObject *parent, 
    e_oid id,
    os_int aflags)
{
	osal_debug_error("clone() not supported for the class");
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Helper function for clone functionality

  The eObject::clonegeneric is helper function to implement cloning object. It copies attachments
  or all child objects and maps names to name space. This depends on flags.

  @param  clonedobj Pointer to cloned object being created.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names. EOBJ_CLONE_ALL_CHILDREN
          to clone all children, not just attachments.
  @return None.

****************************************************************************************************
*/
void eObject::clonegeneric(
    eObject *clonedobj,
    os_int aflags)
{
    eHandle *handle;

    /* If there is no handle pointer, there can be no children to clone.
     */
    if (mm_handle == OS_NULL) return;

    /* Copy clonable attachments or all clonable object.
     */
    for (handle = mm_handle->first();
         handle;
         handle = handle->next())
    {
        if (((handle->m_oflags & EOBJ_IS_ATTACHMENT) || 
             (aflags & EOBJ_CLONE_ALL_CHILDREN)) &&
             (handle->m_oflags & EOBJ_NOT_CLONABLE) == 0)
        {
            handle->m_object->clone(clonedobj, handle->oid(), EOBJ_NO_MAP);
        }
    }

    /* Map names to name spaces.
     */
    if ((aflags & EOBJ_NO_MAP) == 0)
    {
        map(E_ATTACH_NAMES);
    }
}


/**
****************************************************************************************************

  @brief Allocate new object of any listed class.

  The eObject::newobject function looks from global class list by class identifier. If static 
  constructor member function corresponding to classid given as argument is found, then an
  object of that class is created as child object if this object.

  @param   cid Class identifier, specifies what kind of object to create.
  @param   oid Object identifier for the new object.
  @return  Pointer to newly created child object, or OS_NULL if none was found.

****************************************************************************************************
*/
eObject *eObject::newobject(
    eObject *parent,
    os_int cid,
    e_oid id,
	os_int flags) 
{
    eNewObjFunc func;

    /* Look for static constructor by class identifier. If not found, return OS_NULL.
     */
    func = eclasslist_newobj(cid);
    if (func == OS_NULL) return OS_NULL;

    /* Create new object of the class.
     */
    return func(parent, id, flags);
}


#if EOVERLOAD_NEW_AND_DELETE
/**
****************************************************************************************************

  @brief Overloaded new operator.

  The new operator maps object memory allocation to OSAL function os_malloc().

  @param   size Number of bytes to allocate.
  @return  Pointer to allocated memory block.

****************************************************************************************************
*/
void *eObject::operator new(
	size_t size)
{
	os_char *buf;
		
	size += sizeof(os_memsz);
	buf = os_malloc((os_memsz)size, OS_NULL);

	*(os_memsz*)buf = (os_memsz)size;

	return buf + sizeof(os_memsz);
}
#endif

#if EOVERLOAD_NEW_AND_DELETE
/**
****************************************************************************************************

  @brief Overloaded delete operator.

  The delete operator maps freeing object memory to OSAL function os_free().

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
		os_free(buf, *(os_memsz*)buf);
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
        e_oid id = EOID_CHILD)
	{
		if (mm_handle) return mm_handle->childcount(oid);
		return 0;
	} */


/**
****************************************************************************************************

  @brief Convert oix and ucnt to string.

  The eObject::oixstr function creates string which contains both object index and use counter
  of this object. This string can be used as unique identifier of an object/

  Example strings:
  - "@17_3" oix=15, ucnt = 3
  - "@15" oix=15, ucnt = 0

  @param  buf Buffer for resulting string. Recommended size is E_OIXSTR_BUF_SZ.
  @return Size of buffer in bytes. 

****************************************************************************************************
*/
void eObject::oixstr(
    os_char *buf, 
    os_memsz bufsz)
{
    os_int pos, ucnt;

    osal_debug_assert(mm_handle);

    pos = 0;
    buf[pos++] = '@';
    pos += (os_int)osal_int_to_string(buf+pos, bufsz-pos, mm_handle->oix()) - 1;
    if (pos < sizeof(buf)-1) 
    {
        ucnt = mm_handle->ucnt();
        if (ucnt)
        {
            buf[pos++] = '_';
            pos += (os_int)osal_int_to_string(buf+pos, bufsz-pos, mm_handle->ucnt()) - 1;
        }
    }
}


/**
****************************************************************************************************

  @brief Parse oix and ucnt to string.

  The eObject::oixparse function parses object index and use counter from string. See 
  eObject::oixstr() function.

  @param  str Pointer to string to parse. 
   buf Buffer for resulting string. Recommended size is E_OIXSTR_BUF_SZ.
  @return Number of characters parsed to skip over. Zero if the function failed.

****************************************************************************************************
*/
os_short eObject::oixparse(
    os_char *str,
    e_oix *oix, 
    os_int *ucnt)
{
    os_memsz count;
    os_char *p;

    p = str;

    if (*(p++) != '@') goto failed;

    *oix = (e_oix)osal_str_to_int(p, &count);
    if (count == 0) goto failed;
    p += count;
    if (*p != '_') 
    {
        *ucnt = 0;
        return (os_short)(p - str);
    }
    p++;

    *ucnt = (os_int)osal_str_to_int(p, &count);
    p += count;
    return (os_short)(p - str);

failed:
    *oix = 0;
    *ucnt = 0;
    return 0;
}



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
	if (mm_handle) 
    {
        eObject *o = mm_handle->m_root->parent();
        osal_debug_assert(o);
        
        /* If this is thread, return pointer.
         */
        if (o->isthread()) return (eThread*)o;
    }
	return OS_NULL;
}

/**
****************************************************************************************************

  @brief Get first child object identified by oid.

  The eObject::first() function returns pointer to the first child object of this object.

  @param   id Object idenfifier. Default value EOID_CHILD specifies to count a child objects,
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child object, or OS_NULL if none found.

****************************************************************************************************
*/
eObject *eObject::first(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->first(id);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Get the first child variable identified by oid.

  The eObject::firstv() function returns pointer to the first child variable of this object.

  @param   id Object idenfifier. Default value EOID_CHILD specifies to count a child objects,
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child variable. Or OS_NULL if none found.

****************************************************************************************************
*/
eVariable *eObject::firstv(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->first(id);
    while (h)
    {
        if (h->m_object->classid() == ECLASSID_VARIABLE) 
            return eVariable::cast(h->m_object);

        h = h->next(id);
    }
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Get the first child container identified by oid.

  The eObject::firstc() function returns pointer to the first child container of this object.

  @param   id Object idenfifier. Default value EOID_CHILD specifies to count a child objects,
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child container. Or OS_NULL if none found.

****************************************************************************************************
*/
eContainer *eObject::firstc(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->first(id);
    while (h)
    {
        if (h->object()->classid() == ECLASSID_CONTAINER) 
            return eContainer::cast(h->m_object);

        h = h->next(id);
    }
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Get the first child name identified by oid.

  The eObject::firstn() function returns pointer to the first child name of this object.

  @param   id Object idenfifier. Default value EOID_CHILD specifies to count a child objects,
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child name. Or OS_NULL if none found.

****************************************************************************************************
*/
eName *eObject::firstn(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->first(id);
    while (h)
    {
        if (h->object()->classid() == ECLASSID_NAME) 
            return eName::cast(h->m_object);

        h = h->next(id);
    }
    return OS_NULL;
}


/* Get last child object identified by oid.
*/
eObject *eObject::last(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->last(id);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Get next child object identified by oid.

  The eObject::next() function returns pointer to the next child object of this object.

  @param   id Object idenfifier. Default value EOID_CHILD specifies to count a child objects,
		   which are not flagged as an attachment. Value EOID_ALL specifies to get count all 
           child objects, regardless wether these are attachment or not. Other values
		   specify object identifier, only children with that specified object identifier 
           are searched for.

  @return  Pointer to the first child object, or OS_NULL if none found.

****************************************************************************************************
*/
eObject *eObject::next(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->next(id);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/* Get previous object identified by oid.
*/
eObject *eObject::prev(
    e_oid id)
{
	if (mm_handle == OS_NULL) return OS_NULL;
    eHandle *h = mm_handle->prev(id);
	if (h == OS_NULL) return OS_NULL;
	return h->m_object;
}


/**
****************************************************************************************************

  @brief Adopt obeject as child.

  The eObject::adopt() function moves on object from it's position in tree structure to
  an another. 
  
  @param   id EOID_CHILD object identifier unchanged.
  @param   aflags EOBJ_BEFORE_THIS Adopt before this object. EOBJ_NO_MAP not to map names.
  @return  None.

****************************************************************************************************
*/
void eObject::adopt(
    eObject *child, 
    e_oid id,
    os_int aflags)
{
    os_boolean sync;
    eHandle *childh;
    os_int mapflags;

    /* Make sure that parent object is already part of tree structure.
     */
	if (mm_handle == OS_NULL) 
    {
        osal_debug_error("adopt(): parent object is not part of tree");
		return;
    }
    
    if (child->mm_handle == OS_NULL)
    {
        sync = OS_FALSE; // || m_root->is_process ???????????????????????????????????????????????????????????????????????
        if (sync) os_lock();

        child->mm_parent = this;
        mm_handle->m_root->newhandle(child, this, id, 0);

        if (sync) os_unlock();
    }

    else
    {
// child->mm_handle->verify_whole_tree();
// mm_handle->verify_whole_tree();

        // Detach names

        childh = child->mm_handle;

        /* Synchronize if adopting from three structure to another.
         */
        sync = (mm_handle->m_root != childh->m_root);

        if (sync) 
        {
            os_lock();
        }

        /* Detach names of child object and it's childen from name spaces 
           above this object in tree structure.
         */
        child->map(E_DETACH_FROM_NAMESPACES_ABOVE);

        /* if (childh->m_parent)
        {
		    childh->m_parent->rbtree_remove(childh);
        } */
        if (child->mm_parent)
        {
            child->mm_parent->mm_handle->rbtree_remove(childh);

        }

        child->mm_parent = this;

        if (id != EOID_CHILD) childh->m_oid = id;
		childh->m_oflags |= EOBJ_IS_RED;
		childh->m_left = childh->m_right = childh->m_up = OS_NULL;
		mm_handle->rbtree_insert(childh);
        /* childh->m_parent = mm_handle; */

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
            childh->m_root = mm_handle->m_root;
            child->map(E_ATTACH_NAMES|E_SET_ROOT_POINTER);
        }

// mm_root->mm_handle->verify_whole_tree();

    
        if (sync) os_unlock();
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
    const os_char *namespace_id)
{
	eNameSpace *ns;

	/* If object has already name space.
	 */
	ns = eNameSpace::cast(first(EOID_NAMESPACE));
	if (ns)
	{
		/* If namespace identifier matches, just return.
		 */
        if (ns->namespaceid())
        {
            if (!os_strcmp(namespace_id, ns->namespaceid()->gets()))
                return;
        }

		/* Delete old name space.
           We should keep ot if we want to have multiple name spaces???
		 */
		delete ns;
        ns->setnamespaceid(OS_NULL);
	}

	/* Create name space.
	 */
	// ns = eNameSpace::newobj(this, EOID_NAMESPACE);
	ns = new eNameSpace(this, EOID_NAMESPACE);
    if (namespace_id)
    {
        ns->setnamespaceid(new eVariable(ns));
        ns->namespaceid()->sets(namespace_id);
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
    const os_char *name,
    const os_char *namespace_id)
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
    const os_char *namespace_id)
{
    eNameSpace *ns;
    eName *n;
    os_char *p, *q;
    eVariable *tmp_name, *tmp_id;

    tmp_name = tmp_id = OS_NULL;

    /* String type may contain name space prefix, check for it.
     */
    if (name) if (name->type() == OS_STR) 
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
    const os_char *name,
    const os_char *namespace_id,
    os_int cid)
{
    eName *n;
    eObject *p;

    n  = ns_first(name, namespace_id);
    while (n)
    {
        p = n->parent();
        if (cid == ECLASSID_OBJECT || p->classid() == cid) return p;
        n = n->ns_next();
    }

    return OS_NULL;
}


eVariable *eObject::ns_getv(
    const os_char *name,
    const os_char *namespace_id)
{
    return eVariable::cast(ns_get(name, namespace_id, ECLASSID_VARIABLE));
}


eContainer *eObject::ns_getc(
    const os_char *name,
    const os_char *namespace_id)
{
    return eContainer::cast(ns_get(name, namespace_id, ECLASSID_CONTAINER));
}


/**
****************************************************************************************************

  @brief Find name space by naespace identifier.

  The eObject::findnamespace() function adds name to this object and maps it to name space.

  - When looking for parent namespace with "..", the function returns next namespace ABOVE this 
    object. If this object has name space, it will not be returned.
  - When searching by name space identifier, the first name with matching identifier is returned.
    This can be object's (this) own name space.

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
    const os_char *namespace_id,
    os_int *info,
    eObject *checkpoint)
{
	eNameSpace *ns;
    eHandle *h, *ns_h;
    os_boolean getparent;

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
            if (info) *info = E_INFO_ABOVE_CHECKPOINT;
            return eNameSpace::cast(mm_handle->m_root->first(EOID_NAMESPACE));

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
    h = getparent ? (mm_parent ? mm_parent->mm_handle : OS_NULL) : mm_handle;
    /* h = getparent ? mm_handle->parent() : mm_handle; */
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
                    if (ns) if (ns->namespaceid())
                    {
                        if (os_strcmp(namespace_id, ns->namespaceid()->gets()))
                            return ns;
                    }
                }
                ns_h = ns_h->next(EOID_NAMESPACE);
            }
        }

        if (info) if (h->m_object == checkpoint) *info |= E_INFO_ABOVE_CHECKPOINT;
        /* h = h->parent(); */
        if (h->m_object->mm_parent == OS_NULL) break;
        h = h->m_object->mm_parent->mm_handle;
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
    const os_char *name,
    os_int flags,
    const os_char *namespace_id)
{
	eName *n;

	/* Create name object.
	 */
	n = new eName(this, EOID_NAME);

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
            namespace_id = eobj_process_ns;
        }
        else if (flags & ENAME_THREAD_NS)
        {
            namespace_id = eobj_thread_ns;
        }
        else if (flags & ENAME_THIS_NS)
        {
            namespace_id = eobj_this_ns;
        }
        else if (flags & ENAME_PARENT_NS)
        {
            namespace_id = eobj_parent_ns;
        }

        /* If name starts with namespace id.
           Notice: CUSTOM NAME SPACES MISSING!!!
         */
        else if (name)
        {
            if (name[0] == '/') 
            {
                if (name[1] == '/')
                {
                    namespace_id = eobj_process_ns;
                    name += 2;
                }
                else
                {
                    namespace_id = eobj_thread_ns;
                    name++;
                }
            }
            else if (name[0] == '.') 
            {
                if (name[1] == '/') 
                {
                    namespace_id = eobj_this_ns;
                    name += 2;
                }
                else if (name[1] == '.') if (name[2] == '/') 
                {
                    namespace_id = eobj_parent_ns;
                    name += 3;
                }
            }
        }
    }

	/* Set name string, if any.
	 */
	if (name) n->sets(name);

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
            childh->m_root = handle->m_root;
        }

        /* If this is name which needs to be attached or detached, do it.
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
    eName *name;
    eNameSpace *ns;
    os_int info;

    name = eName::cast(handle->m_object);

    ns = handle->m_object->findnamespace(name->namespaceid(), &info, this);

    if ((mflags & E_ATTACH_NAMES))
    {
        osal_debug_assert(ns);
        if (ns) name->mapname2(ns, info);
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

  @brief Get object by name.

  The eObject::message() function looks for name in this object's name space.

  @param  name Name to look for.  
  @return If name is found, returns pointer to namef object. Otherwise the function returns OS_NULL.

****************************************************************************************************
*/
eObject *eObject::byname(
    const os_char *name)
{
    eVariable namev;
    eName *nobj;
    eNameSpace *nspace;
    
    nspace = eNameSpace::cast(first(EOID_NAMESPACE));
    if (nspace)
    {
        namev.sets(name);
        nobj = nspace->findname(&namev);
        if (nobj) return nobj->parent();
    }
    return OS_NULL;
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
    const os_char *target,
    const os_char *source,
    eObject *content,
    os_int mflags,
    eObject *context)
{
    eEnvelope *envelope;
    eObject *parent;

    /* We use eRoot as pasent, in case object receiving message gets deleted.
       parent = this is just fallback mechanim.
     */
	if (mm_handle) 
    {
        parent = mm_handle->m_root;
    }
    else
    {
        parent = this;
    }

    envelope = new eEnvelope(parent, EOBJ_IS_ATTACHMENT);
    envelope->setcommand(command);
    envelope->setmflags(mflags & ~(EMSG_DEL_CONTENT|EMSG_DEL_CONTEXT));
    envelope->settarget(target);
    if (source) envelope->prependsource(source);
    envelope->setcontent(content, mflags);
    envelope->setcontext(context, mflags);
    message(envelope);
}


/**
****************************************************************************************************

  @brief Send message.

  The eObject::message() function sends message. The message will be recieved as onmessage call 
  by another object.
  
  @param   envelope Message envelope to send. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::message(
    eEnvelope *envelope)
{
    os_char *target, *namespace_id;
    os_memsz sz;

    /* Resolve path.
     */
    if ((envelope->mflags() & EMSG_NO_RESOLVE) == 0)
    {
        envelope->addmflags(EMSG_NO_RESOLVE);
    }

    /* Add oix to source path when needed.
     */
    if ((envelope->mflags() & (EMSG_NO_REPLIES|EMSG_NO_NEW_SOURCE_OIX)) == 0)
    {
        envelope->prependsourceoix(this);
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
            message_process_ns(envelope);
        } 

        /* Otherwise thread name space.
         */
        else 
        {
            envelope->move_target_pos(1);
            message_within_thread(envelope, E_THREAD_NS);
        }
        return;

      /* Targer specified using object index
       */
      case '@':
        message_oix(envelope);
        return;
            
      /* Parent or this object's name space
       */
      case '.':  
        /* If this object's name space.
         */
        if (target[1] == '/' || target[1] == '\0') 
        {
            envelope->move_target_over_objname(1);
            message_within_thread(envelope, eobj_this_ns);
            return;
        } 

        /* Otherwise parent name space.
         */
        else if (target[1] == '.') 
             if (target[2] == '/' || target[2] == '\0')
        {
            envelope->move_target_over_objname(2);
            message_within_thread(envelope, E_PARENT_NS);
            return;
        }
        break;
    }

    /* Name or user specified name space.
     */
    eVariable nspacevar;
    envelope->nexttarget(&nspacevar);
    namespace_id = nspacevar.gets(&sz);
    envelope->move_target_over_objname((os_short)sz-1);

    message_within_thread(envelope, namespace_id);
}


/**
****************************************************************************************************

  @brief Send message within thread.

  The eObject::message_process_ns is helper function for eObject::message() to send message
  trough process name space. It finds to which thread's object tree the message target belongs
  to and places message to that thread's message queue.
  
  @param   envelope Message envelope to send. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::message_within_thread(
    eEnvelope *envelope,
    const os_char *namespace_id)
{
	eNameSpace *nspace;
    eVariable objname;
    eName *name;
    os_memsz sz;

    nspace = findnamespace(namespace_id);
    if (nspace == OS_NULL) goto getout;

    /* Get next object name in target path. 
        Remember length of object name.
     */
    envelope->nexttarget(&objname);
    objname.gets(&sz);

    /* Find the name in process name space. Done with objname.
     */
    name = nspace->findname(&objname);
    if (name == OS_NULL)
    {
        goto getout;
    }

    name->parent()->onmessage(envelope);
    delete envelope;
    return;

getout:

    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, 
            EMSG_DEL_CONTEXT, 
            envelope->context());
    }

#if OSAL_DEBUG
    /* Report "no target: error
     */    
    if ((envelope->mflags() & EMSG_NO_ERRORS) == 0)
    {
        osal_debug_error("message() failed: Name or namespace not found within thread");
    }
#endif

    delete envelope;
}    


/**
****************************************************************************************************

  @brief Send message.

  The eObject::message_process_ns is helper function for eObject::message() to send message
  trough process name space. It finds to which thread's object tree the message target belongs
  to and places message to that thread's message queue.
  
  @param   envelope Message envelope to send. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::message_process_ns(
    eEnvelope *envelope)
{
    eNameSpace *process_ns;
    eName *name, *nextname;
    eThread *thread;
    os_memsz sz;
    os_char buf[E_OIXSTR_BUF_SZ], *oname, c;
    os_boolean multiplethreads;

    /* If this is message to process ?
     */
    c = *envelope->target();

    if (c == '@')
    {
        message_oix(envelope);
        return;
    }

    /* Get pointer to process namespace. This is never NULL (or if it is, it is programming error).
     */
    process_ns = eglobal_process_ns();

    if (c == '\0')
    {
        /* Synchronize.
         */
        os_lock();

        if (eglobal->process == OS_NULL) 
        {
            os_unlock();
#if OSAL_DEBUG
            osal_debug_error("message() failed: eobjects library not initialized");
#endif
            goto getout;
        }

        eglobal->process->queue(envelope);

        /* Done, finish with synnchronization and return.
         */
        os_unlock();
        return;
    }

    /* Otherwise message to named object.
     */
    else
    {
        eVariable objname;

        /* Get next object name in target path. 
           Remember length of object name.
         */
        envelope->nexttarget(&objname);
        oname = objname.gets(&sz);

        /* Synchronize.
         */
        os_lock();

        /* Find the name in process name space.
         */
        name = process_ns->findname(&objname);

        /* If name not found: End synchronization/clen up, reply with ECMD_NOTARGET
           and return.
         */
        if (name == OS_NULL)
        {
            os_unlock();
#if OSAL_DEBUG
            if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
            {
                osal_debug_error("message() failed: Name not found in process NS");   
                osal_debug_error(objname.gets());
            }
#endif
            goto getout;
        }

        /* Get thread to which the named object belongs to.
         */
        thread = name->thread();
        if (thread == OS_NULL)
        {
            os_unlock();
#if OSAL_DEBUG
            if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
            {
                osal_debug_error("message() failed: Name in process NS has no eThread as root");
            }
#endif
            goto getout;
        }

        /* Check if targeted to multiple threads.
         */
        multiplethreads = OS_FALSE;
        for (nextname = name->ns_next(); nextname; nextname = nextname->ns_next())
        {
            if (nextname->thread() != thread) 
            {
                multiplethreads = OS_TRUE;
                break;
            }
        }

        /* Single thread target (common case).
         */
        if (!multiplethreads)
        {
            /* If this is not message to thread itself.
             */
            if (thread != name->parent()) 
            {
                /* If object name is not already oix, convert to one.
                 */
                if (*oname != '@')
                {
                    envelope->move_target_over_objname((os_short)sz - 1);
                    name->parent()->oixstr(buf, sizeof(buf));
                    envelope->prependtarget(buf);
                }
            }
            else
            {
                /* Remove object name from envelope's target path.
                 */
                envelope->move_target_over_objname((os_short)sz - 1);
            }

            /* Move the envelope to thread's message queue.
             */
            thread->queue(envelope);
        }

        /* Multiple threads.
         */
        else
        {
            /* Save target path in envelope without name of next target.
             */
            envelope->move_target_over_objname((os_short)sz - 1);

            eVariable savedtarget, mytarget;
            savedtarget.sets(envelope->target());

            while (name)
            {
                nextname = name->ns_next();

                /* Get thread object pointer. 
                 */
                thread = name->thread();
                
                /* If message is not to thread itself.
                   Here we do replace the name with oix string, even 
                   if it is already that. We do this simply because 
                   this is unusual case where efficiency doesn not
                   matter much.
                 */
                if (thread != name->parent()) 
                {
                    name->parent()->oixstr(buf, sizeof(buf));
                    mytarget.sets(buf);
                    if (!savedtarget.isempty()) mytarget.appends("/");
                    mytarget.appendv(&savedtarget);
                    envelope->settarget(mytarget.gets());
                }
                else
                {
                    envelope->settarget(savedtarget.gets());
                }

                /* Queue the envelope and move on. If this is last target for 
                   the envelope, allow adopting the envelope.
                 */
                thread->queue(envelope, nextname == OS_NULL);
                name = nextname;
            }
        }

        /* End synchronization
         */
        os_unlock();
    }

    return;

getout:
    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, EMSG_DEL_CONTEXT, envelope->context());
    }

    delete envelope;
}


/**
****************************************************************************************************

  @brief Send message.

  The eObject::message_oix is helper function for eObject::message() to send message
  using object index string, like "@11_1". It finds to which thread's object tree the message 
  target belongs to. If this is in same object tree as the sender of the message message,
  then object's onmessage function is called directly. If target belongs to different object
  three from sender, the function places message to target thread's message queue.
  
  @param   envelope Message envelope to send. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::message_oix(
    eEnvelope *envelope)
{
    eHandle *handle;
    eThread *thread;
    e_oix oix;
    os_int ucnt;
    os_short count;

    /* Parse object index and use count from string.
     */
    count = oixparse(envelope->target(), &oix, &ucnt);
    if (count == 0)
    {
#if OSAL_DEBUG
        if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
        {
            osal_debug_error("message() failed: object index format error, not \"@11_2\" format");
        }
#endif
        goto getout;
    }

    /* Synchnronize and find handle pointer.
     */
    os_lock();
    handle = eget_handle(oix);
    if (ucnt != handle->m_ucnt)
    {
        os_unlock();
#if OSAL_DEBUG
        if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
        {
            osal_debug_error("message() failed: target object has been deleted");
        }
#endif
        goto getout;
    }

    /* If object is in same root tree (same thread), end syncronization and call function.
     */
    if (mm_handle->m_root == handle->m_root)
    {
        /* Advance in target path.
         */
        envelope->move_target_over_objname(count);

        os_unlock();
        handle->m_object->onmessage(envelope);
        delete envelope;
        return;
    }

    /* Otherwise different threads.
     */
    osal_debug_assert(handle->m_root);
    thread = eThread::cast(handle->m_root->parent());
    if (thread == handle->m_object) envelope->move_target_over_objname(count);
   
    /* Place the envelope in thread's message queue.
     */
    if (thread)
    {
        thread->queue(envelope);
    }
    else
    {
        delete envelope;
    }

    /* Finish with synchronization and return.
     */
    os_unlock();
    return;

getout:
    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, EMSG_DEL_CONTEXT, envelope->context());
    }

    delete envelope;
}


/**
****************************************************************************************************

  @brief Function to process incoming messages. 

  The eObject::onmessage function handles messages received by object. 
  
  @param   envelope Message envelope. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::onmessage(
    eEnvelope *envelope)
{
    os_char *target;
    eVariable objname;
    eNameSpace *nspace;
    eName *name, *nextname;
    os_memsz sz;
    os_int command;

    target = envelope->target();

    switch (*target)
    {
        /* Message to child object using object idenfifier.
         */
        case '@':
            onmessage_oix(envelope);
            break;

        /* Message to this object. 
         */
        case '\0':
            command = envelope->command();
            switch (command)
            {
              case ECMD_BIND:
                srvbind(envelope);
                return;

              case ECMD_UNBIND:
                /* THIS IS TRICKY: WE NEED TO FIND BINDING BY SOURCE
                    PATH AND FORWARD THIS TO IT */
                ;

            }
            osal_debug_error("onmessage(): Message not processed");
            break;

        /* Messages to internal names
         */
        case '_':
            command = envelope->command();
            /* If properties
             */
            if (target[1] == 'p')
            {
                /* Commands to specific property.
                 */
                if (target[2] == '/')
                {
                    switch (command)
                    {
                      case ECMD_SETPROPERTY:
                        setpropertyv(propertynr(target+3),
                            eVariable::cast(envelope->content()));
                        return;
                    }
                }
            }                       

            /* continues...
             */
    
        /* Messages to named child objects.
         */
        default:
            envelope->nexttarget(&objname);
            objname.gets(&sz);
            envelope->move_target_over_objname((os_short)sz-1);

            nspace = eNameSpace::cast(first(EOID_NAMESPACE));
            if (nspace == OS_NULL) goto getout;
            name = nspace->findname(&objname);
            if (name == OS_NULL) goto getout;
           
            do
            {
                nextname = name->ns_next();
                name->parent()->onmessage(envelope);
                name = nextname;
            } 
            while (name);
                
            break;
    }

    return;

getout:
    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, EMSG_KEEP_CONTENT, envelope->context());
    }

#if OSAL_DEBUG
    /* Show error message.
     */
    if ((envelope->mflags() & EMSG_NO_ERRORS) == 0)
    {
        osal_debug_error("onmessage() failed: target not found");
    }
#endif
}


/**
****************************************************************************************************

  @brief Forward message by object index within thread's object tree.

  The eObject::onmessage_oix forwards message using object index string, like "@11_1". 
  This function works only within thread.
  
  @param   envelope Message envelope to send. Contains command, target and source paths and
           message content, etc.
  @return  None. 

****************************************************************************************************
*/
void eObject::onmessage_oix(
    eEnvelope *envelope)
{
    eHandle *handle;
    e_oix oix;
    os_int ucnt;
    os_short count;

    /* Parse object index and use count from string.
     */
    count = oixparse(envelope->target(), &oix, &ucnt);
    if (count == 0)
    {
#if OSAL_DEBUG
        if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
        {
            osal_debug_error("onmessage() failed: object index format error, not \"@11_2\" format");
        }
#endif
        goto getout;
    }

    /* Find handle pointer.
     */
    handle = eget_handle(oix);
    if (ucnt != handle->m_ucnt)
    {
#if OSAL_DEBUG
        if ((envelope->flags() & EMSG_NO_ERRORS) == 0)
        {
            osal_debug_error("message() failed: target object has been deleted");
        }
#endif
        goto getout;
    }

    /* Here object must be in same root tree (same thread).
     */
    osal_debug_assert(mm_handle != OS_NULL);
    osal_debug_assert(mm_handle->m_root == handle->m_root);

    /* Advance in target path and call function.
     */
    envelope->move_target_over_objname(count);
    handle->m_object->onmessage(envelope);

    return;

getout:
    /* Send "no target" reply message to indicate that recipient was not found.
     */
    if ((envelope->mflags() & EMSG_NO_REPLIES) == 0)
    {
        message (ECMD_NO_TARGET, envelope->source(), 
            envelope->target(), OS_NULL, EMSG_DEL_CONTEXT, envelope->context());
    }
}



void eObject::setproperty_msg(
    const os_char *remotepath,
    eObject *x,
    const os_char *propertyname,
    os_int flags)
{
    eVariable
        path;

    if (propertyname)
    {
        path.sets(remotepath);
        path.appends("/_p/");
        path.appends(propertyname);
        remotepath = path.gets();
    }
    else
    {
        if (os_strstr(remotepath, "/_p/", OSAL_STRING_DEFAULT) == OS_NULL)
        {
            path.sets(remotepath);
            path.appends("/_p/x");
            remotepath = path.gets();
        }
    }

    message (ECMD_SETPROPERTY, remotepath, OS_NULL, x, EMSG_KEEP_CONTENT|EMSG_NO_REPLIES);
}


void eObject::setpropertyl_msg(
    const os_char *remotepath,
    os_long x,
    const os_char *propertyname)
{
    eVariable v;
    v.setl(x);
    setproperty_msg(remotepath,  &v, propertyname);
}

void eObject::setpropertyd_msg(
    const os_char *remotepath,
    os_double x,
    const os_char *propertyname)
{
    eVariable v;
    v.setd(x);
    setproperty_msg(remotepath,  &v, propertyname);
}

void eObject::setpropertys_msg(
    const os_char *remotepath,
    const os_char *x,
    const os_char *propertyname)
{
    eVariable v;
    v.sets(x);
    setproperty_msg(remotepath,  &v, propertyname);
}


/**
****************************************************************************************************

  @brief Add property to property set (any type).

  The addproperty function adds a property to class'es global property set. 
  
  @param  classid Specifies to which classes property set the property is being added.
  @param  propertynr Property number, class specific. 
  @param  propertyname Property name, class specific.
  @param  pflags Bit fields, combination of:
          - EPRO_DEFAULT (0): No options
          - EPRO_PERSISTENT: Property value is persistant is when saving classes properties.
          - EPRO_METADATA: Much like EPRO_PERSISTENT, but property value is saved only if
            also metadata is to be saved.
          - EPRO_SIMPLE: Do not keep copy of non default property in variable. Class implementation 
            takes care about this.
          - EPRO_NOONPRCH: Do not call onpropertychange when value changes. 
          - EPRO_NOPACK: Do not pack this property value within property set.
  @param  text Name of the property displayed to user. 

  @return Pointer to eVariable in property set defining the property. Additional attributes for
          property can be added trough this returned pointer.

****************************************************************************************************
*/
eVariable *eObject::addproperty(
    os_int cid, 
    os_int propertynr, 
    const os_char *propertyname,
    os_int pflags,
    const os_char *text)
{
    eContainer *propertyset;
    eVariable *p;

    /* Get pointer to class'es property set. If not found, create one. Property set always
       has name space
     */
    propertyset = eglobal->propertysets->firstc(cid);
    if (propertyset == OS_NULL)
    {
	    propertyset = new eContainer(eglobal->propertysets, cid, EOBJ_IS_ATTACHMENT);
        propertyset->ns_create();
    }

    /* Add variable for this property in property set and name it.
     */
    p = new eVariable(propertyset, propertynr, pflags); 
    p->addname(propertyname);

    /* Set name of the property to display to user.
     */
    if (text) p->setpropertys(EVARP_TEXT, text);

    return p;
}


/**
****************************************************************************************************

  @brief Property set for class done, complete it.

  The propertysetdone function lists attributes (subproperties) for each base property.
  
  @param  classid Specifies to which classes property set the property is being added.
  @return None.

****************************************************************************************************
*/
void eObject::propertysetdone(
    os_int cid)
{
    eContainer *propertyset;
    eVariable *p, *mp;
    eName *name;
    os_char *propertyname, *e;

    /* Get pointer to class'es property set. If not found, create one. Property set always
       has name space
     */
    propertyset = eglobal->propertysets->firstc(cid);
    if (propertyset == OS_NULL) return;

    for (p = propertyset->firstv(); p; p = p->nextv())
    {
        name = p->firstn();
        if (name == OS_NULL) continue;
        propertyname = name->gets();

        /* If this is subproperty, like "x.min", add to main propertie's list of subproperties.
         */
        e = os_strchr((os_char*)propertyname, '.');
        if (e) 
        {
            eVariable v;
            v.sets(propertyname, e - propertyname);
            mp = eVariable::cast(propertyset->byname(v.gets()));
            if (mp)
            {
                p->propertyv(EVARP_CONF, &v);
                v.appends(e);
                p->setpropertyv(EVARP_CONF, &v);
            }
        }
    }
}


/**
****************************************************************************************************

  @brief Add integer property to property set.

  The addpropertyl function adds property typed as integer to property set, and optionally
  sets default value for it. See addproperty() for more information.
  
  @param  x Default value.
  @return Pointer to eVariable in property set.

****************************************************************************************************
*/
eVariable *eObject::addpropertyl(
    os_int cid, 
    os_int propertynr, 
    const os_char *propertyname,
    os_int pflags,
    const os_char *text,
    os_long x)
{
    eVariable *p;
    p = addproperty(cid, propertynr, propertyname, pflags, text);
    p->setl(x);
    p->setpropertyl(EVARP_TYPE, OS_LONG);
    p->setl(x);
    return p;
}


/**
****************************************************************************************************

  @brief Add double property to property set.

  The addpropertyd function adds property typed as double precision float to property set, and 
  optionally sets default value for it. See addproperty() for more information.
  
  @param  x Default value.
  @return Pointer to eVariable in property set.

****************************************************************************************************
*/
eVariable *eObject::addpropertyd(
    os_int cid, 
    os_int propertynr, 
    const os_char *propertyname,
    os_int pflags,
    const os_char *text,
    os_double x,
    os_int digs)
{
    eVariable *p;
    p = addproperty(cid, propertynr, propertyname, pflags, text);
    p->setd(x);
    p->setpropertyl(EVARP_TYPE, OS_DOUBLE);
    p->setpropertyl(EVARP_DIGS, digs);
    p->setd(x);
    return p;
}


/**
****************************************************************************************************

  @brief Add string property to property set.

  The addpropertys function adds property typed as string to property set, and optionally sets
  default value for it. See addproperty() for more information.
  
  @param  x Default value.
  @return Pointer to eVariable in property set.

****************************************************************************************************
*/
eVariable *eObject::addpropertys(
    os_int cid, 
    os_int propertynr,
    const os_char *propertyname,
    os_int pflags,
    const os_char *text,
    const os_char *x)
{
    eVariable *p;
    p = addproperty(cid, propertynr, propertyname, pflags, text);
    p->setpropertyl(EVARP_TYPE, OS_STR);
    if (x)
    {
        p->sets(x);
    }
    return OS_NULL;
}


/**
****************************************************************************************************

  @brief Initialize properties to default values.

  The initproperties function can be called from class'es constructor, if classes properties need 
  to be initialized to default values. Properties with EPRO_SIMPLE or EPRO_NOONPRCH flag will not 
  be initialized.
  

****************************************************************************************************
*/
void eObject::initproperties()
{
    eContainer *propertyset;
    eVariable *p;

    /* Get global property set for the class.
       Synchnonize, to get class'es property set, in case someone is adding class at this moment.
     */
    os_lock();
    propertyset = eglobal->propertysets->firstc(classid());
    os_unlock();
    if (propertyset == OS_NULL)
    {
        osal_debug_error("setproperty: Class has no property support "
            "(did you call setupclass for it?)");
        return;
    }

    /* Get global eVariable describing this property.
     */
    for (p = propertyset->firstv();
         p;
         p = p->nextv())
    {
        if ((p->flags() & (EPRO_SIMPLE|EPRO_NOONPRCH)) == 0)
        {
            onpropertychange(p->oid(), p, 0);
        }
    }
}


/**
****************************************************************************************************

  @brief Get property number by property name.

  The propertynr() function gets property number for this class by property name.
  
  @param  propertyname Name of the property numnr
  @return Property number, -1 if failed.

****************************************************************************************************
*/
os_int eObject::propertynr(
    const os_char *propertyname)
{
    eContainer *propertyset;
    eNameSpace *ns;
    eName *name;
    os_int pnr;
    eVariable v;

    /* Synchronize.
     */
    os_lock();
 
    /* Get pointer to class'es property set. If not found, create one. Property set always
       has name space
     */
    propertyset = eglobal->propertysets->firstc(classid());
    if (propertyset == OS_NULL) goto notfound;

    /* Get property nr from global variable describing the property by name.
     */
    ns = eNameSpace::cast(propertyset->first(EOID_NAMESPACE));
    if (ns == OS_NULL) goto notfound;
    v.sets(propertyname);
    name = ns->findname(&v);
    if (name == OS_NULL) goto notfound;
    pnr = name->parent()->oid();

    /* End sync and return.
     */
    os_unlock();
    return pnr;

notfound:
    os_unlock();
    return -1;
}


/**
****************************************************************************************************

  @brief Get property name by property number.

  The propertynr() function gets property number for this class by property name.
  
  @param  propertyname Name of the property numnr
  @return Property number, OS_NULL if failed.

****************************************************************************************************
*/
os_char *eObject::propertyname(
    os_int propertynr)
{
    eContainer *propertyset;
    eName *name;
    eVariable *p;
    os_char *namestr;

    /* Synchronize.
     */
    os_lock();
 
    /* Get pointer to class'es property set. If not found, create one. Property set always
       has name space
     */
    propertyset = eglobal->propertysets->firstc(classid());
    if (propertyset == OS_NULL) goto notfound;

    /* Get global variable for this propery.
     */
    p = propertyset->firstv(propertynr);
    if (p == OS_NULL) goto notfound;

    /* get first name.
     */
    name = p->firstn(EOID_NAME);
    if (name == OS_NULL) goto notfound;
    namestr = name->gets();

    /* End sync and return.
     */
    os_unlock();
    return namestr;

notfound:
    os_unlock();
    return OS_NULL;
}

/**
****************************************************************************************************

  @brief Set property value.

  The setproperty function sets property value from eVariable.
  
  @param  propertynr
  @param  x
  @param  source
  @param  flags

  @return None.

****************************************************************************************************
*/
void eObject::setpropertyv(
    os_int propertynr, 
    eVariable *x, 
    eObject *source, 
    os_int flags)
{
    eContainer *propertyset;
    eSet *properties;
    eVariable *p;
    eVariable v;
    os_int pflags;

    /* Synchronize access to global property set.
     */
    os_lock();

    /* Get global property set for the class.
     */
    propertyset = eglobal->propertysets->firstc(classid());
    if (propertyset == OS_NULL)
    {
        osal_debug_error("setproperty: Class has no property support");
        os_unlock();
        return;
    }

    /* Get global eVariable describing this property.
     */
    p = propertyset->firstv(propertynr);
    if (p == OS_NULL)
    {
        osal_debug_error("setproperty: Property number is not valid for the class");
        os_unlock();
        return;
    }
    pflags = p->flags();

    /* Finished with synchronization.
     */
    os_unlock();

    /* Empty x and x as null pointer are thes ame thing, handle these in 
       the same way. 
     */
    if (x == OS_NULL) 
    {
        x = eglobal->empty;
    }

    /* If this is simple property without marking.
     */
    if (pflags & EPRO_SIMPLE)
    {
        /* If new value variable is same as current one, do nothing.
         */
        if (x->type() != OS_OBJECT)
        {
            propertyv(propertynr, &v);
            if (!v.compare(x)) return;
        }

        /* Call class'es onpropertychange function.
         */
        if ((pflags & EPRO_NOONPRCH) == 0)
        {
            onpropertychange(propertynr, x,  0); // CHECK FLAGS
        }
    }
    else
    {
        /* Get eSet holding stored property values. If it doesn't exist, create it.
         */
        properties = eSet::cast(first(EOID_PROPERTIES));
        if (properties == OS_NULL)
        {
            properties = new eSet(this, EOID_PROPERTIES);
        }

        /* Find stored property value. If matches value to set, do nothing.
         */
        properties->get(propertynr, &v);
        if (!v.compare(x)) return;

        /* Call class'es onpropertychange function.
         */
        if ((pflags & EPRO_NOONPRCH) == 0)
        {
            onpropertychange(propertynr, x,  0); // CHECK FLAGS
        }

        /* If x matches to default value, then remove the
           value from eSet. 
         */
        if (!p->compare(x)) 
        {
            properties->set(propertynr, OS_NULL);
        }

        /* No match. Store x in eSet. 
         */
        else
        {
            properties->set(propertynr, x);
        }
    }

    /* Forward property value to bindings, if any.
     */
    forwardproperty(propertynr, x, source, flags);
}


/**
****************************************************************************************************

  @brief Forward property change trough bindings.

  The forwardproperty function...
  
  @param  propertynr
  @param  x
  @param  source
  @param  flags

  @return None.

****************************************************************************************************
*/
void eObject::forwardproperty(
    os_int propertynr, 
    eVariable *x, 
    eObject *source, 
    os_int flags)
{
    eContainer *bindings;
    eObject *b, *nextb;

    /* Get bindings container.
     */
    bindings = firstc(EOID_BINDINGS);
    if (bindings == OS_NULL) return;

    for (b = bindings->first(); b; b = nextb)
    {
        nextb = b->next();
        if (b->classid() == ECLASSID_PROPERTY_BINDING && b != source)
        {
            ePropertyBinding::cast(b)->changed(propertynr, x, OS_FALSE);
        }
    }
}


/* Set property value as integer.
 */
void eObject::setpropertyl(
    os_int propertynr, 
    os_long x)
{
    eVariable v;
    v.setl(x);
    setpropertyv(propertynr, &v);
}

/* Set property value as double.
    */
void eObject::setpropertyd(
    os_int propertynr, 
    os_double x)
{
    eVariable v;
    v.setd(x);
    setpropertyv(propertynr, &v);
}

/* Set property value as string.
    */
void eObject::setpropertys(
    os_int propertynr, 
    const os_char *x)
{
    eVariable v;
    v.sets(x);
    setpropertyv(propertynr, &v);
}

/* Get property value.
 */
void eObject::propertyv(
    os_int propertynr, 
    eVariable *x, 
    os_int flags)
{
    eSet *properties;
    eContainer *propertyset;
    eVariable *p;
    
    /* Look for eSet holding stored property values. If found, check for 
       property number.
     */
    properties = eSet::cast(first(EOID_PROPERTIES));
    if (properties)
    {
        /* Find stored property value. If matches value to set, do nothing.
         */
        if (properties->get(propertynr, x)) return;
    }
    
    /* Check for simple property
     */
    if (simpleproperty(propertynr, x) == ESTATUS_SUCCESS) return;

    /* Look for default value. Start by synchronizing access to global property data.
     */
    os_lock();

    /* Get global property set for the class.
     */
    propertyset = eglobal->propertysets->firstc(classid());
    if (propertyset == OS_NULL)
    {
        osal_debug_error("setproperty: Class has no property support");
        goto getout;
    }

    /* Get global eVariable describing this property.
     */
    p = propertyset->firstv(propertynr);
    if (p == OS_NULL)
    {
        osal_debug_error("setproperty: Property number is not valid for the class");
        goto getout;
    }

    /* Finished with synchwonization.
     */
    os_unlock();

    /* Return default value for the property.
     */
    x->setv(p);
    return;

getout:
    os_unlock();
    x->clear();
}

os_long eObject::propertyl(
    os_int propertynr)
{
    eVariable v;
    propertyv(propertynr, &v);
    return v.geti();
}

os_double eObject::propertyd(
    os_int propertynr)
{
    eVariable v;
    propertyv(propertynr, &v);
    return v.getd();
}


/**
****************************************************************************************************

  @brief Bind this object's property to remote property.

  The eObject::bind() function creates binding to remote property. When two variables are bound
  together, they have the same value. When the other changes, so does the another. Bindings
  work over messaging, thus binding work as well between objects in same thread or objects in 
  different computers.
  
  @param  localpropertyno This object's propery number to bind.
  @param  remotepath Path to remote object to bind to.
  @param  remoteproperty Name of remote property to bind. If OS_NULL variable value
          is assumed.
  @param  bflags Combination of EBIND_DEFAULT (0), EBIND_CLIENTINIT, EBIND_NOFLOWCLT
          EBIND_METADATA and EBIND_TEMPORARY. 
          - EBIND_DEFAULT:  bind without special options.
          - EBIND_CLIENTINIT: Local property value is used as initial value. Normally
            remote end's value is used as initial value.
          - EBIND_NOFLOWCLT: Disable flow control. Normally if property value changes
            faster than it can be transferred, some values are skipped. If EBIND_NOFLOWCLT
            flag is given, it disables flow control and every value is transferred without
            any limit to buffered memory use.
          - EBIND_METADATA: If meta data, like text, unit, attributes, etc exists, it is 
            also transferred from remote object to local object.
          - EBIND_TEMPORARY: Binding is temporary and will not be cloned nor serialized.
  @param  envelope Used for server binding only. OS_NULL for clint binding.
  @return None.

****************************************************************************************************
*/
void eObject::bind(
    os_int localpropertynr,
    const os_char *remotepath,
    const os_char *remoteproperty,
    os_int bflags)
{
    eContainer *bindings;
    ePropertyBinding *binding;

    /* Get or create bindings container.
     */
    bindings = firstc(EOID_BINDINGS);
    if (bindings == OS_NULL)
    {
        bindings = new eContainer(this, EOID_BINDINGS, EOBJ_IS_ATTACHMENT);
    }

    /* Verify that same binding dows not already exist ?? How to modify bindings ????
     *?

    /* Create binding
     */
    binding = new ePropertyBinding(bindings, EOID_ITEM, (bflags & EBIND_TEMPORARY)
         ? EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE : EOBJ_DEFAULT);

    /* Bind properties. This function will send message to remote object to bind.
     */
    binding->bind(localpropertynr, remotepath, remoteproperty, bflags);
}


/**
****************************************************************************************************

  @brief Create server end of property binding.

  The eObject::srvbind() function 
  
  @param  envelope Message envelope, COMMAND ECMD_BIND.
  @return None.

****************************************************************************************************
*/
void eObject::srvbind(
    eEnvelope *envelope)
{
    eContainer *bindings;
    ePropertyBinding *binding;

    /* Get or create bindings container.
     */
    bindings = firstc(EOID_BINDINGS);
    if (bindings == OS_NULL)
    {
        bindings = new eContainer(this, EOID_BINDINGS, EOBJ_IS_ATTACHMENT);
    }

    /* Verify that same binding dows not already exist ?? How to modify bindings ????
     *?

    /* Create binding
     */
    binding = new ePropertyBinding(bindings, EOID_ITEM, 
         EOBJ_NOT_CLONABLE | EOBJ_NOT_SERIALIZABLE);

    /* Bind properties.
     */
    if (binding) 
    {
// envelope->json_write(&econsole); // ??????????????????????????????????????????????????????????????????????????????????????
        binding->srvbind(this,  envelope);
    }
}


/**
****************************************************************************************************

  @brief Bind this object's property to remote property.

  See bind() function above, this almost the same but remote path may contain also property 
  name, separated from path by "/_p/". If "/_p/" is not found, the "x" is used as default.

****************************************************************************************************
*/
void eObject::bind(
    os_int localpropertynr,
    const os_char *remotepath,
    os_int bflags)
{
    eVariable v;
    os_char *p, *e;

    v.sets(remotepath);
    p = v.gets();
    e = os_strstr(p, "/_p/", OSAL_STRING_DEFAULT);
    if (e) 
    {
        *e = '\0';
        e += 4;
    }
    else
    {
        e = evarp_value;
    }

    bind(localpropertynr, p, e, bflags);
} 


/**
****************************************************************************************************

  @brief Write object to stream.

  The eObject::write() function writes object with class information, attachments, etc to
  the stream.
  
  @param  stream The stream to write to.
  @param  sflags Serialization flags. EOBJ_SERIALIZE_DEFAULT

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eObject::write(
    eStream *stream, 
    os_int sflags) 
{
    eObject *child;
    os_long n_attachements;

    /* Write class identifier, object identifier and persistant object flags.
     */
    if (*stream << classid()) goto failed;
    if (*stream << oid()) goto failed;
    if (*stream << flags() & (EOBJ_SERIALIZATION_MASK)) goto failed;

    /* Calculate and write number of attachments.
       HERE WE SHOULD USE HANDLES FOR SPEED
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
       HERE WE SHOULD USE HANDLES FOR SPEED
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
  @param  sflags Serialization flags. EOBJ_SERIALIZE_DEFAULT

  @return If successfull the function returns pointer to te new child object. 
          If reading object from stream fails, value OS_NULL is returned. 

****************************************************************************************************
*/
eObject *eObject::read(
    eStream *stream, 
    os_int sflags)
{
    os_int cid, oid, oflags;
    os_long n_attachements, i;
    eObject *child;

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
