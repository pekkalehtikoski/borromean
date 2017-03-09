/**

  @file    econsole.h
  @brief   Console stream class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Console as eobjects output stream.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ECONSOLE_INCLUDED
#define ECONSOLE_INCLUDED


/**
****************************************************************************************************

  @brief eConsole class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eConsole : public eStream
{
public:

    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/

	/* Constructor.
     */
	eConsole(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
	virtual ~eConsole();

    /* Casting eObject pointer to eConsole pointer.
     */
	inline static eConsole *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_CONSOLE)
		return (eConsole*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_CONSOLE; 
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

	/* Static constructor function.
	*/
	static eConsole *newobj(
		eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
	{
        return new eConsole(parent, id, flags);
	}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Console functions for writing to console.

	  X...

	************************************************************************************************
	*/
	/*@{*/

    /* Open the console. 
     */
    virtual eStatus open(
	    os_char *parameters,
        os_int flags = 0);

    /* Close the console. 
     */
    virtual eStatus close();

    /* Write data to console.
     */
    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL);

	/** Write character.
     */
    virtual eStatus writechar(
        os_int c);

    /*@}*/

private:
    os_int m_flags;

};

#endif
