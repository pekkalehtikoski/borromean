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
class eNameSpace;
class eName;
class eStream;


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


    /* Get class identifier
     */
    virtual os_int getclassid() {return ECLASSID_OBJECT;}
       
    /* Allocate new child object by class identifier.
     */
    eObject *newchild(
        os_int classid,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

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
    inline os_int getflags()
    {
		if (mm_handle) return mm_handle->m_oflags;
		return 0;
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


    /** Check if object is an attachment. Returns nonzero if object is an attachment.
     */
    inline os_boolean isattachment()
    {
		if (mm_handle) return isattachment();
		return 0;
    }

    /** Check if object is a serializable attachment. 
     */
    inline os_boolean isserattachment()
    {
		if (mm_handle) return isserattachment();
		return 0;
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
    inline os_int getoid() 
    {
		if (mm_handle) return mm_handle->getoid();
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
    inline eObject *getparent() 
	{
		return mm_parent;
	}

	/* Get first child object identified by oid.
     */
	eObject *getfirst(
		e_oid oid = EOID_CHILD);

	/* Get last child object identified by oid.
     */
	eObject *getlast(
		e_oid oid = EOID_CHILD);

	/* Get next object identified by oid.
     */
	eObject *getnext(
		e_oid oid = EOID_CHILD);

	/* Get previous object identified by oid.
     */
	eObject *getprev(
		e_oid oid = EOID_CHILD);

    /** Adopting object as child of this object.
     */
    void adopt(
        eObject *child, 
        e_oid oid) {}

    eObject *adoptat(
        eObject *beforethis, 
        e_oid oid);

    /** Cloning, adopting and copying.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid) {return 0; }

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
        os_int flags) 
    {
        return ESTATUS_SUCCESS;
    }

    /* Called by read() to read class specific object content.
        The eObject base class provides just dummy place holder
        function for classes which do not need serialization.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags) 
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
	void createnamespace(
		os_char *namespace_id = OS_NULL);

	/* Delete this object's name space.
     */
	void deletenamespace();

    /* Get pointer to name space.
     */
	eNameSpace *getnamespace(
		os_char *namespace_id = OS_NULL);

	/* Give name to this object.
     */
	eName *addname(
		os_char *name,
		os_char *namespace_id = OS_NULL,
		os_int flags = 0);

	/* Get first indexed variable containing the name. 
     */
	/* eName *getfirstname(os_char *namestr); */

    /*@}*/


    /** 
    ************************************************************************************************

      @name Messages

      Any object may send a message to an another object by calling message() function. 
      When an object receives a message, it's onmessage function gets called.

    ************************************************************************************************
    */
    /*@{*/

    /* Messages
     */
    eStatus message();
    virtual eStatus onmessage() {return ESTATUS_SUCCESS;}

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


protected:

	/** Root of the object of the index tree.
     */
	eRoot *mm_root; 

	/* Pointer to object's handle.
     */
	eHandle *mm_handle;

	/** Pointer to parent object of this object.
     */
	eObject *mm_parent;

	/* Delete all child objects.
     */
    void delete_children();

	/* Red/Black tree: Insert a node to red black tree.
     */
    inline void rbtree_insert(
        eObject *inserted_node)
	{
		if (mm_handle && inserted_node) 
			mm_handle->rbtree_insert(inserted_node->mm_handle);
	}

	/* Red/Black tree: Remove node from red/black.
     */
    inline void rbtree_remove(
        eObject *n)
	{
		if (mm_handle && n) 
			mm_handle->rbtree_remove(n->mm_handle);
	}
};

#endif



