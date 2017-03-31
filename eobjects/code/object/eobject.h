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
class ePointer;

/* Flags for message()
 */
#define EMSG_DEFAULT 0
#define EMSG_NO_REPLIES 1
#define EMSG_KEEP_CONTENT 0
#define EMSG_KEEP_CONTEXT 0
#define EMSG_NO_RESOLVE 2
#define EMSG_NO_NEW_SOURCE_OIX 4
#define EMSG_NO_ERRORS 8
#define EMSG_INTERTHREAD 16 /* Message has been passed from thread to another */
#define EMSG_DEL_CONTENT 128
#define EMSG_DEL_CONTEXT 256
#define EMSG_CAN_BE_ADOPTED 512 /* Internal: True if envelope or message can be adopted */
#define EMSG_HAS_CONTENT 2 /* Special flag to be passed over connection only */
#define EMSG_HAS_CONTEXT 4 /* Special flag to be passed over connection only */


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

/* Serialization flags eObject::write(), eObject::read() and clonegeeric() functions.
 */
#define EOBJ_SERIALIZE_DEFAULT 0

/** Flags for json_indent()
 */
#if E_SUPPROT_JSON
#define EJSON_NO_NEW_LINE 0
#define EJSON_NEW_LINE_BEFORE 1
#define EJSON_NEW_LINE_ONLY 2 
#endif


/* Name space identifiers as static strings. eobj_this_ns is default
   for ns_first and ns_firstv functions()
 */
extern os_char eobj_process_ns[];
extern os_char eobj_thread_ns[];
extern os_char eobj_parent_ns[];
extern os_char eobj_this_ns[];


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
    friend class ePointer;

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
        e_oid id = EOID_ITEM,
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
        e_oid id,
	    os_int flags);

public:
/*     void* operator new(size_t sz)
    {
        size_t *p;
        sz += sizeof(size_t);
        p = (size_t*)os_malloc(sz, OS_NULL);
        *p = sz;
        return p + 1;
    }

    void operator delete(void *p)
    {
        if (p)
        {
            size_t *q = (size_t*)p - 1;
            os_free(q, *q);
        }
    }
*/

    /* Delete eObject, virtual destructor.
     */
    virtual ~eObject();

    /** Cloning, adopting and copying.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
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

    /* Return OS_TRUE if object is thread (derived). 
     */
    virtual os_boolean isthread() 
    {
        return OS_FALSE;
    }

    /* Allocate new child object by class identifier.
     */
    inline eObject *newchild( 
        os_int cid,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return newobject(this, cid, id, flags);
    }

    /* Allocate new object by class identifier.
     */
    static eObject *newobject(
        eObject *parent,
        os_int cid,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

    inline eHandle *handle()
    {
        return mm_handle;
    }

    /* Enable or disable timer for this object.
     */
    void timer(
        os_long period_ms);

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

    /* Overloaded new operator calls os_malloc().
     */
    void* operator new(
        size_t);

    /* Overloaded delete operator calls os_free().
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
		return EOID_ITEM;
    }

    /* Recommended size for oixstr() buffer.
     */
    #define E_OIXSTR_BUF_SZ (2 * OSAL_NBUF_SZ+2)

    /** Convert oix and ucnt to string.
     */
    void oixstr(
        os_char *buf, 
        os_memsz bufsz);

    /** Get oix and ucnt from string.
     */
    os_short oixparse(
        os_char *str,
        e_oix *oix, 
        os_int *ucnt);

    /* Get number of childern.
     */
	inline os_long childcount(
        e_oid id = EOID_CHILD)
	{
        if (mm_handle) return mm_handle->childcount(id);
		return 0;
	}

    /** Get parent object of tis object.
     */
    inline eObject *parent() 
	{
        return mm_parent;
        /* if (mm_handle)
        {
            eHandle *h;
            h = mm_handle->parent();
            if (h) return h->m_object;
        }
        return OS_NULL; */
	}

    /** Get grandparent of this object.
     */
    inline eObject *grandparent() 
	{
        if (mm_parent)
        {
            return mm_parent->mm_parent;
        }
		return OS_NULL;
        /* if (mm_handle)
        {
            eHandle *h;
            h = mm_handle->grandparent();
            if (h) return h->m_object;
        }
        return OS_NULL; */
    }


    /** Get thread object.
     */
    eThread *thread();

	/* Get the first child object identified by oid.
     */
	eObject *first(
        e_oid id = EOID_CHILD);

	/* Get the first child variable identified by oid.
     */
    eVariable *firstv(
        e_oid id = EOID_CHILD);

	/* Get the first child container identified by oid.
     */
    eContainer *firstc(
        e_oid id = EOID_CHILD);

	/* Get the first child name identified by oid.
     */
    eName *firstn(
        e_oid id = EOID_NAME);

	/* Get last child object identified by oid.
     */
	eObject *last(
        e_oid id = EOID_CHILD);

	/* Get next object identified by oid.
     */
	eObject *next(
        e_oid id = EOID_CHILD);

	/* Get previous object identified by oid.
     */
	eObject *prev(
        e_oid id = EOID_CHILD);

    /** Adopting object as child of this object.
     */
	void adopt(
		eObject *child,
        e_oid id = EOID_CHILD,
		os_int aflags = 0);

    void adoptat(
        eObject *beforethis, 
        e_oid id = EOID_CHILD);

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
        os_int sflags);

    /* Called by write() to write class specific object content.
        The eObject base class provides just dummy place holder
        function for classes which do not need serialization.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int sflags) 
    {
        osal_debug_error("serialization failed, writer not implemented");
        return ESTATUS_FAILED;
    }

    /* Called by read() to read class specific object content.
        The eObject base class provides just dummy place holder
        function for classes which do not need serialization.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int sflags) 
    {
        osal_debug_error("serialization failed, reader not implemented");
        return ESTATUS_FAILED;
    }

    /*@}*/

#if E_SUPPROT_JSON
    /** 
    ************************************************************************************************

      @name Sopport JSON format object serialization. 

      This includes reading and writing object as from/to stream as JSON. Object can be desccribed
      as valus of it's properties and it's children.

    ************************************************************************************************
    */
    /*@{*/

    /* Write object to stream as JSON.
     */
    eStatus json_write(
        eStream *stream, 
        os_int sflags = EOBJ_SERIALIZE_DEFAULT,
        os_int indent = -1,
        os_boolean *comma = OS_NULL);

    /* Read object from JSON stream.
     */
    eObject *json_read(
        eStream *stream, 
        os_int sflags = EOBJ_SERIALIZE_DEFAULT);

    /* Class specific part of JSON writer.
     */
    virtual eStatus json_writer(
        eStream *stream, 
        os_int sflags = EOBJ_SERIALIZE_DEFAULT,
        os_int indent = 0)
    {
        return ESTATUS_SUCCESS;
    }

#endif

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
        const os_char *namespace_id = OS_NULL);

	/* Delete this object's name space.
     */
	void ns_delete();

	/* Find eName by name and name space.
     */
	eName *ns_first(
        const os_char *name = OS_NULL,
        const os_char *namespace_id = eobj_this_ns);

    /* Find eName by name and name space.
     */
    eName *ns_firstv(
        eVariable *name = OS_NULL,
        const os_char *namespace_id = eobj_this_ns);

    /* Find object by name.
     */
    eObject *ns_get(
        const os_char *name,
        const os_char *namespace_id = eobj_this_ns,
        os_int cid = ECLASSID_OBJECT);

    eVariable *ns_getv(
        const os_char *name,
        const os_char *namespace_id = eobj_this_ns);

    eContainer *ns_getc(
        const os_char *name,
        const os_char *namespace_id = eobj_this_ns);

    /* Info bits for findnamespace().
     */
    #define E_INFO_PROCES_NS 1
    #define E_INFO_ABOVE_CHECKPOINT 2

    /* Find name space by name space ID. 
     */
	eNameSpace *findnamespace(
        const os_char *namespace_id = OS_NULL,
        os_int *info = OS_NULL,
        eObject *checkpoint = OS_NULL);

	/* Give name to this object.
     */
	eName *addname(
        const os_char *name,
        os_int flags = 0,
        const os_char *namespace_id = OS_NULL);

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

    /* Get object by name.
     */
	eObject *byname(
        const os_char *name);


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
    void message(
        eEnvelope *envelope);

    /* Send message.
     */
    void message(
        os_int command, 
        const os_char *target,
        const os_char *source = OS_NULL,
        eObject *content = OS_NULL,
        os_int mflags = EMSG_DEFAULT,
        eObject *context = OS_NULL);

    virtual void onmessage(
        eEnvelope *envelope);

    /* Set object's property by sending a message.
     */
    void setproperty_msg(
        const os_char *remotepath,
        eObject *x,
        const os_char *propertyname = OS_NULL,
        os_int flags = EMSG_KEEP_CONTENT|EMSG_NO_REPLIES);

    void setpropertyl_msg(
        const os_char *remotepath,
        os_long x,
        const os_char *propertyname = OS_NULL);

    void setpropertyd_msg(
        const os_char *remotepath,
        os_double x,
        const os_char *propertyname = OS_NULL);

    void setpropertys_msg(
        const os_char *remotepath,
        const os_char *x,
        const os_char *propertyname = OS_NULL);

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
        const os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        const os_char *text = OS_NULL);

    /* Add integer property to property set.
     */
    static eVariable *addpropertyl(
        os_int cid, 
        os_int propertynr, 
        const os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        const os_char *text = OS_NULL,
        os_long x = 0);

    /* Add double property to property set.
     */
    static eVariable *addpropertyd(
        os_int cid, 
        os_int propertynr, 
        const os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        const os_char *text = OS_NULL,
        os_double x = 0.0,
        os_int digs = 2);

    /* Add string property to property set.
     */
    static eVariable *addpropertys(
        os_int cid, 
        os_int propertynr, 
        const os_char *propertyname,
        os_int pflags = EPRO_DEFAULT,
        const os_char *text = OS_NULL,
        const os_char *x = OS_NULL);

    /* Property set for class done, complete it.
     */
    static void propertysetdone(
        os_int cid);

    /* Initialize properties to default values.
     */
    void initproperties();

    /* Property name to number.
     */
    os_int propertynr(
        const os_char *propertyname);

    /* Property number to name.
     */
    os_char *propertyname(
        os_int propertynr);

    /* Set property value.
     */
    void setpropertyv(
        os_int propertynr, 
        eVariable *x, 
        eObject *source = OS_NULL, 
        os_int flags = 0);

    /* Forward property change trough bindings.
     */
    void forwardproperty(
        os_int propertynr, 
        eVariable *x, 
        eObject *source, 
        os_int flags);

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
        const os_char *x);

    /* Get property value.
     */
    void propertyv(
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
        const os_char *remotepath,
        const os_char *remoteproperty,
        os_int bflags);

    /* Bind properties, remote property .
     */
    void bind(
        os_int localpropertynr,
        const os_char *remotepath,
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
    virtual eStatus oncallback()
    {
        return ESTATUS_SUCCESS;
    }
    /*@}*/

private:
    void message_within_thread(
        eEnvelope *envelope,
        const os_char *namespace_id);

    void message_process_ns(
        eEnvelope *envelope);

    void message_oix(
        eEnvelope *envelope);

    /* Forward message by object index within thread's object tree.
     */
    void onmessage_oix(
        eEnvelope *envelope);

    /* Create server side of property binding.
     */
    void srvbind(
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

#if E_SUPPROT_JSON
    /* Write optional comma and new line to terminate the 
       line and optional intendation for next line to JSON output.
     */
    eStatus json_indent(
        eStream *stream, 
        os_int indent,
        os_int iflags = EJSON_NEW_LINE_BEFORE,
        os_boolean *comma = OS_NULL);

    /* Write string to JSON output.
     */
    eStatus json_puts(
        eStream *stream, 
        const os_char *str);

    /* Write quoted string to JSON output.
     */
    eStatus json_putqs(
        eStream *stream, 
        const os_char *str);

    /* Write long integer to JSON output.
     */
    eStatus json_putl(
        eStream *stream, 
        os_long x);

    /* Write variable value to JSON output.
     */
    eStatus json_putv(
        eStream *stream, 
        eVariable *p,
        eVariable *value,
        os_int sflags,
        os_int indent);

    /* Append list item string to variable if bit is set in flags.
     */
    void json_append_list_item(
        eVariable *list, 
        const os_char *item,
        os_int flags,
        os_int bit);
#endif

	/* Pointer to object's handle.
     */
	eHandle *mm_handle;

    /** Pointer to parent object of this object. (THIS SHOULD PERHAPS MOVE TO OBJ)
     */
    eObject *mm_parent;

};

#endif



