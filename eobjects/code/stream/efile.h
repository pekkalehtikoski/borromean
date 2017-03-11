/**

  @file    efile.h
  @brief   Stream IO for files.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Reading and writing files as a stream.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EFILE_INCLUDED
#define EFILE_INCLUDED


/**
****************************************************************************************************

  @brief File Class.

  The eFile class is flat byte buffer, which can also be used as a stream.

****************************************************************************************************
*/
class eFile : public eStream
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
    eFile(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
     */
    virtual ~eFile();

    /* Casting eObject pointer to eFile pointer.
     */
    inline static eFile *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_FILE)
        return (eFile*)o;
	}

	/* Get class identifier.
	*/
	virtual os_int classid() 
    { 
        return ECLASSID_FILE;
    }

    /*@}*/


    /**
    ************************************************************************************************

      @name Queue functions.
      Open, close, read, write, select soecke, etc. These implement eStream functionality.

    ************************************************************************************************
    */
    /*@{*/

    /* Open the file.
     */
    virtual eStatus open(
	    os_char *parameters,
        os_int flags = 0);

    /* Close the file.
     */
    virtual eStatus close();

    /* Flush writes.
     */
    virtual eStatus flush(
        os_int flags = 0);

    /* Write data to file.
     */
    virtual eStatus write(
        const os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nwritten = OS_NULL);

    /* Read data from file.
     */
    virtual eStatus read(
        os_char *buf, 
        os_memsz buf_sz, 
        os_memsz *nread = OS_NULL,
        os_int flags = 0);

    /*@}*/

private:
    /**
    ************************************************************************************************

      @name Internal for the class.
      Member variables and protected functions.

    ************************************************************************************************
    */
    /*@{*/
        osalStream m_handle;
    /*@}*/
};

#endif
