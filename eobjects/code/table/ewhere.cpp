/**

  @file    ewhere.cpp
  @brief   Where clause interprenter.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Where clause interpenter.
  The where clause here is simplified SQL where clause.

  Grammar (not formally correct, just to get idea):

  where_clause
   : WHERE expression
   ;

  expression
   : simple_expression
   | simple_expression expr_op expression
   ;

  simple_expression
   : element
   | element relational_op element
   | element is_or_is_not NULL
   ;

  element
   : numeric_constant
   : @timestamp_constant
   | 'string_constant'
   | "column_name"
   | column_name
   | '(' expression ')'
   ;

  column_name
   : ['A'-'Z','a'-'z','_'] digits ['0'-'9'], but not as first character.
        Double quoted column names are not checked for content.
   ;

  numeric_constant
   : [-]XXX[.YYY]
   ;

  timestamp_constant
   ; @78872134738217 Time stamp constants are GMT in microseconds since 1.1.1970. These
        are not part of real SQL at all, but must be converted to SQL time stamps when
        used with SQL server. Within this software these are just integers.
        Any character not decimal digit ends the timestamp constant.

  expr_op
   : AND | OR
   ;

  relational_op
   : '=' | '<>' | '<' | '>' | '>=' | '<='
   ;

  is_or_is_not
   : IS | IS NOT
   ;

  Notes:
  - SQL requires single quotes around text values.
  - If SQL server doesn't like double quotes, they can be just removed from expression string.
  - GMT time stamps can be expressed as microseconds sin

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"


/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eWhere::eWhere(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{

    /* Create container for variables and enable name space for it.
     */
    m_vars = new eContainer(this);
    m_vars->ns_create();
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eWhere::~eWhere()
{
}


/* Compile where clause. Generates byte code and variables.
 */
eStatus eWhere::compile(
    os_char *expression);
