/**

  @file    emovingbject3d.cpp
  @brief   3D object base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2016

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
eMovingObject3D::eMovingObject3D(
	eObject *parent,
	os_int oid,
	os_int flags)
    : eObject3D(parent, oid, flags)
{
    m_growth_rate = 0.0;

    /* Gravity
     */
    m_acceleration.y -= /* 1.62 */ 9.81; 
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eMovingObject3D::~eMovingObject3D()
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
os_int eMovingObject3D::flags()
{
    return eObject3D::flags()|EOBJ3D_MOVING|EOBJ3D_OBJECT_ROOT;
}


/**
****************************************************************************************************

  @brief Logic how this object moves or modifies itself.

  X...

  @return  None.

****************************************************************************************************
*/
void eMovingObject3D::update_game(
    eWorld3D *world,
    eKeyboardCtrl *kbctrl,
    os_double elapsed_s)
{
    if (kbctrl->up || kbctrl->down)
    {
        /* Make acceleration vector and rotate it to parent coordinates.
         */
        glm::dvec4 accel(0.0, -9.81, kbctrl->up ? -10.0 : 10.0, 1);
        glm::dvec4 raccel = m_rotation_mtx * accel;

        m_acceleration.x = raccel.x;
        m_acceleration.z = raccel.z;
        m_acceleration.y = -9.81;
    }
    else
    {
        m_acceleration.x = 0;
        m_acceleration.z = 0;
        m_acceleration.y = -9.81;
    }

    /* if (kbctrl->down)
    {
        m_acceleration.x = -10;
        //m_velocity.x *= 0.95 * elapsed_s;
    }
    else if (kbctrl->up)
    {
        m_acceleration.x = 2*9.81;
        if (m_velocity.x > 10) m_velocity.x = 10;
    }
    else 
    {
        if (m_velocity.x > 1)
        {
            m_acceleration.x = -10;
        }
        else if (m_velocity.x < -1)
        {
            m_acceleration.x = 10;
        }
        else
        {
            m_velocity.x = 0;
            m_acceleration.x = 0;
        }
    } */

    /* Turn left or right.
     */
    if (kbctrl->left)
    {
        m_angular_velocity.y = 3.14;
    }
    else if (kbctrl->right)
    {
        m_angular_velocity.y = -3.14;
    }
    else 
    {
        m_angular_velocity.y = 0.0;
    }
       
    m_velocity += m_acceleration * elapsed_s;
    m_acceleration -= 0.2 * m_acceleration * elapsed_s;
    m_position += m_velocity * elapsed_s;
    m_velocity -= 0.2 * m_velocity * elapsed_s;
    m_rotate += m_angular_velocity * elapsed_s;
    m_scale += m_scale * m_growth_rate * elapsed_s;
    if (m_scale < 1.0e-5) m_scale = 1.0e-5;

    /* Calculate world for childern.
     */
    eObject3D::update_game(world, kbctrl, elapsed_s);
}
