/**

  @file    egui.h
  @brief   Main egui library header file.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This egui library base main header file. If further includes rest of egui headers. 

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EGUI_INCLUDED
#define EGUI_INCLUDED

/* Include objects headers.
 */
#include "eobjects/eobjects.h"

/* Initialize the egui library for use.
 */
void egui_initialize(
    void *reserved);

/* Shut down the egui library.
*/
void egui_shutdown();

/* Include egui headers.
 */
#include "egui/extensions/3d/eobject3d.h"
#include "egui/extensions/3d/emesh3d.h"
#include "egui/extensions/3d/emovingobject3d.h"
#include "egui/extensions/3d/eterrain3d.h"
#include "egui/extensions/3d/eworld3d.h"

#endif
