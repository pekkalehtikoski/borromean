/**

  @file    eworld3d.cpp
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
#include "egui/egui3d.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eWorld3D::eWorld3D(
	eObject *parent,
	os_int oid,
	os_int flags)
    : eObject3D(parent, oid, flags)
{
    m_terrains = eContainer::newobj(this, EOID_ITEM, EOBJ_IS_ATTACHMENT);
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eWorld3D::~eWorld3D()
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
os_int eWorld3D::flags()
{
    return eObject3D::flags()|EOBJ3D_WORLD;
}


/**
****************************************************************************************************

  @brief How to update game.

  X...

  @return  None.

****************************************************************************************************
*/
void eWorld3D::update_game(
    eKeyboardCtrl *kbctrl,
    os_double elapsed_s)
{
    /* Calculate world for childern.
     */
    eObject3D::update_game(this, kbctrl, elapsed_s);
}
