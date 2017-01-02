/**

  @file    eobject.h
  @brief   Object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The eObject is base class for all objects. 
  
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
#ifndef EOBJECT_INCLUDED
#define EOBJECT_INCLUDED

class eHandle;
class eRoot;
class eVariable;
class eContainer;
class eNameSpace;
class eName;
class eStream;
class eEnvelope;
class eThread;

/* Flags for message()
 */
#define EMGS_NO_REPLIES 1
#define EMSG_KEEP_CONTENT 0
#define EMSG_NO_RESOLVE 2
#define EMSG_NO_NEW_SOURCE_OIX 4
#define EMSG_DEL_CONTENT 128
#define EMSG_DEL_CONTEXT 256

/* Macro to debug object type casts.
 */
#if OSAL_DEBUG == 0
  #define e_assert_type(o,id)
#else
  #define e_assert_type(o,id) if (o) osal_debug_assert((o)->classid()==(id));
#endif 


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.

****************************************************************************************************
*/
class eObject
{
	friend class eHandle;
	friend class eRoot;

    /** 
    ************************************************************************************************

      @name Constructors and destructor

      When object is constructed it may be placed within thread's. 

    ************************************************************************************************
    */
    /*@{*/

protected:
	/* Private constructor for new eObject.
     */
    eObject(
        eObject *parent = OS_NULL,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

private:
	/* Disable C++ standard copy constructor and assignment operator. Assignment is implemnted
		class specifically, and copy as clone() function which also positions object in object
		tree.
     */
	eObject(eObject const&);
	eObject& operator=(eObject const&);
public:

    /* Delete eObject, virtual destructor.
     */
    virtual ~eObject();

    /** Cloning, adopting and copying.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD);

    /* Get class identifier
     */
    virtual os_int classid() {return ECLASSID_OBJECT;}

    /* Allocate new child object by class identifier.
     */
    eObject *newchild(
        os_int cid,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

    inline eHandle *handle()
    {
        return mm_handle;
    }

    /*@}*/


#if EOVERLOAD_NEW_AND_DELETE
    /** 
    ************************************************************************************************

      @name Memory allocation

      Memory for objects is allocated by overloaded new and delete operators. These map the
      memory allocation to OSAL memory management.

    ************************************************************************************************
    */
    /*@{*/

    /* Overloaded new operator calls osal_memory_allocate().
     */
    void* operator new(
        size_t);

    /* Overloaded delete operator calls osal_memory_free().
     */
    void operator delete(
        void *buf); 

    /*@}*/
#endif

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
		if (mm_handle) return mm_handle->m_oflags;
		return EOBJ_DEFAULT;
    }

    /** Set specified object flags.
     */
    inline void setflags(
		os_int flags)
    {
		if (mm_handle) mm_handle->setflags(flags);
    }

    /** Clear specified object flags.
     */
    inline void clearflags(
		os_int flags)
    {
		if (mm_handle) mm_handle->clearflags(flags);
    }

    /** If object can be cloned?
     */
    inline os_boolean isclonable()
    {
		if (mm_handle) return mm_handle->isclonable();
		return OS_TRUE;
    }

    /** Check if object is an attachment. Returns nonzero if object is an attachment.
     */
    inline os_boolean isattachment()
    {
		if (mm_handle) return mm_handle->isattachment();
		return OS_FALSE;
    }

    /** Check if object is a serializable attachment. 
     */
    inline os_boolean isserattachment()
    {
		if (mm_handle) return mm_handle->isserattachment();
		return OS_FALSE;
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
		if (mm_handle) return mm_handle->oid();
		return 0;
    }

    /* Get number of childern.
     */
	inline os_long childcount(
		e_oid oid = EOID_CHILD)
	{
		if (mm_handle) return mm_handle->childcount(oid);
		return 0;
	}

    /** Move trough object hirarcy.
     */
    inline eObject *parent() 
	{
		if (mm_handle) 
        {
            eHandle *h;
            h = mm_handle->parent();
            if (h) return h->m_object;
        }
		return OS_NULL;
	}

    /** Get thread object.
     */
    eThread *thread();

	/* Get the first child object identified by oid.
     */
	eObject *first(
		e_oid oid = EOID_CHILD);

	/* Get the first child variable identified by oid.
     */
    eVariable *firstv(
	    e_oid oid = EOID_CHILD);

	/* Get the first child container identified by oid.
     */
    eContainer *firstc(
	    e_oid oid = EOID_CHILD);

	/* Get the first child name identified by oid.
     */
    eName *firstn(
	    e_oid oid = EOID_CHILD);

	/* Get last child object identified by oid.
     */
	eObject *last(
		e_oid oid = EOID_CHILD);

	/* Get next object identified by oid.
     */
	eObject *next(
		e_oid oid = EOID_CHILD);

	/* Get previous object identified by oid.
     */
	eObject *prev(
		e_oid oid = EOID_CHILD);

    /* Flags for adopt function.
     */
    #define EOBJ_BEFORE_THIS 1
    #define EOBJ_NO_MAP 2

    /** Adopting object as child of this object.
     */
	void adopt(
		eObject *child,
		e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    void adoptat(
        eObject *beforethis, 
        e_oid oid = EOID_CHILD);

    /* Verify whole object tree.
     */
#if EOBJECT_DBTREE_DEBUG
    inline void verify_whole_tree() {if (mm_handle) mm_handle->verify_whole_tree();}
#endif


    /*@}*/


    /** 
    ************************************************************************************************

      @name Serialization

      Writing object to stream or reading it from strem. Application should use read() and write
      functions to read and write objects with additional information. writer() and reader()
      are class specific implementations for class content only.

    ************************************************************************************************
    */
    /*@{*/

    /* Write object to stream.
     */
    eStatus write(
        eStream *stream, 
        os_int flags);

    /* Read object from stream as new child object.
     */
    eObject *read(
        eStream *stream, 
        os_int flags);

    /* Called by write() to write class specific object content.
        The eObject base class provides just dummy place holder
        function for classes which do not need serialization.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int sflags) 
    {
        return ESTATUS_SUCCESS;
    }

    /* Called by read() to read class specific object content.
        The eObject base class provides just dummy place holder
        function for classes which do not need serialization.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int sflags) 
    {
        return ESTATUS_SUCCESS;
    }

    /*@}*/


    /** 
    ************************************************************************************************

      @name Names and namespaces.

      The name and name space functionality is based on eNameSpace and eName classes.

    ************************************************************************************************
    */
    /*@{*/

    /* Create name space for this object.
     */
	void ns_create(
		os_char *namespace_id = OS_NULL);

	/* Delete this object's name space.
     */
	void ns_delete();

	/* Find eName by name and name space.
     */
	eName *ns_first(
        os_char *name = OS_NULL,
        os_char *namespace_id = OS_NULL); 

    /* Find eName by name and name space.
     */
    eName *ns_firstv(
        eVariable *name = OS_NULL,
        os_char *namespace_id = OS_NULL);

    /* Find object by name.
     */
    eObject *ns_get(
        os_char *name,
        os_char *namespace_id = OS_NULL,
        os_int classid = ECLASSID_OBJECT);

    eVariable *ns_getv(
        os_char *name,
        os_char *namespace_id = OS_NULL);

    eContainer *ns_getc(
        os_char *name,
        os_char *namespace_id = OS_NULL);

    /* Info bits for findnamespace().
     */
    #define E_INFO_PROCES_NS 1
    #define E_INFO_ABOVE_CHECKPOINT 2

    /* Find name space by name space ID. 
     */
	eNameSpace *findnamespace(
		os_char *namespace_id = OS_NULL,
        os_int *info = OS_NULL,
        eObject *checkpoint = OS_NULL);

	/* Give name to this object.
     */
	eName *addname(
	    os_char *name,
        os_int flags = 0,
	    os_char *namespace_id = OS_NULL);

    /* Flags for map() function: Attach all names of child object (this) and it's childen to 
       name spaces. If a name is already mapped, it is not remapped.
    */
    #define E_ATTACH_NAMES 1

    /* Flags for map() function: Copy m_root pointer (pointer to eRoot of a tree structure)
       from child object (this) to all child objects of it.
    */
    #define E_SET_ROOT_POINTER 8

    /* Flags for map() function: Detach names of child object (this) and it's childen from name 
       spaces above this object in tree structure.
      */
    #define E_DETACH_FROM_NAMESPACES_ABOVE 16

    /* Attach/detach names in tree sturcture to name spaces. Set eRoot pointers.
     */
    void map(os_int mflags);

    /*@}*/


    /** 
    ************************************************************************************************

      @name Messages

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/

    /* Send message.
     */
    void message(eEnvelope *envelope);

    /* Send message.
     */
    void message(
        os_int command, 
        os_char *target,
        os_char *source = OS_NULL,
        eObject *content = OS_NULL,
        os_int mflags = EMSG_DEL_CONTENT,
        eObject *context = OS_NULL);

    virtual eStatus onmessage(
        eEnvelope *envelope);


    /*@}*/


    /** 
    ************************************************************************************************

      @name Object properties

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/

    /* Set property value.
     */
    eStatus setproperty(
        os_char *propertyname, 
        eVariable *variable, 
        eObject *source, 
        os_int flags);

    eStatus getproperty(
        os_char *propertyname, 
        eVariable *variable, 
        os_int flags);

    virtual eStatus onpropertychange(
        os_int propertynr, 
        eVariable *variable, 
        os_int flags) {return ESTATUS_SUCCESS;}

    /*@}*/

    /** 
    ************************************************************************************************

      @name Binding

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/
    /* Binding
     */
    void bind();
    virtual void onmessagefrombinding() {}
    void messagetobinding();
    /*@}*/

    /** 
    ************************************************************************************************

      @name Child callback

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/
    virtual eStatus oncallback() {return ESTATUS_SUCCESS;}
    /*@}*/

private:
    void process_ns_message(
        eEnvelope *envelope);

protected:
	/* Delete all child objects.
     */
    void delete_children();

	/* Red/Black tree: Insert a node to red black tree.
     */
    /* inline void rbtree_insert(
        eObject *inserted_node)
	{
		if (mm_handle && inserted_node) 
			mm_handle->rbtree_insert(inserted_node->mm_handle);
	} */

	/* Red/Black tree: Remove node from red/black.
     */
    /* inline void rbtree_remove(
        eObject *n)
	{
		if (mm_handle) if (mm_handle->m_parent)
			mm_handle->m_parent->rbtree_remove(mm_handle);
	} */

    void map2(
        eHandle *handle,
        os_int mflags);

    void mapone(
        eHandle *handle, 
        os_int mflags);

	/** Root of the object of the index tree.
     */
	eRoot *mm_root; 

	/* Pointer to object's handle.
     */
	eHandle *mm_handle;
};

#endif



