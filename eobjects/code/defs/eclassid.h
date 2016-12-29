/**

  @file    eclassid.h
  @brief   Enumeration of class identifiers.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  This header file defines class identifiers used by eobjects library.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECLASSID_INCLUDED
#define ECLASSID_INCLUDED


/**
****************************************************************************************************

  @name Class identifiers used by eobjects library.

  Each serializable eobjects library class has it's own class identifier. These may not be 
  modified, would break serialization.

****************************************************************************************************
*/
/*@{*/

#define ECLASSID_OBJECT 1
#define ECLASSID_ROOT 2
#define ECLASSID_CONTAINER 3
#define ECLASSID_VARIABLE 4
#define ECLASSID_NAME 5
#define ECLASSID_NAMESPACE 6
#define ECLASSID_ENVELOPE 7
#define ECLASSID_THREAD 8
#define ECLASSID_POINTER 9
#define ECLASSID_STREAM 10

#define ECLASSID_MAX 20

/*@}*/

#endif
