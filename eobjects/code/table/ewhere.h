/**

  @file    ewhere.h
  @brief   Where clause interprenter.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Where clause interpenter.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EWHERE_INCLUDED
#define EWHERE_INCLUDED


/**
****************************************************************************************************

  @brief Where clause interpenter.

  The eWhere class interprents and evaluates a where clause. The where clause is simplified
  SQL where clause, which can be converted easily to SQL where clause. It is only small subset
  of SQL is included and all names and values are quoted, so real SQL where clauses will not
  work without modification.

****************************************************************************************************
*/
class eWhere : public eObject
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
    eWhere(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
    virtual ~eWhere();

    /* Get class identifier.
     */
    virtual os_int classid()
    {
        return ECLASSID_WHERE;
    }

    /*@}*/

	/** 
	************************************************************************************************

      @name Evaluating where clause.

      First call compile function to generate bytecode and list of needed variables. Then set
      values for variables and call evaluate to see of where clause is true or false.

	************************************************************************************************
	*/
	/*@{*/

    /* Compile where clause. Generates byte code and variables.
     */
    void compile(
        os_char *where);

    /* Get pointer to eContainer holding variables.
     */
    inline eContainer *variables()
    {
        return m_vars;
    }

    /* Evaluate where clause with given set of variable values.
     */
    os_boolean evaluate();

    /*@}*/

protected:
    /**
    ************************************************************************************************

      @name Internal to where clause interprenter.

      Protected functions and member variables.

    ************************************************************************************************
    */
    /*@{*/

    /** Container for variables, exists always, has name space.
     */
    eContainer *m_vars;

    /*@}*/
};

#endif
