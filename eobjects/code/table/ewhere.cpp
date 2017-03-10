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

typedef enum eWhereOp
{
    EOP_AND,
    EOP_OR,

    EOP_LE,
    EOP_NE,
    EOP_LT,
    EOP_GE,
    EOP_GT,
    EOP_EQ,
    EOP_IS_NULL,
    EOP_IS_NOTNULL
}
eWhereOp;


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

    m_error = new eVariable(this);
    m_word = new eVariable(this);
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


/**
****************************************************************************************************

  @brief Compile where clause. Generates byte code and variables.

  X...

  @return  None.

****************************************************************************************************
*/
eStatus eWhere::compile(
    os_char *whereclause)
{
    m_vars->clear();
    m_pos = whereclause;
    return expression() ? ESTATUS_SUCCESS : ESTATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Evaluate where clause with given set of variable values.

  X...

  @return  None.

****************************************************************************************************
*/
os_boolean eWhere::evaluate()
{
    return OS_FALSE;
}


os_boolean eWhere::expression()
{
    os_short op;
    os_char *w;
    os_boolean parenthises = OS_FALSE;

    skipspace();
    if (*m_pos == '(') parenthises = OS_TRUE;

    if (!simple_expression()) return OS_FALSE;

    while (OS_TRUE)
    {
        skipspace();
        if (*m_pos == '\0')
        {
            if (parenthises)
            {
                m_error->sets("closing ')' missing");
                return OS_FALSE;
            }
            return OS_TRUE;
        }
        if (*m_pos == ')')
        {
            if (!parenthises)
            {
                m_error->sets("extra ')' found");
                return OS_FALSE;
            }
            m_pos++;
            return OS_TRUE;
        }

        w = getword();
        if (!os_strcmp(w, "AND"))
        {
            op = EOP_AND;
        }
        else if (!os_strcmp(w, "OR"))
        {
            op = EOP_OR;
        }
        else
        {
            m_error->sets("Unknown extr_op, got: ");
            m_error->appendv(m_word);
            break;
        }

        if (!simple_expression()) break;

        code(op);
    }

    return OS_FALSE;
}

os_boolean eWhere::simple_expression()
{
    os_short op;
    os_char *w;

    if (!element()) return OS_FALSE;
    skipspace();
    if (*m_pos == '\0') return OS_TRUE;

    if (*m_pos == '<')
    {
        if (*(++m_pos) == '=')
        {
            op = EOP_LE;
            m_pos++;
        }
        else if (*m_pos == '>')
        {
            op = EOP_NE;
            m_pos++;
        }
        else
        {
            op = EOP_LT;
        }
    }
    else if (*m_pos == '>')
    {
        if (*(++m_pos) == '=')
        {
            op = EOP_GE;
            m_pos++;
        }
        else
        {
            op = EOP_GT;
        }
    }
    else if (*m_pos == '=')
    {
        op = EOP_EQ;
        m_pos++;
    }
    else
    {
        w = getword();
        if (os_strcmp(w, "IS"))
        {
            m_error->sets("relational_op or IS expected");
            return OS_FALSE;
        }
        skipspace();
        w = getword();
        op = EOP_IS_NULL;
        if (!os_strcmp(w, "NOT"))
        {
            skipspace();
            w = getword();
            op = EOP_IS_NOTNULL;
        }
        if (os_strcmp(w, "NULL"))
        {
            m_error->sets("NULL expected, got: ");
            m_error->appendv(m_word);
            return OS_FALSE;
        }
    }

    if (!element()) return OS_FALSE;

    code(op);
}

os_boolean eWhere::element()
{
    skipspace();

    switch (*m_pos)
    {
        /* expression */
        case '(':
            if (!expression()) return OS_FALSE;
            break;

        /* column name */
        case '\"':
            break;

        /* time stamp constant */
        case '@':
            m_pos++;
            break;

        /* numeric constant or column name */
        default:
            if (!number_or_column_name()) return OS_FALSE;
            break;

        /* string constant */
        case '\'':
            break;
    }

    return OS_TRUE;
}

os_boolean eWhere::number_or_column_name()
{
#if 0
    /* If column name.
     */
    if (isalpha(*m_pos))
    {
        w = getword();
        xx addcolumn(w);
    }

    /* Otherwise must be numeric constant.
     */
    else
    {
        nbytes = float_from_strin(?);
        if (nbytes == 0)
        {
            m_error->sets("Number expected, got: ");
            m_error->appends(m_pos);
            return OS_FALSE;
        }
    }
#endif

    return OS_TRUE;
}

