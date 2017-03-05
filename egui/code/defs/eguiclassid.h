/**

  @file    eguiclassid.h
  @brief   Enumeration of egui class identifiers.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  This header file defines class identifiers used by egui library.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EGUICLASSID_INCLUDED
#define EGUICLASSID_INCLUDED


/**
****************************************************************************************************

  @name Class identifiers used by eobjects library.

  Each serializable eobjects library class has it's own class identifier. These may not be 
  modified, would break serialization.

****************************************************************************************************
*/
/*@{*/

/* First class id referved for application applications. All positive 32 bit integers
  starting from ECLASSID_APP_BASE can be used by application.
 */
#define ECLASSID_EGUI_BASE 64

#define EGUICLASSID_OBJECT3D       (ECLASSID_EGUI_BASE + 1)
#define EGUICLASSID_MOVINGOBJECT3D (ECLASSID_EGUI_BASE + 2)
#define EGUICLASSID_MESH3D         (ECLASSID_EGUI_BASE + 3)
#define EGUICLASSID_TERRAIN3D      (ECLASSID_EGUI_BASE + 4)
#define EGUICLASSID_WORLD3D        (ECLASSID_EGUI_BASE + 5)

/*@}*/

#endif
