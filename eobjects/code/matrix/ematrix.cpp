/**

  @file    ematrix.cpp
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
#include "eobjects/eobjects.h"


/* Approximate size for one eBuffer, adjusted to memory allocation block.
 */
#define OEMATRIX_APPROX_BUF_SZ 120

typedef union
{
    os_long l;
    os_double d;
    os_char *s;
    eObject *o;
}
eMatrixObj;



/**
****************************************************************************************************

  @brief Constructor.

  X...

  @return  None.

****************************************************************************************************
*/
eMatrix::eMatrix(
	eObject *parent,
    e_oid id,
	os_int flags)
    : eObject(parent, id, flags)
{
    /** Default data type is OS_OBJECT.
     */
    m_datatype = OS_OBJECT;
    m_typesz = typesz(m_datatype);

    /** Clear rest of member variables.
     */
    m_nrows = m_ncolumns = 0;
    m_elems_per_block = 0;
}


/**
****************************************************************************************************

  @brief Virtual destructor.

  X...

  @return  None.

****************************************************************************************************
*/
eMatrix::~eMatrix()
{
    clear();
}


/**
****************************************************************************************************

  @brief Add eMatrix to class list.

  The eMatrix::setupclass function adds newobj function pointer to class list. This enables creating
  new objects dynamically by class identifier, which is used for serialization reader()
  functions.

****************************************************************************************************
*/
void eMatrix::setupclass()
{
    const os_int cls = ECLASSID_MATRIX;

    /* Add the class to class list.
     */
    os_lock();
    eclasslist_add(cls, (eNewObjFunc)newobj, "eMatrix");
    os_unlock();
}


/* Allocate matrix.
 */
void eMatrix::allocate(
    osalTypeId datatype,
    os_int nrows,
    os_int ncolumns,
    os_int mflags)
{

    /* Make sure that data type is known.
     */
    switch (datatype)
    {
        case OS_OBJECT:
        case OS_CHAR:
        case OS_SHORT:
        case OS_INT:
        case OS_LONG:
        case OS_FLOAT:
        case OS_DOUBLE:
            break;

        default:
            datatype = OS_OBJECT;
            break;
    }

    /* If we have previous data with different data type, clear it from memory.
     */
    if (datatype != m_datatype && m_nrows && m_ncolumns)
    {
        clear();
    }

    /* Save data type and element size. Elements per block has not been set yet
     */
    m_datatype = datatype;
    m_typesz = typesz(datatype);
    m_elems_per_block = 0;

    /* Resize the matrix. This doesn't allocate any memory yet, unless data can
       fit into small buffer.
     */
    resize(nrows, ncolumns);
}

/* Release all allocated data, empty the matrix.
 */
void eMatrix::clear()
{
    eBuffer *buffer, *nextbuffer;

    for (buffer = eBuffer::cast(first());
         buffer;
         buffer = nextbuffer)
    {
        nextbuffer = eBuffer::cast(buffer->next());
        if (buffer->oid() > 0)
        {
            releasebuffer(buffer);
        }
    }

    m_nrows = m_ncolumns = 0;
}


/**
****************************************************************************************************

  @brief Store variable value into matrix.

  The eMatrix::set stores variable value to matrix. Row and column specify position in matrix.
  The first row is 0 and first columns is 0. The matrix is expanded if row/column specify position
  outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Value to store.
  @param  mflags Reserved for future, set 0 for now.
  @return None.

****************************************************************************************************
*/
void eMatrix::set(
    os_int row,
    os_int column,
    eVariable *x,
    os_int mflags)
{
    /* If variable is empty, clear the value
     */
    if (x->isempty())
    {
        clear(row, column);
        return;
    }

    /* Switch by data type in variable.
     */
    switch (x->type())
    {
        case OS_LONG:
            setl(row, column, x->getl());
            break;

        case OS_DOUBLE:
            setd(row, column, x->getd());
            break;

        case OS_STRING:
            sets(row, column, x->gets());
            break;

        case OS_OBJECT:
            seto(row, column, x->geto());
            break;

        case OS_POINTER:
            clear(row, column);
            break;

        default:
            osal_debug_error("ematrix.cpp: unknown variable data type.");
            clear(row, column);
            break;
    }
}


/**
****************************************************************************************************

  @brief Store integer value into matrix.

  The eMatrix::setl stores integer value to matrix. Row and column specify position in matrix.
  The first row is 0 and first columns is 0. The matrix is expanded if row/column specify position
  outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Value to store.
  @return None.

****************************************************************************************************
*/
void eMatrix::setl(
    os_int row,
    os_int column,
    os_long x)
{
    os_char *dataptr, *typeptr;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_TRUE);
    if (dataptr == OS_NULL) return;

    switch (m_datatype)
    {
        case OS_OBJECT:
            ((eMatrixObj*)dataptr)->l = x;
            *typeptr = OS_LONG;
            break;

        case OS_CHAR:
            *((os_char*)dataptr) = (os_char)x;
            break;

        case OS_SHORT:
            *((os_short*)dataptr) = (os_short)x;
            break;

        case OS_INT:
            *((os_int*)dataptr) = (os_int)x;
            break;

        case OS_LONG:
            *((os_int*)dataptr) = (os_int)x;
            break;

        case OS_FLOAT:
            *((os_float*)dataptr) = (os_float)x;
            break;

        case OS_DOUBLE:
            *((os_double*)dataptr) = (os_double)x;
            break;

        default:
            break;
    }
}


/**
****************************************************************************************************

  @brief Store double value into matrix.

  The eMatrix::setd stores double precision floating point value to matrix. Row and column specify
  position in matrix. The first row is 0 and first columns is 0. The matrix is expanded if
  row/column specify position outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Value to store.
  @return None.

****************************************************************************************************
*/
void eMatrix::setd(
    os_int row,
    os_int column,
    os_double x)
{
    os_char *dataptr, *typeptr;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_TRUE);
    if (dataptr == OS_NULL) return;

    switch (m_datatype)
    {
        case OS_OBJECT:
            ((eMatrixObj*)dataptr)->d = x;
            *typeptr = OS_DOUBLE;
            break;

        case OS_CHAR:
            *((os_char*)dataptr) = eround_double_to_char(x);
            break;

        case OS_SHORT:
            *((os_short*)dataptr) = eround_double_to_short(x);
            break;

        case OS_INT:
            *((os_int*)dataptr) = eround_double_to_int(x);
            break;

        case OS_LONG:
            *((os_int*)dataptr) = eround_double_to_long(x);
            break;

        case OS_FLOAT:
            *((os_float*)dataptr) = (os_float)x;
            break;

        case OS_DOUBLE:
            *((os_double*)dataptr) = x;
            break;

        default:
            break;
    }
}


/* Resize the matrix.
 */
void eMatrix::resize(
    os_int nrows,
    os_int ncolumns)
{
    eBuffer *buffer, *nextbuffer;
    eVariable *tmp;
    eMatrix *m;
    os_int elem_ix, buffer_nr, minrows, mincolumns, row, column;

    /* If we need to reorganize, do it the hard way. This is very slow,
       application should be written in such way that this is never needed.
     */
    if (ncolumns != m_ncolumns && m_nrows > 1 && m_ncolumns > 0)
    {
        tmp = new eVariable(this);
        m = new eMatrix(this);
        m->allocate(m_datatype, nrows, ncolumns);

        minrows = nrows < m_nrows ? nrows : m_nrows;
        mincolumns = ncolumns < m_ncolumns ? ncolumns : m_ncolumns;

        for (row = 0; row < minrows; row++)
        {
            for (column = 0; column < mincolumns; column++)
            {
                if (get(row, column, tmp))
                {
                    m->set(row, column, tmp);
                }
            }
        }

        clear();

        /* Adopt data buffers.
         */
        for (buffer = eBuffer::cast(m->first());
             buffer;
             buffer = nextbuffer)
        {
            nextbuffer = eBuffer::cast(buffer->next());
            if (buffer->oid() > 0) adopt(buffer);
        }

        delete m;
        delete tmp;
    }

    /* Otherwise if we need to delete rows
     */
    else if (nrows < m_nrows && m_nrows > 0 && m_ncolumns > 0)
    {
        /* Element index of last element to keep.
         */
        elem_ix = ((nrows-1) * m_ncolumns-1) + (ncolumns-1);

        /* Buffer number of last buffer to keep
         */
        buffer_nr =  elem_ix / m_elems_per_block + 1;

        /* Delete buffers with bigger number than buffer_nr
         */
        for (buffer = eBuffer::cast(first());
             buffer;
             buffer = nextbuffer)
        {
            nextbuffer = eBuffer::cast(buffer->next());
            if (buffer->oid() > buffer_nr)
            {
                releasebuffer(buffer);
            }
        }
    }

    m_nrows = nrows;
    m_ncolumns = ncolumns;
}


/* Get pointer to data for element and if m_type is OS_OBJECT also type for element.
 */
os_char *eMatrix::getptrs(
    os_int row,
    os_int column,
    os_char **typeptr,
    os_boolean isset)
{
    eBuffer *buffer;
    os_char *dataptr;
    os_int elem_ix, buffer_nr;

    /* If this is outside current matrix size.
     */
    if (row >= m_nrows ||
        column >= m_ncolumns)
    {
        /* If reading from matrix, it cannot be expanded.
         */
        if (!isset) return OS_NULL;

        /* Make matrix bigger to fit this point
         */
        resize(row >= m_nrows ? row + 1 : m_nrows,
            column >= m_ncolumns ? column + 1 : m_ncolumns);
    }

    /* Element index is
     */
    elem_ix = (row * m_ncolumns + column);

    /* Buffer index from 1... and element index within buffer 0...
     */
    buffer_nr =  elem_ix / m_elems_per_block + 1;
    elem_ix %= m_elems_per_block;

    /* Get eBuffer where this value belongs to.
     */
    buffer = getbuffer(buffer_nr, isset);
    if (buffer == OS_NULL) return OS_NULL;

    dataptr = buffer->ptr();
    *typeptr = dataptr + m_elems_per_block * m_typesz + elem_ix;
    dataptr += elem_ix * m_typesz;

    /* Item found, dataptr and typeptr are set now. If this is
       set and m_datatype is OS_OBJECT, we check if we need to
       release object or string from memory.
     */
    if (isset && m_datatype == OS_OBJECT)
    {
        emptyobject(dataptr, *typeptr);
    }
    return dataptr;
}


/* Get or allocate eBuffer by buffer number (oid).
 */
eBuffer *eMatrix::getbuffer(
    os_int buffer_nr,
    os_boolean isset)
{
    eBuffer *buffer;
    os_int bytes_per_elem, count;
    os_char *ptr;

    buffer = eBuffer::cast(first(buffer_nr));
    if (buffer || !isset) return buffer;

    buffer = new eBuffer(this, buffer_nr);

    bytes_per_elem = m_typesz;
    if (m_datatype == OS_OBJECT) bytes_per_elem += sizeof(os_char);

    /* If we have not yet decided on elements per block
     */
    if (m_elems_per_block == 0)
    {
        buffer->allocate(OEMATRIX_APPROX_BUF_SZ);
        m_elems_per_block = buffer->allocated() / bytes_per_elem;
    }
    else
    {
        buffer->allocate(m_elems_per_block * bytes_per_elem);
    }

    /* If this is not object data type, mark all items
       empty (maximum value of the type.
     */
    if (m_datatype != OS_OBJECT)
    {
        ptr = buffer->ptr();
        count = m_elems_per_block;
        while (count--)
        {
            emptyobject(ptr, OS_NULL);
            ptr += m_typesz;
        }
    }

    return buffer;
}

/* Free memory buffer and any objects and strings allocated for it.
 */
void eMatrix::releasebuffer(
    eBuffer *buffer)
{
    eMatrixObj *mo;
    os_char *typeptr;
    os_int i;

    /* If this object data type.
     */
    if (m_datatype == OS_OBJECT)
    {
        mo = (eMatrixObj*)buffer->ptr();
        typeptr = (os_char*)(mo + m_elems_per_block);

        for (i = 0; i < m_elems_per_block; i++)
        {
            switch (typeptr[i])
            {
                case OS_STRING:
                    os_free(mo[i].s, os_strlen(mo[i].s));
                    break;

                case OS_OBJECT:
                    delete mo[i].o;
                    break;

                default:
                    break;
            }
        }
    }

    delete buffer;
}


/* Release element empty. Release allocated data if any.
 */
void eMatrix::emptyobject(
    os_char *dataptr,
    os_char *typeptr)
{
    eMatrixObj *mo;

    switch (m_datatype)
    {
        case OS_OBJECT:
            mo = (eMatrixObj*)dataptr;
            switch (*typeptr)
            {
                case OS_STRING:
                    os_free(mo->s, os_strlen(mo->s));
                    break;

                case OS_OBJECT:
                    delete mo->o;
                    break;

                default:
                    break;
            }

            os_memclear(mo, sizeof(eMatrixObj));
            *typeptr = OS_UNDEFINED_TYPE;
            break;

        case OS_CHAR:
            *(os_char*)dataptr = OS_CHAR_MAX;
            break;

        case OS_SHORT:
            *(os_short*)dataptr = OS_SHORT_MAX;
            break;

        case OS_INT:
            *(os_int*)dataptr = OS_INT_MAX;
            break;

        case OS_LONG:
            *(os_long*)dataptr = OS_LONG_MAX;
            break;

        case OS_FLOAT:
            *(os_float*)dataptr = OS_FLOAT_MAX;
            break;

        case OS_DOUBLE:
            *(os_double*)dataptr = OS_DOUBLE_MAX;
            break;
    }
}


/* How many bytes are needed for matrix datatype
 */
os_short eMatrix::typesz(
    osalTypeId datatype)
{
    if (datatype == OS_OBJECT) return sizeof(eMatrixObj);
    return osal_typeid_size(datatype);
}


