/**

  @file    eobject3d.cpp
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
#include "egui/egui3d.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eObject3D::eObject3D(
	eObject *parent,
	os_int oid,
	os_int flags)
    : eObject(parent, oid, flags)
{
    m_flags = EOBJ3D_DEFAULT;
    m_first_mesh = OS_NULL;
    m_scale = 1.0;
    m_bb_ok_mesh = false;
    m_bb_ok_world = false;
    m_bb_ok_cam = false;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eObject3D::~eObject3D()
{
}

/**
****************************************************************************************************

  @brief Clone object

  The clone function clones and object including object's children.
  Names will be left detached in clone if EOBJ_NO_MAP flag is given.

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eObject3D::clone(
    eObject *parent,
    e_oid id,
    os_int aflags)
{
    eObject3D *clonedobj;

    /* Clone must have parent object.
     */
    osal_debug_assert(parent);

    clonedobj = new eObject3D(parent, id == EOID_CHILD ? oid() : id, flags());

    /** Clone specific stuff.
     */
    /* clonedobj->m_command = m_command;
    clonedobj->m_mflags = m_mflags;
    clonedobj->settarget(target());
    clonedobj->prependsource(source()); */

    /* Copy all clonable children.
     */
    clonegeneric(clonedobj, aflags|EOBJ_CLONE_ALL_CHILDREN);
    return clonedobj;
}

#if 0
/**
****************************************************************************************************

  @brief Add the class to class list and class'es properties to it's property set.

  The eVariable::setupclass function adds the class to class list and class'es properties to
  it's property set. The class list enables creating new objects dynamically by class identifier,
  which is used for serialization reader functions. The property set stores static list of
  class'es properties and metadata for those.

****************************************************************************************************
*/
void eEnvelope::setupclass()
{
    const os_int cls = ECLASSID_ENVELOPE;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eEnvelope");

    addpropertyl(cls, EENVP_COMMAND, eenvp_command, EPRO_PERSISTENT|EPRO_SIMPLE, "command");
    addpropertys(cls, EENVP_TARGET, eenvp_target, EPRO_PERSISTENT|EPRO_SIMPLE, "target");
    addpropertys(cls, EENVP_SOURCE, eenvp_source, EPRO_PERSISTENT|EPRO_SIMPLE, "source");
    addproperty (cls, EENVP_CONTENT, eenvp_content, EPRO_PERSISTENT|EPRO_SIMPLE, "content");
    addproperty (cls, EENVP_CONTEXT, eenvp_context, EPRO_PERSISTENT|EPRO_SIMPLE, "context");
    os_unlock();
}
#endif


/**
****************************************************************************************************

  @brief Get object flags.

  Get object flags. Bit fields, returns EOBJ3D_DEFAULT (0) for no flags. Flag EOBJ3D_CAMERA 
  indicates camera, EOBJ3D_LIGHT a light source...

  @return  3D object flags.

****************************************************************************************************
*/
os_int eObject3D::flags()
{
    return m_flags;
}


/**
****************************************************************************************************

  @brief Logic how this object moves or modifies itself.

  X...

  @return  None.

****************************************************************************************************
*/
void eObject3D::update_game(
    eWorld3D *world,
    eKeyboardCtrl *kbctrl,
    os_double elapsed_s)
{
    eObject3D 
        *objroot;

    if ((flags() & EOBJ3D_MOVING) && m_bb_ok_world)
    {
        eContainer collisions;
        list_collisions(world, &collisions);
        if (collisions.first())
        {
            objroot = this;
            do
            {
                if (objroot->m_flags & EOBJ3D_OBJECT_ROOT)
                { 
                    if (objroot->m_velocity.y < 0) 
                    {
                        objroot->m_velocity.y = -0.27 * objroot->m_velocity.y;
                    }
                    break;
                }
                objroot = eObject3D::cast(objroot->parent());
            }
            while (objroot);
        }
    }

    /* Calculate world for childern.
     */
    for (eObject *child = first(); child; child = child->next())
    {
        eObject3D::cast(child)->update_game(world, kbctrl, elapsed_s);
    }
}


/**
****************************************************************************************************

  @brief Calculate positions for everything.

  X...

  @return  None.

****************************************************************************************************
*/
void eObject3D::calculate_world_mtx(
    eObject3D *camera)
{
    /* Calculate rotation matrix.
     */
    bool changed = false;
    if (m_rotate.x != m_prev_rotation.x)
    {
        m_prev_rotation.x = m_rotate.x;
        changed = true;
        m_rotation_x_mtx = glm::rotate(glm::dmat4(1.0), m_rotate.x, glm::dvec3(1.0, 0.0, 0.0));
    }
    if (m_rotate.y != m_prev_rotation.y)
    {
        m_prev_rotation.y = m_rotate.y;
        changed = true;
        m_rotation_y_mtx = glm::rotate(glm::dmat4(1.0), m_rotate.y, glm::dvec3(0.0, 1.0, 0.0));
    }
    if (m_rotate.z != m_prev_rotation.z)
    {
        m_prev_rotation.z = m_rotate.z;
        changed = true;
        m_rotation_z_mtx = glm::rotate(glm::dmat4(1.0), m_rotate.z, glm::dvec3(0.0, 0.0, 1.0));
    }
    if (changed)
    {
        m_rotation_mtx = m_rotation_z_mtx * m_rotation_x_mtx * m_rotation_y_mtx;
    }

    /* Calculate scaling matrix to make object bigger or smaller.
        */
    glm::dmat4 scale_mtx;
    scale_mtx[0][0] = m_scale;
    scale_mtx[1][1] = m_scale;
    scale_mtx[2][2] = m_scale;

    /* Calculate positioning to parent's coordinates.
        */
    glm::dmat4 position_mtx;
    position_mtx[3][0] = m_position.x;
    position_mtx[3][1] = m_position.y;
    position_mtx[3][2] = m_position.z;

    /* Calculate transformation to parent objects's coordinates.
     */
    m_local_mtx = position_mtx * m_rotation_mtx * scale_mtx;

    /* Calculate transformation of normals to parent objects's coordinate system.
     */
    m_norm_local_mtx = m_rotation_mtx * scale_mtx;

    /* Calculate world position.
     */
    if (oid() != EOID_ROOT)
    {
        eObject3D *p = eObject3D::cast(parent());

        if (p)
        {
            m_world_mtx = p->m_world_mtx * m_local_mtx;
            m_norm_world_mtx = p->m_norm_world_mtx * m_norm_local_mtx;
        }
    }

    /* Calculate world for childern.
     */
    for (eObject *child = first(); child; child = child->next())
    {
        eObject3D::cast(child)->calculate_world_mtx(camera);
    }

    /* If we have no bounding box in mesh coordinates, calculate now.
     */
    if (!m_bb_ok_mesh)
    {
        calculate_bounding_box_mesh();
    }

    /* Calculate bounding box in word coordinates.
     */
    calculate_bounding_box_world();

    /* Calculate bounding box in camera coordinates.
     */
    calculate_bounding_box_camera(camera);

    if (flags() & EOBJ3D_CAMERA)
    {
        calculate_inverse_world_mtx();
    }
}

void eObject3D::calculate_inverse_world_mtx()
{
    if (oid() == EOID_ROOT) return;

    glm::dmat4 rotation_x_mtx = glm::rotate(glm::dmat4(1.0), -m_rotate.x, glm::dvec3(1.0, 0.0, 0.0));
    glm::dmat4 rotation_y_mtx = glm::rotate(glm::dmat4(1.0), -m_rotate.y, glm::dvec3(0.0, 1.0, 0.0));
    glm::dmat4 rotation_z_mtx = glm::rotate(glm::dmat4(1.0), -m_rotate.z, glm::dvec3(0.0, 0.0, 1.0));
    glm::dmat4 rotation_mtx = rotation_y_mtx * rotation_x_mtx * rotation_z_mtx;

    /* Calculate scaling matrix to make object bigger or smaller.
     */
    glm::dmat4 scale_mtx;
    scale_mtx[0][0] = 1.0/m_scale;
    scale_mtx[1][1] = 1.0/m_scale;
    scale_mtx[2][2] = 1.0/m_scale;

    /* Calculate positioning to parent's coordinates.
        */
    glm::dmat4 position_mtx;
    position_mtx[3][0] = -m_position.x;
    position_mtx[3][1] = -m_position.y;
    position_mtx[3][2] = -m_position.z;

    glm::dmat4 local_mtx = scale_mtx * rotation_mtx * position_mtx;

    eObject3D *p = eObject3D::cast(parent());
    if (p) if (p->oid() != EOID_ROOT)
    {
        p->calculate_inverse_world_mtx();
        m_inv_word_mtx = local_mtx * p->m_inv_word_mtx;
    }
    else
    {
        m_inv_word_mtx = local_mtx;
    }
}


/**
****************************************************************************************************

  @brief List objects with which this object collides.

  Append collisions to collissions list.

  @return  None.

****************************************************************************************************
*/
void eObject3D::list_collisions(
    eWorld3D *world,
    eContainer *collisions)
{
    ePointer
        *ptrobj,
        *collision;

    eTerrain3D
        *terrain;

    /* If we have no bounding box, no collitions are possible.
     */
    if (!m_bb_ok_world) return;

    for (ptrobj = ePointer::cast(world->m_terrains->first(EOID_PPTR_TARGET));
            ptrobj;
            ptrobj = ePointer::cast(ptrobj->next(EOID_PPTR_TARGET)))
    {
        terrain = eTerrain3D::cast(ptrobj->get());
        if (terrain)
        {
            if (terrain->collides_with_terrain(this))
            {
                collision = new ePointer(collisions);
                collision->set(terrain);
            }
        }
    }
}


/**
****************************************************************************************************

  @brief Calculate bounding box in mesh coordinates.

  The calculate_bounding_box_mesh function calculates bounding box in mesh coordinates.
  If

  @return  None.

****************************************************************************************************
*/
void eObject3D::calculate_bounding_box_mesh()
{
    eMesh3D
        *mesh;

    /* Bounding box not yet calculated.
     */
    m_bb_ok_mesh = false;

    /* Loop through all meshes.
     */
    for (mesh = m_first_mesh; mesh; mesh = mesh->m_next_mesh)
    {
        mesh->calculate_bounding_box_mesh(&m_bb_1_mesh, &m_bb_2_mesh, &m_bb_ok_mesh);
    }
}


/**
****************************************************************************************************

  @brief Calculate bounding box in world coordinates.

  The calculate_bounding_box_mesh function calculates bounding box in mesh coordinates.
  If

  @return  None.

****************************************************************************************************
*/
void eObject3D::calculate_bounding_box_world()
{
    /* If bounding box in world coordinates has been calculated and this is not moving object,
       then do nothing.
     */
    if (m_bb_ok_world && (flags() & EOBJ3D_MOVING)==0) return;

    /* If we do not have bounding box in mesh coordinates, we cannot calculate.
     */
    if (!m_bb_ok_mesh)
    { 
        m_bb_ok_world = false;
        return;
    }

    m_bb_1_world = m_world_mtx * m_bb_1_mesh;
    m_bb_2_world = m_world_mtx * m_bb_2_mesh;

    m_bb_ok_world = true;
}


/**
****************************************************************************************************

  @brief Calculate bounding box in world coordinates.

  The calculate_bounding_box_mesh function calculates bounding box in mesh coordinates.
  If

  @return  None.

****************************************************************************************************
*/
void eObject3D::calculate_bounding_box_camera(
    eObject3D *camera)
{
    /* If mouse click is not supported for this object or we have no bounding box in mesh
       cooddinates, then do nothing.
     */
    if ((flags() & EOBJ3D_MOUSE_CLICK)==0 || !m_bb_ok_mesh) 
    {
        m_bb_ok_cam = false;
        return;
    }

/*
    CAMERA NEEDS TO BE MOVED TO EOBJECTS BEFORE THIS CAN BE CALCULATED
    m_bb_1_cam = m_world_mtx * m_bb_1_mesh;
    m_bb_2_cam = m_world_mtx * m_bb_2_mesh;
    m_bb_ok_cam = true;
*/
}


/**
****************************************************************************************************

  @brief Get pointer to mesh.

  X...

  @return  None.

****************************************************************************************************
*/
/* eMesh3D *eObject3D::get_mesh()
{
    return m_mesh;
}
*/

/* void eObject3D::set_mesh(
    eMesh3D *mesh)
{
    m_mesh = mesh;
}
*/
