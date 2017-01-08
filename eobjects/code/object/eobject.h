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
#define EMSG_NO_ERROR_MSGS 8
#define EMSG_INTERTHREAD 16 /* Message has been passed from thread to another */
#define EMSG_DEL_CONTENT 128
#define EMSG_DEL_CONTEXT 256

/* Macro to debug object type casts.
 */
#if OSAL_DEBUG == 0
  #define e_assert_type(o,id)
#else
  #define e_assert_type(o,id) if (o) osal_debug_assert((o)->classid()==(id));
#endif 

/* Flags for addproperty() function.
 */
#define EPRO_DEFAULT 0
#define EPRO_PERSISTENT EOBJ_CUST_FLAG1
#define EPRO_METADATA EOBJ_CUST_FLAG2
#define EPRO_SIMPLE EOBJ_CUST_FLAG3
#define EPRO_NOONPRCH EOBJ_CUST_FLAG4
#define EPRO_NOPACK EOBJ_CUST_FLAG5

/* Flags for adopt(), clone() and clonegeeric() functions.
    */
#define EOBJ_BEFORE_THIS 1
#define EOBJ_NO_MAP 2
#define EOBJ_CLONE_ALL_CHILDREN 4

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

    /* Create root helper object and handles for root and root helper.
     */
    void makeroot(
        e_oid oid,
	    os_int flags);

public:

    /* Delete eObject, virtual destructor.
     */
    virtual ~eObject();

    /** Cloning, adopting and copying.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    /* Helper function for clone functionality.
     */
    void clonegeneric(
        eObject *clonedobj,
        os_int aflags);

    /* Get class identifier
     */
    virtual os_int classid() 
    {
        return ECLASSID_OBJECT;
    }

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

    /* Recommended size for oixstr() buffer.
     */
    #define E_OEXSTR_BUF_SZ (2 * OSAL_NBUF_SZ+2)

    /** Convert oix and ucnt to string.
     */
    void oixstr(
        os_char *buf, 
        os_memsz bufsz);

    /** Get oix and ucnt from string.
     */
    os_short eObject::oixparse(
        os_char *str,
        e_oix *oix, 
        os_int *ucnt);

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
        os_int cid = ECLASSID_OBJECT);

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

    virtual void onmessage(
        eEnvelope *envelope);

    /* Set object's property by sending a message.
     */
    void setproperty_msg(
        os_char *remotepath,
        eObject *x,
        os_int flags);

    void setpropertyd_msg(
        os_char *remotepath,
        os_double x);

    /*@}*/


    /** 
    ************************************************************************************************

      @name Object properties

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/

    /* Add property to property set (any type).
     */
    static eVariable *addproperty(
        os_int cid, 
        os_int propertynr, 
        os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        os_char *text = OS_NULL);

    /* Add integer property to property set.
     */
    static eVariable *addpropertyl(
        os_int cid, 
        os_int propertynr, 
        os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        os_char *text = OS_NULL,
        os_long x = 0);

    /* Add double property to property set.
     */
    static eVariable *addpropertyd(
        os_int cid, 
        os_int propertynr, 
        os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        os_char *text = OS_NULL,
        os_double x = 0.0,
        os_int digs = 2);

    /* Add string property to property set.
     */
    static eVariable *addpropertys(
        os_int cid, 
        os_int propertynr, 
        os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        os_char *text = OS_NULL,
        os_char *x = OS_NULL);

    /* Initialize properties to default values.
     */
    void initproperties();

    /* Property name to number.
     */
    os_int propertynr(
        os_char *propertyname);

    /* Property number to name.
     */
    void propertyname(os_int propertynr,
        os_char buf,
        os_memsz bufsz);

    /* Set property value.
     */
    void setproperty(
        os_int propertynr, 
        eVariable *x, 
        eObject *source = OS_NULL, 
        os_int flags = 0);

    /* Set property value as integer.
     */
    void setpropertyl(
        os_int propertynr, 
        os_long x);

    /* Set property value as double.
     */
    void setpropertyd(
        os_int propertynr, 
        os_double x);

    /* Set property value as string.
     */
    void setpropertys(
        os_int propertynr, 
        os_char *x);

    /* Get property value.
     */
    void property(
        os_int propertynr, 
        eVariable *x, 
        os_int flags = 0);

    os_long propertyl(
        os_int propertynr);

    os_double propertyd(
        os_int propertynr);

    /* Called when property value changes.
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags) {}

    /* Get value of simple property.
     */
    virtual eStatus simpleproperty(
        os_int propertynr, 
        eVariable *x) 
    {
        return ESTATUS_NO_SIMPLE_PROPERTY_NR;
    }

    /*@}*/

    /** 
    ************************************************************************************************

      @name Binding

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/

    /* Bind properties.
     */
    void bind(
        os_int localpropertynr,
        os_char *remotepath,
        os_char *remoteproperty,
        os_int bflags);

    /* Bind properties, remote property .
     */
    void bind(
        os_int localpropertynr,
        os_char *remotepath,
        os_int bflags);


    /* virtual void onmessagefrombinding() {}
    void messagetobinding(); */
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
    void message_within_thread(
        eEnvelope *envelope,
        os_char *namespace_id);

    void message_process_ns(
        eEnvelope *envelope);

    void message_oix(
        eEnvelope *envelope);

protected:
	/* Delete all child objects.
     */
    void delete_children();

    void map2(
        eHandle *handle,
        os_int mflags);

    void mapone(
        eHandle *handle, 
        os_int mflags);

	/* Pointer to object's handle.
     */
	eHandle *mm_handle;
};

#endif



