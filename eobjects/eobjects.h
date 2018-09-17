/**

  @file    eobjects.h
  @brief   Main eobject library header file.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This eobject library base main header file. If further includes rest of eobjects base 
  headers. 

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
#include "eosalx.h"

/* Initialize eobject library for use.
 */
void eobjects_initialize(
    void *reserved);

/* Shut down eobjects library.
*/
void eobjects_shutdown();

/* Include eobject headers.
 */
#include "eobjects/code/defs/estatus.h"
#include "eobjects/code/defs/edefs.h"
#include "eobjects/code/defs/eoid.h"
#include "eobjects/code/defs/eclassid.h"
#include "eobjects/code/defs/emacros.h"
#include "eobjects/code/object/ehandle.h"
#include "eobjects/code/object/eobject.h"
#include "eobjects/code/object/ehandletable.h"
#include "eobjects/code/object/ehandleroot.h"
#include "eobjects/code/object/eclasslist.h"
#include "eobjects/code/root/eroot.h"
#include "eobjects/code/variable/evariable.h"
#include "eobjects/code/set/eset.h"
#include "eobjects/code/container/econtainer.h"
#include "eobjects/code/pointer/epointer.h"
#include "eobjects/code/name/ename.h"
#include "eobjects/code/name/enamespace.h"
#include "eobjects/code/binding/ebinding.h"
#include "eobjects/code/binding/epropertybinding.h"
#include "eobjects/code/envelope/eenvelope.h"
#include "eobjects/code/table/ewhere.h"
#include "eobjects/code/table/etable.h"
#include "eobjects/code/matrix/ematrix.h"
#include "eobjects/code/thread/ethreadhandle.h"
#include "eobjects/code/thread/ethread.h"
#include "eobjects/code/timer/etimer.h"
#include "eobjects/code/global/eprocess.h"
#include "eobjects/code/global/eglobal.h"
#include "eobjects/code/stream/estream.h"
#include "eobjects/code/stream/equeue.h"
#include "eobjects/code/stream/econsole.h"
#include "eobjects/code/stream/efile.h"
#include "eobjects/code/buffer/ebuffer.h"
#include "eobjects/code/connection/econnection.h"
#include "eobjects/code/connection/eendpoint.h"
#include "eobjects/code/main/emain.h"
/* #include "eobjects/extensions/socket/esocket.h" */

#endif
