/**

  @file    dragocamera.h
  @brief   Camera.
  @author  Sofie Lehtikoski, Pekka Lehtikoski
  @version 1.0
  @date    9.5.2016

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef DRAGOCAMERA_INCLUDED
#define DRAGOCAMERA_INCLUDED

/**
****************************************************************************************************

  @brief Camera class.

  The DragoCamera is class...


****************************************************************************************************
*/
class DragoCamera : public eObject3D
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
		DragoCamera(
			eObject *parent = OS_NULL,
			os_int oid = EOID_ITEM);

		/* Virtual destructor.
		 */
		virtual ~DragoCamera();

        /* Casting eObject pointer to eModel3D pointer.
         */
		inline static DragoCamera *cast(
			eObject *o) 
		{ 
			return (DragoCamera*)o;
		}

        /* Get object flags, like is object camera, etc.
         */
		virtual os_int flags();

    /*@}*/

	/** 
	************************************************************************************************

	  @name Camera functions

	************************************************************************************************
	*/
	/*@{*/

        /* Draw world to camera.
         */
        virtual void draw_world(
            DragoWorld *world);

        /* Draw object recursively.
         */
        virtual void draw_object(
            DragoWorld *world,
            eObject3D *obj,
            glm::dmat4 *camera_mtx);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Camera parameters

	************************************************************************************************
	*/
	/*@{*/

    /** The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". 
        Usually between 90° (extra wide) and 30° (quite zoomed in)
     */
    double m_fovy;

    /** Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, 
        sounds familiar ?
     */
    double m_aspect;

    /** Near clipping plane. Keep as big as possible, or you'll get precision issues.
     */
    double m_znear;

    /** Far clipping plane. Keep as little as possible.
     */
    double m_zfar;
    /*@}*/
};

#endif
