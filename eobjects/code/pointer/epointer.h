/**

  @file    epointer.h
  @brief   Automatic object pointers.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    14.5.2015

  Automatic object pointer is a pointer which detects if object pointing to it is deleted.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EPOINTER_INCLUDED
#define EPOINTER_INCLUDED

/* Undefined pointer or index/use count reference to object.
 */
typedef union
{
    /* Pointer to undefined type (often function pointer).
        */
    void *undef;

    /** Reference to object. Object index and use count.
     */
    struct 
    {
        e_oix oix;
        os_int ucnt;
    }
    ref;
}
ePointerRef;


/**
****************************************************************************************************

  @brief Object pointer class.

  X..

****************************************************************************************************
*/
class ePointer : public eObject
{
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/
public:
	/* Constructor.
		*/
	ePointer(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
		*/
	virtual ~ePointer();

    /* Casting eObject pointer to ePointer pointer.
     */
	inline static ePointer *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_POINTER)
		return (ePointer*)o;
	}

	/* Get class identifier.
 		*/
	virtual os_int classid() 
    { 
        return ECLASSID_POINTER; 
    }

	/* Static constructor function.
	*/
	/* static ePointer *newobj(
		eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
		return new ePointer(parent, oid, flags);
	} */

    /*@}*/


	/** 
	************************************************************************************************

      @name ePointer specific functions

	  X...

	************************************************************************************************
	*/
	/*@{*/

    /* Set object pointer.
     */
	void set(eObject *ptr);

    /* Get object referred to by object pointer.
     */
    eObject *get();

    /* Set plain pointer.
     */
	inline void set_undef(
        void *ptr) 
    {
        m_ref.undef = ptr;
    }

    /* Get plain pointer.
     */
	inline void *get_undef()
    {
        return m_ref.undef;
    }
   
    /*@}*/

protected:
    /* Undefined pointer or index/use count reference to object.
     */
    ePointerRef m_ref;
};

#endif
