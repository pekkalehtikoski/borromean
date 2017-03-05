/**

  @file    eterrain.h
  @brief   Earth crust or other walk on surface.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    13.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ETERRAIN_INCLUDED
#define ETERRAIN_INCLUDED

/**
****************************************************************************************************

  @brief Terrain class.

  The eTerrain3D is class...

****************************************************************************************************
*/
class eTerrain3D : public eObject3D
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
		eTerrain3D(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);

		/* Virtual destructor.
		 */
		virtual ~eTerrain3D();

        /* Casting eObject pointer to eModel3D pointer.
         */
		inline static eTerrain3D *cast(
			eObject *o) 
		{ 
            e_assert_type(o, EGUICLASSID_TERRAIN3D)
            return (eTerrain3D*)o;
		}

        /* Get class identifier.
         */
        virtual os_int getclassid()
        {
            return EGUICLASSID_MESH3D;
        }

        /* Static function to add class to propertysets and class list.
         */
        static void setupclass();

		/* Static constructor function.
		*/
		static eTerrain3D *newobj(
			eObject *parent,
			os_int oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eTerrain3D(parent, oid, flags);
		}

    /*@}*/

	/** 
	************************************************************************************************

	  @name Terrain functions

	************************************************************************************************
	*/
	/*@{*/

        /* Connect terrain to world.
         */
        void connect_to_world();

        void generate_elev_map(
            os_int sz,
            os_double res,
            os_float variation_m,
            os_float dampening,
            os_float adjust_1_m);

        void steep_banks();

        void avoid_square_island(
            os_double drop_m,
            os_int limit_dist);

        void above_sea_level(
            os_double above_precents,
            os_double step_m);

        void blur(
            os_int blur_r);

        void generate_mesh(
            eContainer *meshes,
            os_double red,
            os_double green,
            os_double blue);

        /* Get elevation at specific point of terrain.
         */
        os_double get_elevation_at(
            os_double x, 
            os_double y);

        bool collides_with_terrain(
            eObject3D *o /* ,
            glm::dvec3 *collision_point */);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Terrain parameters

	************************************************************************************************
	*/
	/*@{*/

    /* ePointer to eWorld'd m_terrains container.
     */
    ePointer *m_world_connection;

    /* X resolution, square with, m.
     */
    double m_res_x;

    /* Y resolution, square height, m.
     */
    double m_res_y;

    /* Width in vertices (squares + 1). 
     */
    int m_w;

    /* Height in vertices (squares + 1). 
     */
    int m_h;

    /* Eletion map, m (w * h items)
     */
    float *m_elev;

    /* Elevation map size, bytes.
     */
    int m_elev_sz;

    /*@}*/
};

#endif
