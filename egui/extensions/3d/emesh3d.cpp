/**

  @file    emesh3d.cpp
  @brief   3D mesh base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    12.5.2016

  Base class for mesges. Mesh consists of vertices and triangles? texture?

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#include "egui/egui3d.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eMesh3D::eMesh3D(
	eObject *parent,
	os_int oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    m_vertices = OS_NULL;
    m_vertices_sz = 0;
    m_vertex_offset = 0;
    m_gl_vertex_buf_nr = E3GL_NRO_BUFFERS;
    m_elements = OS_NULL;
    m_elements_sz = 0;
    m_element_offset = 0; 
    m_gl_element_buf_nr = E3GL_NRO_BUFFERS;
    m_next_mesh = OS_NULL;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eMesh3D::~eMesh3D()
{
    os_free(m_vertices, m_vertices_sz);
    os_free(m_elements, m_elements_sz);
}


/**
****************************************************************************************************

  @brief Allocate new empty mesh from the mesh container.

  The emesh_new function allocates a new empty mesh. New mesh will be child of meshes container.
  Meshes are not never deleted during the game, but may be cloned and/or modified. Memory
  allocated for meshes is freed only when mesh container is deleted.

  @return  None.

****************************************************************************************************
*/
eMesh3D *emesh3d_new(
    eContainer *meshes)
{
    return eMesh3D::newobj(meshes);
}


/**
****************************************************************************************************

  @brief Calculate bounding box in mesh coordinates.

  @param mm_1 Pointer to bounding box maximum coodinates to update or initialize.
  @param mm_2 Pointer to bounding box minimum coodinates to update or initialize.

  @return  None.

****************************************************************************************************
*/
void eMesh3D::calculate_bounding_box_mesh(
    glm::fvec4 *bb_1,
    glm::fvec4 *bb_2,
    bool *initialized)
{
    os_int 
        i,
        n;

    os_float
        x, 
        y,
        z;

    e3VertexAttrs
        *v;

    n = m_vertices_sz / sizeof(e3VertexAttrs);
    if (n <= 0) return;
    
    v = (e3VertexAttrs*)m_vertices;

    if (!*initialized)
    {
        bb_1->x = bb_2->x = v->x;
        bb_1->y = bb_2->y = v->y;
        bb_1->z = bb_2->z = v->z;
        bb_1->w = bb_2->w = 1.0F;

        *initialized = true;
        v++;
    }

    for (i = 1; i<n; i++)
    {
        x = v->x;
        y = v->y;
        z = v->z;
        v++;
        if (x > bb_1->x) bb_1->x = x;
        if (x < bb_2->x) bb_2->x = x;
        if (y > bb_1->y) bb_1->y = y;
        if (y < bb_2->y) bb_2->y = y;
        if (z > bb_1->z) bb_1->z = z;
        if (z < bb_2->z) bb_2->z = z;
    }
}
