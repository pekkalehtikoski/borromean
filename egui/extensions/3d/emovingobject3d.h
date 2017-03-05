/**

  @file    emovingbject3d.h
  @brief   3D object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2016

  Base class for objects in 3D object tree. Like "car" could be child of world and "car door" 
  child of car. 
  - Holds current object position, speed, rotation, rotational speed, scale.
  - References to mesh which contains vertices.
  - References to code related to object?

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EMOVINGOBJECT3D_INCLUDED
#define EMOVINGOBJECT3D_INCLUDED

/**
****************************************************************************************************

  @brief 3D object base class.

  The eMovingObject3D is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eMovingObject3D : public eObject3D
{
	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
        /* Constructor.
		 */
		eMovingObject3D(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);

        /* Virtual destructor.
		 */
		virtual ~eMovingObject3D();

        /* Casting eObject pointer to eMovingObject3D pointer.
         */
		inline static eMovingObject3D *cast(
			eObject *o) 
		{ 
            e_assert_type(o, EGUICLASSID_MOVINGOBJECT3D)
            return (eMovingObject3D*)o;
		}

		/* Get class identifier.
         */
        virtual os_int getclassid()
        {
            return EGUICLASSID_MOVINGOBJECT3D;
        }

        /* Static function to add class to propertysets and class list.
         */
        static void setupclass();

		/* Static constructor function for generating instance by class list.
         */
		static eMovingObject3D *newobj(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eMovingObject3D(parent, oid, flags);
		}

        /* Get object flags, like is object camera, etc.
         */
		virtual os_int flags();

    /*@}*/


	/** 
	************************************************************************************************

	  @name Other functions

	  Current object position, speed, rotation, rotational speed, scale.

	************************************************************************************************
	*/
	/*@{*/
        virtual void update_game(
            eWorld3D *world,
            eKeyboardCtrl *kbctrl,
            os_double elapsed);
    /*@}*/

	/** 
	************************************************************************************************

	  @name Generic properties

	  Current object position, speed, rotation, rotational speed, scale.

	************************************************************************************************
	*/
	/*@{*/


    /*@}*/
};

#endif
