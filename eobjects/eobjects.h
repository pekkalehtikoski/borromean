/**

  @file    eobjects.h
  @brief   Main eobject library header file.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This eobject library base main header file. If further includes rest of eobjects base 
  headers. When directory "/open-en" is in compiler's include path (either in project 
  settings of in compiler options), then "#include "eobjects/eobjects.h" will 
  include all eobjects base headers.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EOBJECTS_INCLUDED
#define EOBJECTS_INCLUDED

/* Include extended eosal headers.
 */
#include "eosal/osalx.h"

/* Include eobject headers.
 */
#include "eobjects/defs/estatus.h"
#include "eobjects/defs/edefs.h"
#include "eobjects/defs/eoid.h"
#include "eobjects/defs/eclassid.h"
#include "eobjects/object/ehandle.h"
#include "eobjects/object/eobject.h"
#include "eobjects/object/ehandletable.h"
#include "eobjects/object/ehandleroot.h"
#include "eobjects/object/eclasslist.h"
#include "eobjects/global/eglobal.h"
#include "eobjects/root/eroot.h"
#include "eobjects/variable/evariable.h"
#include "eobjects/container/econtainer.h"
#include "eobjects/pointer/epointer.h"
#include "eobjects/name/ename.h"
#include "eobjects/name/enamespace.h"
#include "eobjects/stream/estream.h"

/* Initialize eobject library for use.
 */
void eobjects_initialize(
    void *reserved);

/* Shut down eobjects library.
*/
void eobjects_shutdown();

#endif
