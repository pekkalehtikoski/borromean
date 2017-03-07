/**

  @file    ematrix.h
  @brief   Matrix class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EMATRIX_INCLUDED
#define EMATRIX_INCLUDED


/**
****************************************************************************************************

  @brief Table interface.

  The eMatrix is base class for tables. Underlying table can be eMatrix, eSelection or database
  table class.


****************************************************************************************************
*/
class eMatrix : public eObject
{
    /**
    ************************************************************************************************

      @name Generic object functionality.

      These functions enable using objects of this class as generic eObjects.

    ************************************************************************************************
    */
    /*@{*/
public:
    /* Constructor.
	 */
    eMatrix(
		eObject *parent = OS_NULL,
        e_oid oid = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
    virtual ~eMatrix();

    /* Clone object.
     */
/*    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
        os_int aflags = 0); */

    /* Casting eObject pointer to eMatrix pointer.
     */
    inline static eMatrix *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_MATRIX)
        return (eMatrix*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid()
    {
        return ECLASSID_MATRIX;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eMatrix *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eMatrix(parent, oid, flags);
    }

    /* Write set content to stream.
     */
/*    virtual eStatus writer(
        eStream *stream, 
        os_int flags);
 */

    /* Read set content from stream.
     */
/*    virtual eStatus reader(
        eStream *stream, 
        os_int flags);
 */

#if E_SUPPROT_JSON
    /* Write set to stream as JSON.
     */
/*     eStatus json_writer(
        eStream *stream, 
        os_int sflags,
        os_int indent);
 */
#endif

    /*@}*/

	/** 
	************************************************************************************************

      @name Table function overrides.

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

    /* Update a row or rows of a table.
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


    /**
    ************************************************************************************************

      @name Matrix functions.

      These function treat matrix as organized to rows and columns.

    ************************************************************************************************
    */

    /* Allocate matrix.
     */
    void allocate(
        osalTypeId type,
        os_int width,
        os_int height,
        os_int mflags);

    /* Get matrix width.
     */
    os_int width();

    /* Get matrix height.
     */
    os_int height();

    /* Store value into matrix.
     */
    void set(
        os_int row,
        os_int column,
        eVariable *x,
        os_int mflags = 0);

    /* Store integer value into matrix.
     */
    inline void setl(
        os_int row,
        os_int column,
        os_long x);

    /* Store double value into matrix.
     */
    void setd(
        os_int row,
        os_int column,
        os_double x);

    /* Store string value into matrix.
     */
    void sets(
        os_int row,
        os_int column,
        const os_char *x);

    /* Clear matrix value to be empty.
     */
    void clear(
        os_int row,
        os_int column);

    /* Get value from matrix.
     */
    os_boolean get(
        os_int row,
        os_int column,
        eVariable *x);

    /* Get value from matrix as integer.
     */
    os_long getl(
        os_int row,
        os_int column,
        os_boolean *hasvalue = OS_NULL);

    /* Get value from matrix as double.
     */
    os_double getd(
        os_int row,
        os_int column,
        os_boolean *hasvalue = OS_NULL);

    /*@}*/
};

#endif
