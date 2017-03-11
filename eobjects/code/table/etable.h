/**

  @file    etable.h
  @brief   Table interface.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ETABLE_INCLUDED
#define ETABLE_INCLUDED


/**
****************************************************************************************************

  @brief Table interface.

  The eTable is base class for tables. Underlying table can be eMatrix, eSelection or database
  table class.


****************************************************************************************************
*/
class eTable : public eObject
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
    eTable(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
    virtual ~eTable();

    /* Casting eObject pointer to eTable pointer.
     */
    inline static eTable *cast(
		eObject *o) 
	{ 
        /* e_assert_type(o, ECLASSID_TABLE) */
        return (eTable*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid()
    {
        return ECLASSID_TABLE;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eTable *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eTable(parent, id, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

      @name Table functions.

      X...

	************************************************************************************************
	*/
	/*@{*/

    /* Configure the table.
     */
    virtual void configure(
        eContainer *configuration,
        os_int tflags)
    {}

    /* Insert rows into table.
     * Row can be one row or container with multiple rows.
     */
    virtual void insert(
        eContainer *rows,
        os_int tflags)
    {}

    /* Update a row or rows of a table or insert a row to the table.
     */
    virtual void update(
        eVariable *where,
        eContainer *row,
        os_int tflags)
    {}

    /* Remove rows from table.
     */
    virtual void remove(
        eVariable *where,
        os_int tflags)
    {}

    /* Select rows from table.
     */
    virtual void select(
        eVariable *where,
        os_int tflags)
    {}

    /*@}*/
};

#endif
