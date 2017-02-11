/**

  @file    egui3d.h
  @brief   egui library header file with 3D extensions.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This egui library main header file with 3D extensions. If further includes rest of egui headers. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EGUI3D_INCLUDED
#define EGUI3D_INCLUDED

/* Include GLM headers.
 */
#include "egui/tools/glm-0.9.7.4/glm/glm.hpp"
#include "egui/tools/glm-0.9.7.4/glm/gtc/matrix_transform.hpp"

/* Include Glew header.
 */
#define GLEW_STATIC
#include <GL/glew.h>


/* Initialize the egui library for use.
 */
void egui3d_initialize(
    void *reserved);

/* Include egui base headers.
 */
#include "egui/egui.h"

/* Include egui 3D headers.
 */
#include "egui/extensions/3d/eobject3d.h"
#include "egui/extensions/3d/emesh3d.h"
#include "egui/extensions/3d/emovingobject3d.h"
#include "egui/extensions/3d/eterrain3d.h"
#include "egui/extensions/3d/eworld3d.h"

#endif