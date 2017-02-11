/**

  @file    dragoworld.h
  @brief   World and all it contains.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    9.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef DRAGOWORLD_INCLUDED
#define DRAGOWORLD_INCLUDED

class DragoCamera;

#define DRAGO_MAX_CAMERAS 3

/**
****************************************************************************************************

  @brief World class

  X...


****************************************************************************************************
*/
class DragoWorld : public eObject
{
    public:

	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/

		/** Constructor.
		 */
		DragoWorld(
			eObject *parent = OS_NULL,
			os_int oid = EOID_ITEM);

		/* Virtual destructor.
		 */
		virtual ~DragoWorld();

        /* Casting eObject pointer to DragoWorld pointer.
         */
		inline static DragoWorld *cast(
			eObject *o) 
		{ 
			return (DragoWorld*)o;
		}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Other functions

	  X..

	************************************************************************************************
	*/
	/*@{*/

        void upload_mesh_data();

        void update_game(
            eKeyboardCtrl *kbctrl,
            os_long elapsed);

        void calculate_world_mtx(
            os_int camera_nr);

        void draw_world(
            os_int camera_nr);

    /*@}*/

	/** 
	************************************************************************************************

	  @name World data

	  X..

	************************************************************************************************
	*/
	/*@{*/

//        eMesh3D m_box_mesh;

        eWorld3D 
            *m_world;

        eTerrain3D
            *m_mountains,
            *m_plains,
            *m_sea;

        eMovingObject3D
            *m_box,
            *m_box2,
            *m_box3;

        DragoCamera
            *m_camera[DRAGO_MAX_CAMERAS];

        GLuint
            m_vertexShader,
            m_fragmentShader,
            m_shaderProgram,
            m_posAttrib,
            m_colAttrib,
            m_mormalAttrib,
            m_MVP_uniform,
            m_M_uniform,
            m_N_uniform,
            m_LightPos1_uniform,
            m_LightColor1_uniform;
            //m_LightAmbient1_uniform,
            //m_LightDiffuse1_uniform;

        /* Container for loaded meshes.
         */
        eContainer m_meshes;
        
        /* GL buffer objects.
         */
        GLuint m_glbuf[E3GL_NRO_BUFFERS];

    /*@}*/
};

#endif
