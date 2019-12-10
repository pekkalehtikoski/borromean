/**

  @file    ewhere.cpp
  @brief   Where clause interprenter.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Where clause interpenter.
  The where clause here is simplified SQL where clause.

  Grammar (not formally correct, just to get idea):

  \verbatim
  where_clause
   : WHERE expression

  expression
   : simple_expression
   | simple_expression expr_op expression

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

  column_name
   : ['A'-'Z','a'-'z','_'] digits ['0'-'9'], but not as first character.
        Double quoted column names are not checked for content.

  numeric_constant
   : [-]XXX[.YYY]

  timestamp_constant
   ; @78872134738217 Time stamp constants are GMT in microseconds since 1.1.1970. These
        are not part of real SQL at all, but must be converted to SQL time stamps when
        used with SQL server. Within this software these are just integers.
        Any character not decimal digit ends the timestamp constant.

  expr_op
   : AND | OR

  relational_op
   : '=' | '<>' | '<' | '>' | '>=' | '<='

  is_or_is_not
   : IS | IS NOT
  \endverbatim

  Notes:
  - SQL requires single quotes around text values.
  - If SQL server doesn't like double quotes, they can be just removed from expression string.
  - GMT time stamps can be expressed as microseconds sin
  - eWhere is intended for tables, but it could be used to implement other definable conditions.
    For example it could be useful for setting up show/hide conditions of GUI components.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#include "eobjects/eobjects.h"

/** Enumeration of operators in where clause. Operators are stored in m_code as they are.
 */
typedef enum eWhereOp
{
    EOP_AND = 1,
    EOP_OR,

    EOP_LE,
    EOP_NE,
    EOP_LT,
    EOP_GE,
    EOP_GT,
    EOP_EQ,
    EOP_IS_NULL,
    EOP_IS_NOT_NULL
}
eWhereOp;

/* Offsets for push variable and push constant in code. The item ID is added to this base
   in m_code. For example code value 10001 would mean "push first variable to execution stack".
 */
#define EOP_VARIABLE_BASE 10000
#define EOP_CONSTANT_BASE 20000


/**
****************************************************************************************************

  @brief Constructor.

  Clear member variables and create child objects which will be needed later.

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

    m_constants = new eContainer(this);
    m_code = new eBuffer(this);

    m_stack = new eBuffer(this);
    m_stack_ptr = 0;

    m_error = new eVariable(this);
    m_word = new eVariable(this);
    m_tmp = new eVariable(this);
    m_exec_tmp = new eContainer(this);

    m_nvars = 0;
    m_nconstants = 0;
}


/**
****************************************************************************************************

  @brief Compile where clause. Generates byte code and variables.

  The eWhere::compile() function compiles where clause given as argument to code. It also generates
  list of variables needed, this variables container is available by eWhere::variables() function.
  Each variable is named with column name and needs to be set to appropriate value before calling
  eWhere::evaluate().

  @param  whereclause Where clause, basically simplified SQL where clause, with added time stamp
          marking. This typically defines to which rows of a table a select, update or remove is
          applied.
  @return None.

****************************************************************************************************
*/
eStatus eWhere::compile(
    os_char *whereclause)
{
    m_vars->clear();
    m_nvars = 0;
    m_constants->clear();
    m_nconstants = 0;
    m_code->clear();
    m_pos = whereclause;
    return expression() ? ESTATUS_SUCCESS : ESTATUS_FAILED;
}


/**
****************************************************************************************************

  @brief Evaluate where clause.


  The eWhere::evaluate function evaluates (executes) the where clause. First eWhere::compile()
  function is called compile string function to generate code, and make list of needed variables.
  Before calling this function the variable values need to be initialized, use eWhere::variables()
  function to get pointer container holding variables for values. Each variable is named with
  column name. Finally call evaluate to see of where clause is true or false. evaluate can
  be called multiple times wirh different variable values without recompiling the original
  where code.

  @return ESTATUS_SUCCESS if condition is true, ESTATUS_FALSE if no match or ESTATUS_FAILED
          if something went wrong.

****************************************************************************************************
*/
eStatus eWhere::evaluate()
{
    eStackItem *stackitem;
    os_short *code, op;
    os_int count;

    /* No stack, clear execution tmp vars.
     */
    m_stack_ptr = 0;
    m_exec_tmp->clear();

    /* Get code pointer.
     */
    code = (os_short*)m_code->ptr();
    if (code == OS_NULL)
    {
        m_error->sets("ewhere.cpp: no code to execute");
        return ESTATUS_FAILED;
    }

    /* Get number of instructions
     */
    count = (os_int)(m_code->used() / sizeof(os_short));

    /* Execute it
     */
    while (count--)
    {
        op = *(code++);
        if (op >= EOP_CONSTANT_BASE)
        {
            pushconstant(op - EOP_CONSTANT_BASE);
        }
        else if (op >= EOP_VARIABLE_BASE)
        {
            pushvariable(op - EOP_VARIABLE_BASE);
        }
        else switch (op)
        {
            case EOP_IS_NULL:
            case EOP_IS_NOT_NULL:
                if (evalunaryop(op)) return ESTATUS_FAILED;
                break;

            default:
                if (evalbinaryop(op)) return ESTATUS_FAILED;
                break;
        }
    }

    /* If we do not have exactly one item in stack?
     */
    if (m_stack_ptr != 1)
    {
        m_error->sets("where clause evaluation failed");
        return ESTATUS_FAILED;
    }

    /* Get the final result from stack.
     */
    stackitem = (eStackItem*)m_stack->ptr();
    if (stackitem->datatype != OS_LONG)
    {
        changedatatype(stackitem, OS_LONG);
    }
    return stackitem->value.l ? ESTATUS_SUCCESS : ESTATUS_FALSE;
}


/**
****************************************************************************************************

  @brief Parse an expression.

  The eWhere::expression function is part of interprenter which parses source text and generates
  code to evaluate the where clause. Current input position within where clause is at m_pos,
  code is generated into buffer m_code, and variables into m_vars and constants into m_constants.
  The function parses following part of grammar:

  \verbatim
  expression
   : simple_expression
   | simple_expression expr_op expression

  expr_op
   : AND | OR
  \endverbatim

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
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
            m_error->sets("unknown extr_op, got: ");
            m_error->appendv(m_word);
            break;
        }

        if (!simple_expression()) break;

        code(op);
    }

    return OS_FALSE;
}


/**
****************************************************************************************************

  @brief Parse a simple expression.

  The eWhere::simple_expression function is part of interprenter which parses source text and
  generates code to evaluate the where clause. The function parses following part of grammar:

  \verbatim
  simple_expression
   : element
   | element relational_op element
   | element is_or_is_not NULL

  relational_op
   : '=' | '<>' | '<' | '>' | '>=' | '<='

  is_or_is_not
   : IS | IS NOT

  \endverbatim

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
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
            op = EOP_IS_NOT_NULL;
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
    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Parse an element.

  The eWhere::element function is part of interprenter which parses source text and generates
  code to evaluate the where clause. The function parses following part of grammar:

  \verbatim
  element
   : numeric_constant
   : @timestamp_constant
   | 'string_constant'
   | "column_name"
   | column_name
   | '(' expression ')'

  column_name
   : ['A'-'Z','a'-'z','_'] digits ['0'-'9'], but not as first character.
        Double quoted column names are not checked for content.

  numeric_constant
   : [-]XXX[.YYY]

  timestamp_constant
   ; @78872134738217 Time stamp constants are GMT in microseconds since 1.1.1970. These
        are not part of real SQL at all, but must be converted to SQL time stamps when
        used with SQL server. Within this software these are just integers.
        Any character not decimal digit ends the timestamp constant.

  \endverbatim

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
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
            if (column_name()) return OS_FALSE;
            break;

        /* time stamp constant (here these are just integers) */
        case '@':
            m_pos++;
            /* continues.. */

        /* numeric constant or column name */
        default:
            if (!number_or_column_name()) return OS_FALSE;
            break;

        /* string constant */
        case '\'':
            if (!string_constant()) return OS_FALSE;
            break;
    }

    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Parse an column name in double quotes.

  The eWhere::column_name function is part of interprenter which parses source text and generates
  code to evaluate the where clause. The function gets column name from within double quotes.
  The variable for value is created in m_vars container, and code to push variable value
  when executing is generated.

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
os_boolean eWhere::column_name()
{
    os_char *end;

    end = m_pos;
    while (*(++end) != '\"')
    {
        if (*end == '\0')
        {
            m_error->sets("terminating double quote missing");
            return OS_FALSE;
        }
    }

    code(addvariable(m_pos + 1, (os_memsz)(end - m_pos - 1)));
    m_pos = end+1;
    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Parse a number or column name without quotes.

  The eWhere::number_or_column_name_name function is part of interprenter which parses source
  text and generates code to evaluate the where clause. The function gets either number constant
  or column name without quotes. Number constant is stored in m_constants container or
  variable for column is created in m_vars container, and code to push variable value or numeric
  constant when executing is generated.

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
os_boolean eWhere::number_or_column_name()
{
    os_char *end, c;
    os_memsz nbytes;
    os_double d;
    os_long l;
    os_boolean isint;

    /* If column name.
     */
    if (osal_char_isaplha(*m_pos) || *m_pos == '_')
    {
        end = m_pos;
        do
        {
            c = *(++end);
        }
        while (osal_char_isaplha(c) ||
               osal_char_isdigit(c) ||
               c == '_');

        code(addvariable(m_pos, (os_memsz)(end - m_pos)));
        m_pos = end;
    }

    /* Otherwise must be numeric constant.
     */
    else
    {
        /* Decide if this is integer or floating point number.
         */
        isint = OS_TRUE;
        end = m_pos;
        if (*end == '-') end++;
        while (*end != '\0')
        {
            if (*end == '.')
            {
                isint = OS_FALSE;
                break;
            }
            if (!osal_char_isdigit(*end))
            {
                break;
            }
            end++;
        }

        if (isint)
        {
            l = osal_str_to_int(m_pos, &nbytes);
        }
        else
        {
             d = osal_string_to_double(m_pos, &nbytes);
        }
        if (nbytes == 0)
        {
            m_error->sets("Number expected, got: ");
            m_error->appends(m_pos);
            return OS_FALSE;
        }

        if (isint)
        {
            code(addlong(l));
        }
        else
        {
            code(adddouble(d));
        }
    }

    return OS_TRUE;
}


/**
****************************************************************************************************

  @brief Parse an sting constant in single quotes.

  The eWhere::string_constant function is part of interprenter which parses source text and
  generates code to evaluate the where clause. The function gets string constant from within
  single quotes. The string constant is stored in m_constants container and code to push
  string constant value when executing is generated.

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
os_boolean eWhere::string_constant()
{
    os_char *end;

    end = m_pos;
    while (*(++end) != '\'')
    {
        if (*end == '\0')
        {
            m_error->sets("terminating signle quote missing");
            return OS_FALSE;
        }
    }

    code(addstring(m_pos + 1, (os_memsz)(end - m_pos) - 1));
    m_pos = end+1;
    return OS_TRUE;
}

os_short eWhere::addlong(
    os_long l)
{
    eVariable *v;

    /* Add long to constants.
     */
    v = new eVariable(m_constants, ++m_nconstants);
    v->setl(l);

    /* Return object index for constant.
     */
    return m_nconstants + EOP_CONSTANT_BASE;
}

os_short eWhere::adddouble(
    os_double d)
{
    eVariable *v;

    /* Add double to constants.
     */
    v = new eVariable(m_constants, ++m_nconstants);
    v->setd(d);

    /* Return object index for constant.
     */
    return m_nconstants + EOP_CONSTANT_BASE;
}

os_short eWhere::addstring(
    os_char *str,
    os_memsz len)
{
    eVariable *v;

    /* Add string to constants.
     */
    v = new eVariable(m_constants, ++m_nconstants);
    v->sets(str, len);

    /* TEST Convert to long or double, if string can
     * be converted. Here we prettu much ignore
     * type set by quotes.
     *
    v->autotype(OS_TRUE);
    */

    /* Return object index for constant.
     */
    return m_nconstants + EOP_CONSTANT_BASE;
}


/**
****************************************************************************************************

  @brief Create value variable for a column and name it.

  The eWhere::column_name function creates a value variable for column in m_vars container and
  names it with column name.
  code to evaluate the where clause. The function parses following part of grammar:

  \verbatim
  column_name
   : ['A'-'Z','a'-'z','_'] digits ['0'-'9'], but not as first character.
        Double quoted column names are not checked for content.
  \endverbatim

  @return  OS_TRUE if parsing was successfull. OS_FALSE if the function failed on syntaxt error.

****************************************************************************************************
*/
os_short eWhere::addvariable(
    os_char *name,
    os_memsz len)
{
    eVariable *v;

    m_tmp->sets(name, len);
    name = m_tmp->gets();

    /* Find variable, if we got it already.
     * otherwise create new one.
     */
    v = eVariable::cast(m_vars->byname(name));
    if (v == OS_NULL)
    {
        v = new eVariable(m_vars, ++m_nvars);
        v->addname(name);
    }

    /* Return object index for variable.
     */
    return v->oid() + EOP_VARIABLE_BASE;
}


/**
****************************************************************************************************

  @brief Skip over empty space in input.

  The eWhere::skipspace function moves m_pos forward until character which is not space character
  is met.

  @return None.

****************************************************************************************************
*/
void eWhere::skipspace()
{
    while (*m_pos != '\0' && osal_char_isspace(*m_pos)) m_pos++;
}


/**
****************************************************************************************************

  @brief Get a word from input.

  The eWhere::getword function gets next word like AND, OR, IS, NOT, NULL from input, stores
  it to temporary m_word variable and returns pointer to the word.

  @return Pointer to next word, parsed from current input position.

****************************************************************************************************
*/
os_char *eWhere::getword()
{
    os_char *p;

    p = m_pos;
    while (osal_char_isaplha(*p) && *p != '\n') p++;

    m_word->sets(m_pos, p - m_pos);
    m_pos = p;
    return m_word->gets();
}


/**
****************************************************************************************************

  @brief Append instruction to code being generated.

  The eWhere::code function appends an instruction (code value) to code being generarated.
  These values are like 1, 2, 3... for operators, 10001, 1002, 1003... to push variable value
  to execution stack and 20001, 20002, 20003 to push constants into execution stack. See
  defines EOP_VARIABLE_BASE (10000) and EOP_CONSTANT_BASE (20000)

  @param  op Code value (instruction) to append to m_code.
  @return None.

****************************************************************************************************
*/
void eWhere::code(
    os_short op)
{
    m_code->write((os_char*)&op, sizeof(os_short));
}


/**
****************************************************************************************************

  @brief Make sure that execution stack has space for one push.

  The eWhere::checkstacksize function is part of code execution. It checks that execution stack
  is big enough to do one more push. If not, execution stack is expanded.

  @return None.

****************************************************************************************************
*/
void eWhere::checkstacksize()
{
    os_memsz stack_sz;

    stack_sz = m_stack->allocated()/sizeof(eStackItem);
    if (m_stack_ptr >= stack_sz)
    {
        m_stack->allocate((stack_sz+6) * sizeof(eStackItem));
    }
}


/**
****************************************************************************************************

  @brief Push a constant to execution stack.

  The eWhere::pushconstant function is part of code execution. It pushes constant value from
  m_constants container into execution stack.

  @param  id Constant identifier within m_constants container: 1, 2, 3...
  @return None.

****************************************************************************************************
*/
void eWhere::pushconstant(
    os_short id)
{
    eVariable *v;
    eStackItem *stackitem;

    checkstacksize();

    v = m_constants->firstv(id);
    if (v == OS_NULL)
    {
        osal_debug_error("ewhere.cpp: pushconstant error 1");
        return;
    }

    stackitem = (eStackItem*)m_stack->ptr() + m_stack_ptr++;
    switch (v->type())
    {
        case OS_LONG:
            stackitem->value.l = v->getl();
            stackitem->datatype = OS_LONG;
            stackitem->is_empty = OS_FALSE;
            break;

        case OS_DOUBLE:
            stackitem->value.d = v->getd();
            stackitem->datatype = OS_DOUBLE;
            stackitem->is_empty = OS_FALSE;
            break;

        case OS_STR:
            stackitem->value.s = v->gets();
            stackitem->datatype = OS_STR;
            stackitem->is_empty = (os_boolean)(stackitem->value.s == '\0');
            break;

        default:
            osal_debug_error("ewhere.cpp: pushconstant error 2");
            stackitem->value.l = 0;
            stackitem->datatype = OS_LONG;
            stackitem->is_empty = OS_TRUE;
            break;
    }

    stackitem->is_variable = OS_FALSE;
}


/**
****************************************************************************************************

  @brief Push a variable value to execution stack.

  The eWhere::pushvariable function is part of code execution. It pushes variable value from
  m_vars container into execution stack.

  @param  id Variable identifier within m_vars container: 1, 2, 3...
  @return None.

****************************************************************************************************
*/
void eWhere::pushvariable(
    os_short id)
{
    eVariable *v;
    eStackItem *stackitem;

    checkstacksize();

    v = m_vars->firstv(id);
    if (v == OS_NULL)
    {
        osal_debug_error("ewhere.cpp: pushvariable error");
        return;
    }

    stackitem = (eStackItem*)m_stack->ptr() + m_stack_ptr++;
    switch (v->type())
    {
        case OS_LONG:
            stackitem->value.l = v->getl();
            stackitem->datatype = OS_LONG;
            stackitem->is_empty = OS_FALSE;
            break;

        case OS_DOUBLE:
            stackitem->value.d = v->getd();
            stackitem->datatype = OS_DOUBLE;
            stackitem->is_empty = OS_FALSE;
            break;

        case OS_STR:
            stackitem->value.s = v->gets();
            stackitem->datatype = OS_STR;
            stackitem->is_empty = (os_boolean)(stackitem->value.s == '\0');
            break;

        default:
            stackitem->value.l = 0;
            stackitem->datatype = OS_LONG;
            stackitem->is_empty = OS_TRUE;
            break;
    }

    stackitem->is_variable = OS_TRUE;
}


/**
****************************************************************************************************

  @brief Unary operator.

  The eWhere::evalunaryop function is part of code execution. Unary operators take one argument
  from execution stack and push the result back to execution stack, so execution stack size
  does not change.
  Unary operator "IS NULL" is true if variable has no value. "IS NOT NULL" is true if variable
  has value.

  @param  op Unary operator, either EOP_IS_NULL or EOP_IS_NOT_NULL. See eWhereOp enumeration.
  @return ESTATUS_SUCCESS if all is fine. ESTATUS_FAILED on error.

****************************************************************************************************
*/
eStatus eWhere::evalunaryop(
    os_short op)
{
    eStackItem *stackitem;

    if (m_stack_ptr < 1)
    {
        m_error->sets("no data for unary OP");
        return ESTATUS_FAILED;
    }

    stackitem = (eStackItem*)m_stack->ptr() + m_stack_ptr - 1;

    if (stackitem->is_empty)
    {
        stackitem->value.l = (os_boolean)(op == EOP_IS_NULL);
        stackitem->datatype = OS_LONG;
        stackitem->is_empty = OS_FALSE;
    }
    else
    {
        stackitem->value.l = (os_boolean)(op == EOP_IS_NOT_NULL);
        stackitem->datatype = OS_LONG;
    }

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Binary operator.

  The eWhere::evalbinaryop function is part of code execution. Binary operators take two arguments
  from execution stack and push the result back to execution stack, so execution stack size
  gets smaller by one item.
  Binary operators are comparations, like less than, greater than, etc, and then operators
  like AND, OR.

  @param  op Binary operator, see eWhereOp enumeration.
  @return ESTATUS_SUCCESS if all is fine. ESTATUS_FAILED on error.

****************************************************************************************************
*/
eStatus eWhere::evalbinaryop(
    os_short op)
{
    eStackItem *item1, *item2, *tmp;
    osalTypeId datatype;
    os_int sign;
    os_boolean swapped;

    if (m_stack_ptr < 2)
    {
        m_error->sets("no data for binary OP");
        return ESTATUS_FAILED;
    }

    item2 = (eStackItem*)m_stack->ptr() + --m_stack_ptr;
    item1 = item2 - 1;

    if (op == EOP_AND || op == EOP_OR)
    {
        if (item1->datatype != OS_LONG)
        {
            changedatatype(item1, OS_LONG);
        }
        if (item2->datatype != OS_LONG)
        {
            changedatatype(item2, OS_LONG);
        }

        if (op == EOP_AND)
        {
            item1->value.l = (item1->value.l != 0 && item2->value.l != 0);
        }
        else
        {
            item1->value.l = (item1->value.l != 0 || item2->value.l != 0);
        }

        return ESTATUS_SUCCESS;
    }

    /* If types are same, no need to do ponder about conversions.
     */
    if (item1->datatype == item2->datatype)
    {
        datatype = item1->datatype;
        goto skipconv;
    }

    /* Start sorting so that stackitem1 holds desired result
     * type. If second one is string and first one is not, swap
     */
    swapped = OS_FALSE;
    if (item2->datatype == OS_STR && item1->datatype != OS_STR)
    {
        tmp = item2;
        item2 = item1;
        item1 = tmp;
        swapped = !swapped;
    }

    /* If second one is variable and first one is not, swap
     */
    if (item2->is_variable && !item1->is_variable)
    {
        tmp = item2;
        item2 = item1;
        item1 = tmp;
        swapped = !swapped;
    }

    /* If second one is double and first one is long, swap
     */
    if (item2->datatype == OS_DOUBLE && item1->datatype != OS_LONG)
    {
        tmp = item2;
        item2 = item1;
        item1 = tmp;
        swapped = !swapped;
    }

    /* If first one is empty and second one is not, swap
     */
    if (item1->is_empty && !item2->is_empty)
    {
        tmp = item2;
        item2 = item1;
        item1 = tmp;
        swapped = !swapped;
    }

    /* Convert item 2 to same type. datatype is type used.
     */
    datatype = item1->datatype;
    if (datatype != item2->datatype)
    {
        changedatatype(item2, datatype);
    }

    /* Swap order back
     */
    if (swapped)
    {
        tmp = item2;
        item2 = item1;
        item1 = tmp;
    }

skipconv:
    switch (datatype)
    {
        case OS_LONG:
            switch (op)
            {
                case EOP_LE:
                    item1->value.l = (item1->value.l <= item2->value.l);
                    break;

                case EOP_NE:
                    item1->value.l = (item1->value.l != item2->value.l);
                    break;

                case EOP_LT:
                    item1->value.l = (item1->value.l < item2->value.l);
                    break;

                case EOP_GE:
                    item1->value.l = (item1->value.l >= item2->value.l);
                    break;

                case EOP_GT:
                    item1->value.l = (item1->value.l > item2->value.l);
                    break;

                case EOP_EQ:
                    item1->value.l = (item1->value.l == item2->value.l);
                    break;
            }
            break;

        case OS_DOUBLE:
            switch (op)
            {
                case EOP_LE:
                    item1->value.d = (item1->value.d <= item2->value.d);
                    break;

                case EOP_NE:
                    item1->value.d = (item1->value.d != item2->value.d);
                    break;

                case EOP_LT:
                    item1->value.d = (item1->value.d < item2->value.d);
                    break;

                case EOP_GE:
                    item1->value.d = (item1->value.d >= item2->value.d);
                    break;

                case EOP_GT:
                    item1->value.d = (item1->value.d > item2->value.d);
                    break;

                case EOP_EQ:
                    item1->value.d = (item1->value.d == item2->value.d);
                    break;
            }
            break;

        case OS_STR:
            sign = os_strcmp(item1->value.s, item2->value.s);
            switch (op)
            {
                case EOP_LE:
                    item1->value.l = (sign <= 0);
                    break;

                case EOP_NE:
                    item1->value.l = (sign != 0);
                    break;

                case EOP_LT:
                    item1->value.l = (sign < 0);
                    break;

                case EOP_GE:
                    item1->value.l = (sign >= 0);
                    break;

                case EOP_GT:
                    item1->value.l = (sign > 0);
                    break;

                case EOP_EQ:
                    item1->value.l = (sign == 0);
                    break;
            }
            break;

    }

    return ESTATUS_SUCCESS;
}


/**
****************************************************************************************************

  @brief Change data type of item in execution stack.

  The eWhere::changedatatype function converts data type of item in evecution stack to another.
  When converting to string, temporary string is allocated from m_exec_tmp container.

  @param  item Item in execution stack to convert.
  @param  datatype Possible data types are OS_STR, OS_DOUBLE and OS_LONG.
  @return ESTATUS_SUCCESS if all is fine. ESTATUS_FAILED on error.

****************************************************************************************************
*/
void eWhere::changedatatype(
    eStackItem *item,
    osalTypeId datatype)
{
    eVariable *tmp;

    switch (datatype)
    {
        case OS_LONG:
            switch (item->datatype)
            {
                case OS_STR:
                    item->value.l = osal_str_to_int(item->value.s, OS_NULL);
                    break;

                case OS_DOUBLE:
                    item->value.l = eround_double_to_long(item->value.d);
                    break;
            }
            break;

        case OS_DOUBLE:
            switch (item->datatype)
            {
                case OS_STR:
                    item->value.d = osal_string_to_double(item->value.s, OS_NULL);
                    break;

                case OS_LONG:
                    item->value.d = (os_double)item->value.l;
                    break;
            }
            break;

        case OS_STR:
            switch (item->datatype)
            {
                case OS_DOUBLE:
                    tmp = new eVariable(m_exec_tmp);
                    tmp->setd(item->value.d);
                    item->value.s = tmp->gets();
                    break;

                case OS_LONG:
                    tmp = new eVariable(m_exec_tmp);
                    tmp->setl(item->value.l);
                    item->value.s = tmp->gets();
                    break;
            }
            break;
    }
    item->datatype = datatype;
}
