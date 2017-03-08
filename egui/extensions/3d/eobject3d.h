/**

  @file    eobject3d.h
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
#ifndef EOBJECT3D_INCLUDED
#define EOBJECT3D_INCLUDED

/* 3D object flags.
 */
#define EOBJ3D_DEFAULT 0
#define EOBJ3D_OBJECT_ROOT 1
#define EOBJ3D_CAMERA 2
#define EOBJ3D_LIGHT 4
#define EOBJ3D_WORLD 8
#define EOBJ3D_MOVING 16
#define EOBJ3D_MOUSE_CLICK 32

/* Import flags.
 */
#define EOBJ3D_FLIP_YZ 11
#define EOBJ3D_BLENDER_DEFAULT_ORIENTATION EOBJ3D_FLIP_YZ


typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} 
eKeyboardCtrl;

class eWorld3D;
class eMesh3D;


/**
****************************************************************************************************

  @brief 3D object base class.

  The eObject3D is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eObject3D : public eObject
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
		eObject3D(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);


		/* Virtual destructor.
		 */
		virtual ~eObject3D();

        /* Clone object.
         */
        virtual eObject *clone(
            eObject *parent,
            e_oid id = EOID_CHILD,
            os_int aflags = 0);

        /* Casting eObject pointer to eObject3D pointer.
         */
		inline static eObject3D *cast(
			eObject *o) 
		{ 
            return (eObject3D*)o;
		}

        /* Get class identifier.
        */
        virtual os_int getclassid()
        {
            return EGUICLASSID_OBJECT3D;
        }

        /* Static function to add class to propertysets and class list.
         */
        /* static void setupclass(); */

		/* Static constructor function.
		*/
		static eObject3D *newobj(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eObject3D(parent, oid, flags);
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
        void import(
            const os_char *path,
            eContainer *meshes,
            os_int flags);

        virtual void update_game(
            eWorld3D *world,
            eKeyboardCtrl *kbctrl,
            os_double elapsed);

        /* Calculate m_world_mtx and m_local_mtx to position this object into the world.
         */
        void calculate_world_mtx(
            eObject3D *camera);

        /* Calculate m_inv_world_mtx.
         */
        void calculate_inverse_world_mtx();

        /* List objects with which this object collides.
         */
        virtual void list_collisions(
            eWorld3D *world,
            eContainer *collitions);

        /* Calculate bounding box in mesh coordinates.
         */
        void calculate_bounding_box_mesh();

        /* Calculate bounding box in world coordinates.
         */
        void calculate_bounding_box_world();

        /* Calculate bounding box in camera coordinates.
         */
        void calculate_bounding_box_camera(
            eObject3D *camera);

/*        virtual eMesh3D *get_mesh();

        virtual void set_mesh(
            eMesh3D *mesh); */

    /*@}*/

	/** 
	************************************************************************************************

	  @name Generic properties

	  Current object position, speed, rotation, rotational speed, scale.

	************************************************************************************************
	*/
	/*@{*/
        /* Additional flags for flags() function. By default non moving object, like example 
           terrain can be made moving in most ways by setting EOBJ3D_MOVING flag.
         */
        os_int m_flags;

        /* Position relative to parent, unit m.
         */
        glm::dvec3 m_position;

        /* Rotation, radians.
         */
        glm::dvec3 m_rotate;

        /* Scale, coefficient. 1.0 is no scaling.
         */
        os_double m_scale;

        /* Previous rotation, used to check if rotation matrices need to be recalculated.
         */
        glm::dvec3 m_prev_rotation;

        /* velocity relative to parent, unit m/s.
         */
        glm::dvec3 m_velocity;

        /* Acceleration relative to parent, unit m/s2.
         */
        glm::dvec3 m_acceleration;

        /* Rotation, radians/s.
         */
        glm::dvec3 m_angular_velocity;

        /* Growth rate (change of scale), relative. 0.0 = stay the same. 0.1 = grow 10% per second,
           -0.20 = shrink 20% per second.
         */
        os_double m_growth_rate;
        
        /* Rotation matrix.
         */
        glm::dmat4 m_rotation_x_mtx;
        glm::dmat4 m_rotation_y_mtx;
        glm::dmat4 m_rotation_z_mtx;
        glm::dmat4 m_rotation_mtx;

        /* Transformation of content to coordinates of the parent 3d object. 
         */
        glm::dmat4 m_local_mtx;

        /* Transformation of normals content to coordinates system of the parent 3d object. 
         */
        glm::dmat4 m_norm_local_mtx;

        /* Transformation of content to world coordinates. 
         */
        glm::dmat4 m_world_mtx;

        /* Inverse of m_wold_mtx, usually for camera.
         */
        glm::dmat4 m_inv_word_mtx;

        /* Transformation of normals to world coordinate space. 
         */
        glm::dmat4 m_norm_world_mtx;

        /** Pointer to the first mesh for this object.
         */
        eMesh3D *m_first_mesh;

        /* Bounding box in mesh coordinates.
         */
        glm::fvec4 m_bb_1_mesh;
        glm::fvec4 m_bb_2_mesh;
        bool m_bb_ok_mesh;       
    
        /* Bounding box in world coordinates.
         */
        glm::fvec4 m_bb_1_world;
        glm::fvec4 m_bb_2_world;
        bool m_bb_ok_world;

        /* Bounding box in camera coordinates.
         */
        glm::fvec4 m_bb_1_cam;
        glm::fvec4 m_bb_2_cam; 
        bool m_bb_ok_cam;

    /*@}*/
};

#endif
