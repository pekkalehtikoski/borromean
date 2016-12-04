/**

  @file    eoid.h
  @brief   Enumeration of object identifiers.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  This header file defines object identifiers used by eobjects library.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EOID_INCLUDED
#define EOID_INCLUDED


/**
****************************************************************************************************

  @name Object identifiers used by eobjects library.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
/*@{*/

	/** This is default object identifier, which specifies object simply as list item.
	 */
	#define EOID_ITEM -1

	/** This object is root of object tree.
	 */
	#define EOID_ROOT -2

	/** Attachment: This object is name.
	 */
	#define EOID_NAME -10

	/** Attachment: This object is namespace.
	 */
	#define EOID_NAMESPACE -11

	/** Attachment: This object pointer target link (ePointer use only).
	 */
	#define EOID_PPTR_TARGET -12

	/** This is special object identifier that can be given as argument to object hierarchy 
		functions, like getfirstchild(). The EOID_CHILD specifies to get a child object, 
		which is not flagged as attachment.
	 */
	#define EOID_CHILD -1000

	/** This is special object identifier that can be given as argument to object hierarchy 
		functions, like getfirstchild(). The EOID_ALL specifies to get a child object, 
		including child objects which are flagged as attachments.
	 */
	#define EOID_ALL -1001

	 /** Specifal object identifier EOID_PRIMITIVE indicates primitive object, typically 
	     eVariable, which is not part of tree hierarchy. This makes object use less memory 
		 and to be faster to allocate and free, but it cannot be root or child object.
		 This flag is argument for costructore
	 */
	#define EOID_PRIMITIVE -1002

/*@}*/

#endif
