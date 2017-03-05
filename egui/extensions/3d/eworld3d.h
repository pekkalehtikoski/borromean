/**

  @file    eworld3d.h
  @brief   3D world.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EWORLD3D_INCLUDED
#define EWORLD3D_INCLUDED


/**
****************************************************************************************************

  @brief 3D object base class.

  The eWorld3D is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eWorld3D : public eObject3D
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
		eWorld3D(
			eObject *parent = OS_NULL,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);

		/* Virtual destructor.
		 */
		virtual ~eWorld3D();

        /* Casting eObject pointer to eWorld3D pointer.
         */
		inline static eWorld3D *cast(
			eObject *o) 
		{ 
            e_assert_type(o, EGUICLASSID_WORLD3D)
            return (eWorld3D*)o;
		}

        /* Get class identifier.
         */
        virtual os_int getclassid()
        {
            return EGUICLASSID_WORLD3D;
        }

        /* Static function to add class to propertysets and class list.
         */
        static void setupclass();

		/* Static constructor function.
		*/
		static eWorld3D *newobj(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eWorld3D(parent, oid, flags);
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

    /* Empty container which holds only references to it for each connected terrain.
     */
    eContainer *m_terrains;

    /*@}*/
};

#endif
