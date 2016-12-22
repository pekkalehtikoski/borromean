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

	  @name Constructors, destructor and cast

	  X...

	************************************************************************************************
	*/
	/*@{*/
protected:
		/** Constructor.
		 */
		ePointer(
			eObject *parent = OS_NULL,
			e_oid oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);

public:
		/** Public constructor.
		*/
		inline ePointer() : ePointer(OS_NULL) {}

		/* Virtual destructor.
		 */
		virtual ~ePointer();

        /* Casting eObject pointer to ePointer pointer.
         */
		inline static ePointer *cast(
			eObject *o) 
		{ 
			return (ePointer*)o;
		}

		/* Get class identifier.
 		 */
		virtual os_int getclassid() { return ECLASSID_POINTER; }

		/* Static constructor function.
		*/
		static ePointer *newobj(
			eObject *parent,
			e_oid oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new ePointer(parent, oid, flags);
		}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Other functions

	  X...

	************************************************************************************************
	*/
	/*@{*/

        /* Set automatic pointer.
         */
		void set(eObject *ptr);

        /* Get object referred to by automatic pointer.
         */
        inline eObject *get()
        {
            if (m_my_pair) return m_my_pair->getparent();
            return OS_NULL;
        }
   
    /*@}*/


protected:
        ePointer *m_my_pair;
};

#endif
