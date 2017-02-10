/**

  @file    dragocamera.cpp
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
#include "drago.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
DragoCamera::DragoCamera(
	eObject *parent,
	os_int oid)
    : eObject3D(parent, oid)
{
    /* Set default camera parameters. See dragocamera.h for exaplanation.
     */
    m_fovy = 45;
    m_aspect = 4.0f / 3.0f;
    m_znear = 0.01f;
    m_zfar = 10000.0f;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
DragoCamera::~DragoCamera()
{
}

/**
****************************************************************************************************

  @brief Get object flags.

  Get object flags. Bit fields, returns EOBJ3D_DEFAULT (0) for no flags. Flag EOBJ3D_CAMERA 
  indicates camera, EOBJ3D_LIGHT a light source...

  @return  3D object flags.

****************************************************************************************************
*/
os_int DragoCamera::flags()
{
    return eObject3D::flags()|EOBJ3D_CAMERA|EOBJ3D_MOVING;
}



/**
****************************************************************************************************

  @brief Draw world to camera.

  X...

  @return  None.

****************************************************************************************************
*/
void DragoCamera::draw_world(
    DragoWorld *world)
{
    /* Calculate camera matrix
     */
    glm::dmat4 projection_mtx = glm::perspective(
        m_fovy, m_aspect, m_znear, m_zfar);
    glm::dmat4 camera_mtx = projection_mtx * m_inv_word_mtx;

    /* Draw all visible objects in world.
     */
    draw_object(world, world->m_world, &camera_mtx);
}


/**
****************************************************************************************************

  @brief Draw objects to camera recursively.

  X...

  @return  None.

****************************************************************************************************
*/
void DragoCamera::draw_object(
    DragoWorld *world,
    eObject3D *obj,
    glm::dmat4 *camera_mtx)
{
    eMesh3D
        *mesh;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

    for (mesh = obj->m_first_mesh; mesh; mesh = mesh->m_next_mesh)
    {
//        glBindBuffer(GL_ARRAY_BUFFER, world->m_glbuf[mesh->m_gl_vertex_buf_nr]);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, world->m_glbuf[mesh->m_gl_element_buf_nr]);

        glm::mat4 mmm2 = *camera_mtx * obj->m_world_mtx;
        glUniformMatrix4fv(world->m_MVP_uniform, 1, GL_FALSE, &mmm2[0][0]);

        mmm2 = obj->m_world_mtx;
        glUniformMatrix4fv(world->m_M_uniform, 1, GL_FALSE, &mmm2[0][0]);

        mmm2 = obj->m_norm_world_mtx;
        glUniformMatrix4fv(world->m_N_uniform, 1, GL_FALSE, &mmm2[0][0]);


// glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0);

// Wire frame
// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // WORKS
// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // WORKS (default)

// Rejection mode DO NOT USE
 // glEnable(GL_CULL_FACE); // WORKS, REJECTION MODE
// glDisable(GL_CULL_FACE); // WORKS, NO REJECTION (default)
 // glCullFace(GL_BACK);  // Reject back face


        glDrawElements(GL_TRIANGLES, mesh->m_elements_sz/sizeof(unsigned int), 
            GL_UNSIGNED_INT, BUFFER_OFFSET(mesh->m_element_offset));
    }

    for (eObject *child = obj->getfirst(); child; child = child->getnext())
    {
        draw_object(world, eObject3D::cast(child), camera_mtx);
    }
}
