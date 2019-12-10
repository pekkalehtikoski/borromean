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


/**
****************************************************************************************************

  @brief Clone object

  The eMatrix::clone function clones an eMatrix.

  @param  parent Parent for the clone.
  @param  id Object identifier for the clone.
  @param  aflags 0 for default operation. EOBJ_NO_MAP not to map names.
  @return Pointer to the clone.

****************************************************************************************************
*/
eObject *eMatrix::clone(
    eObject *parent,
    e_oid id,
    os_int aflags)
{
    eMatrix *clonedobj;
    eVariable *tmp;
    os_int row, column;

    clonedobj = new eMatrix(parent, id == EOID_CHILD ? oid() : id, flags());
    tmp = new eVariable(this);

    /* Slightly slow but simple clone. Optimize later if time.
     */
    clonedobj->allocate(m_datatype, m_nrows, m_ncolumns);
    for (row = 0; row < m_nrows; row++)
    {
        for (column = 0; column < m_ncolumns; column++)
        {
            if (getv(row, column, tmp))
            {
                clonedobj->setv(row, column, tmp);
            }
        }
    }

    clonegeneric(clonedobj, aflags);

    delete tmp;
    return clonedobj;
}


/**
****************************************************************************************************

  @brief Write matrix to stream.

  The eMatrix::writer() function serializes the eBuffer to stream. This writes only the
  content, use eObject::write() to save also class information, attachements, etc.

  @param  stream The stream to write to.
  @param  sflags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_WRITING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eMatrix::writer(
    eStream *stream,
    os_int sflags)
{
    eBuffer *buffer;
    os_char *dataptr, *typeptr;
    os_long l;
    os_double d;
    os_float f;
    e_oid id;
    os_int first_elem_ix, elem_ix, first_full_ix, full_count, i;
    os_boolean prev_isempty, isempty;

    /* Version number. Increment if new serialized items are added to the object,
       and check for new version's items in read() function.
     */
    const os_int version = 0;

    /* Begin the object and write version number.
     */
    if (stream->write_begin_block(version)) goto failed;

    /* Write matrix data type and size.
     */
    if (stream->putl(m_datatype)) goto failed;
    if (stream->putl(m_nrows)) goto failed;
    if (stream->putl(m_ncolumns)) goto failed;

    /* Write data as "full groups".
     */
    prev_isempty = OS_TRUE;
    first_full_ix = full_count = 0;

    for (buffer = eBuffer::cast(first());
         buffer;
         buffer = eBuffer::cast(buffer->next()))
    {
        id = buffer->oid();
        if (id <= 0) continue;

        first_elem_ix = (id - 1) * m_elems_per_block;

        dataptr = buffer->ptr();
        typeptr = dataptr + m_elems_per_block * m_typesz;

        for (i = 0; i < m_elems_per_block; i++)
        {
            elem_ix = first_elem_ix + i;

            /* If element is empty
             */
            isempty = OS_TRUE;
            switch (m_datatype)
            {
                case OS_OBJECT:
                    switch (typeptr[i])
                    {
                        case OS_LONG:
                        case OS_DOUBLE:
                        case OS_STR:
                        case OS_OBJECT:
                            isempty = OS_FALSE;
                        break;

                        default:
                            break;
                    }
                    break;

                case OS_CHAR:
                    l = *((os_char*)dataptr);
                    isempty = (os_boolean)(l == OS_CHAR_MAX);
                    break;

                case OS_SHORT:
                    l = *((os_short*)dataptr);
                    isempty = (os_boolean)(l == OS_SHORT_MAX);
                    break;

                case OS_INT:
                    l = *((os_int*)dataptr);
                    isempty = (os_boolean)(l == OS_INT_MAX);
                    break;

                case OS_LONG:
                    l = *((os_long*)dataptr);
                    isempty = (os_boolean)(l == OS_INT_MAX);
                    break;

                case OS_FLOAT:
                    f = *((os_float*)dataptr);
                    isempty = (os_boolean)(f == OS_FLOAT_MAX);
                    break;

                case OS_DOUBLE:
                    d = *((os_double*)dataptr);
                    isempty = (os_boolean)(d == OS_DOUBLE_MAX);
                    break;

                default:
                    break;
            }

            if (isempty)
            {
                if (!prev_isempty)
                {
                    if (elementwrite(stream, first_full_ix,
                        full_count, sflags)) goto failed;
                    prev_isempty = OS_TRUE;
                }
            }
            else
            {
                if (prev_isempty)
                {
                    first_full_ix = elem_ix;
                    full_count = 1;
                    prev_isempty = OS_FALSE;
                }
                else
                {
                    full_count++;
                }
            }
        }
    }

    /* Last group.
     */
    if (!prev_isempty)
    {
        if (elementwrite(stream, first_full_ix, full_count, sflags))
            goto failed;
    }

    /* Write -1 to indicate end of data.
     */
    if (stream->putl(-1)) goto failed;

    /* End the object.
     */
    if (stream->write_end_block()) goto failed;

    /* Object succesfully written.
     */
    return ESTATUS_SUCCESS;

    /* Writing object failed.
     */
failed:
    return ESTATUS_WRITING_OBJ_FAILED;
}

/* Write consequent non-empty matrix elements to stream.
 */
eStatus eMatrix::elementwrite(
    eStream *stream,
    os_int first_full_ix,
    os_int full_count,
    os_int sflags)
{
    eBuffer *buffer = OS_NULL;
    eMatrixObj *mo;
    eObject *o;
    os_char *s, *dataptr, *typeptr;
    os_long l;
    os_double d;
    os_float f;
    osalTypeId datatype;
    os_int i, prev_buffer_nr, buffer_nr, elem_ix;

    if (stream->putl(first_full_ix)) goto failed;
    if (stream->putl(full_count)) goto failed;

    prev_buffer_nr = -1;

    for (i = 0; i<full_count; i++)
    {
        elem_ix = first_full_ix + i;
        buffer_nr =  elem_ix / m_elems_per_block + 1;
        if (buffer_nr != prev_buffer_nr)
        {
            buffer = eBuffer::cast(first(buffer_nr));
            if (buffer == OS_NULL)
            {
                osal_debug_error("ematrix.cpp: progerr 1.");
                return ESTATUS_FAILED;
            }

            prev_buffer_nr = buffer_nr;
        }

//        dataptr = buffer->ptr();
//        typeptr = dataptr + m_elems_per_block * m_typesz;          HERE IS SOMETHING WERY WRONG

        datatype = OS_UNDEFINED_TYPE;
        switch (m_datatype)
        {
            case OS_OBJECT:
                mo = ((eMatrixObj*)dataptr) + i;
                switch (typeptr[i])
                {
                    case OS_LONG:
                        l = mo->l;
                        datatype = OS_LONG;
                        break;

                    case OS_DOUBLE:
                        l = mo->l;
                        datatype = OS_DOUBLE;
                        break;

                    case OS_STR:
                        s = mo->s;
                        datatype = OS_STR;
                        break;

                    case OS_OBJECT:
                        o = mo->o;
                        datatype = OS_OBJECT;
                        break;

                    default:
                        break;
                }
                break;

            case OS_CHAR:
                l = *((os_char*)dataptr);
                datatype = OS_LONG;
                break;

            case OS_SHORT:
                l = *((os_short*)dataptr);
                datatype = OS_LONG;
                break;

            case OS_INT:
                l = *((os_int*)dataptr);
                datatype = OS_LONG;
                break;

            case OS_LONG:
                l = *((os_long*)dataptr);
                datatype = OS_LONG;
                break;

            case OS_FLOAT:
                f = *((os_float*)dataptr);
                datatype = OS_FLOAT;
                break;

            case OS_DOUBLE:
                d = *((os_double*)dataptr);
                datatype = OS_DOUBLE;
                break;

            default:
                break;
        }

        if (m_datatype == OS_OBJECT)
        {
            if (stream->putl(datatype)) goto failed;
        }

        switch (datatype)
        {
            case OS_LONG:
                if (stream->putl(l)) goto failed;
                break;

            case OS_FLOAT:
                if (stream->putf(f)) goto failed;
                break;

            case OS_DOUBLE:
                if (stream->putd(d)) goto failed;
                break;

            case OS_STR:
                osal_debug_assert(s);
                if (stream->puts(s)) goto failed;
                break;

            case OS_OBJECT:
                osal_debug_assert(o);
                if (o->write(stream, sflags)) goto failed;
                break;

            default:
                osal_debug_error("ematrix.cpp: progerr 2.");
                break;
        }
    }


    /* Object succesfully written.
     */
    return ESTATUS_SUCCESS;

    /* Writing object failed.
     */
failed:
    return ESTATUS_WRITING_OBJ_FAILED;
}


/**
****************************************************************************************************

  @brief Read a matrix from stream.

  The eMatrix::reader() function reads serialized eBuffer from stream. This function
  reads only the object content. To read whole object including attachments, names, etc,
  use eObject::read().

  @param  stream The stream to read from.
  @param  sflags Serialization flags.

  @return If successfull the function returns ESTATUS_SUCCESS (0). If writing object to stream
          fails, value ESTATUS_READING_OBJ_FAILED is returned. Assume that all nonzero values
          indicate an error.

****************************************************************************************************
*/
eStatus eMatrix::reader(
    eStream *stream,
    os_int sflags)
{
    /* Version number. Used to check which versions item's are in serialized data.
     */
    eObject *o;
    eVariable tmp;
    os_long datatype, nrows, ncolumns, first_full_ix, full_count, l;
    os_double d;
    os_float f;
    os_int version, elem_ix, i, row, column;

    /* If we have old data, delete it.
     */
    clear();

    /* Read object start mark and version number.
     */
    if (stream->read_begin_block(&version)) goto failed;

    /* Read matrix data type and size, allocate matrix.
     */
    if (stream->getl(&datatype)) goto failed;
    if (stream->getl(&nrows)) goto failed;
    if (stream->getl(&ncolumns)) goto failed;
    allocate((osalTypeId)datatype, (os_int)nrows, (os_int)ncolumns);

    /* Read data
     */
    while (OS_TRUE)
    {
        /* Read first full index. -1 indicates end of data.
         */
        if (stream->getl(&first_full_ix)) goto failed;
        if (first_full_ix == -1) break;

        /* Read number of full elements.
         */
        if (stream->getl(&full_count)) goto failed;

        /* Read elements
         */
        for (i = 0; i<first_full_ix; i++)
        {
            elem_ix = (os_int)first_full_ix + i;
            row = elem_ix / m_ncolumns;
            column = elem_ix % m_ncolumns;

            /* If we have datatype, read it.
             */
            if (m_datatype == OS_OBJECT)
            {
                if (stream->getl(&datatype)) goto failed;
            }

            switch (datatype)
            {
                case OS_LONG:
                    if (stream->getl(&l)) goto failed;
                    setl(row, column, l);
                    break;

                case OS_FLOAT:
                    if (stream->getf(&f)) goto failed;
                    setd(row, column, f);               // SHOULD SETF BE IMPLEMETED
                    break;

                case OS_DOUBLE:
                    if (stream->getd(&d)) goto failed;
                    setd(row, column, d);
                    break;

                case OS_STR:
                    if (stream->gets(&tmp)) goto failed;
                    setv(row, column, &tmp);
                    break;

                case OS_OBJECT:
                    o = read(stream, sflags);
                    seto(row, column, o);  // HERE WE SHOULD USE ADOPT
                    delete o;
                    break;
            }
        }
    }

    /* End the object.
     */
    if (stream->read_end_block()) goto failed;

    /* Object succesfully read.
     */
    return ESTATUS_SUCCESS;

    /* Reading object failed.
     */
failed:
    return ESTATUS_READING_OBJ_FAILED;
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
void eMatrix::setv(
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

        case OS_STR:
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
            *((os_long*)dataptr) = eround_double_to_long(x);
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


/**
****************************************************************************************************

  @brief Store string value into matrix.

  The eMatrix::sets stores string value to matrix. Row and column specify
  position in matrix. The first row is 0 and first columns is 0. The matrix is expanded if
  row/column specify position outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Value to store.
  @return None.

****************************************************************************************************
*/
void eMatrix::sets(
    os_int row,
    os_int column,
    const os_char *x)
{
    os_char *dataptr, *typeptr, *p;
    os_long l;
    os_memsz sz;

    if (x == 0) x = "";
    if (*x == '\0')
    {
        clear(row, column);
        return;
    }

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
            sz = os_strlen(x);
            p = os_malloc(sz, OS_NULL);
            os_memcpy(p, x, sz);
            ((eMatrixObj*)dataptr)->s = p;
            *typeptr = OS_DOUBLE;
            break;

        case OS_CHAR:
        case OS_SHORT:
        case OS_INT:
        case OS_LONG:
            /* Convert string to integer.
             */
            l = osal_str_to_int(x, OS_NULL);
            setl(row, column, l);
            break;

        case OS_FLOAT:
        case OS_DOUBLE:
            /* Convert string to float.
             */
            setd(row, column, osal_string_to_double(x, OS_NULL));
            break;

        default:
            break;
    }
}


/**
****************************************************************************************************

  @brief Store object into matrix.

  The eMatrix::seto stores object to matrix. Row and column specify position in matrix.
  The first row is 0 and first columns is 0. The matrix is expanded if row/column specify
  position outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Value to store.
  @param  mflags Reserved for future, set 0 for now.
  @return None.

****************************************************************************************************
*/
void eMatrix::seto(
    os_int row,
    os_int column,
    eObject *x,
    os_int mflags)
{
    os_char *dataptr, *typeptr;
    eBuffer *buffer;
    eObject *o;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return;

    /* Objects can be stored only if matrix data type is OS_OBJECT.
     */
    if (m_datatype != OS_OBJECT)
    {
        osal_debug_error("ematrix.cpp: cannot store object to matrix with fixed type.");
        return;
    }

    dataptr = getptrs(row, column, &typeptr, OS_TRUE, &buffer);
    if (dataptr == OS_NULL) return;

    o = x->clone(buffer, EOID_INTERNAL);
    o->setflags(EOBJ_IS_ATTACHMENT|EOBJ_NOT_CLONABLE|EOBJ_NOT_SERIALIZABLE);
    ((eMatrixObj*)dataptr)->o = o;
    *typeptr = OS_OBJECT;
}


/**
****************************************************************************************************

  @brief Clear matrix element.

  The eMatrix::clear removed value from matrix, so that elemet is empty. Row and column specify
  position in matrix. The first row is 0 and first columns is 0. The matrix is expanded if
  row/column specify position outside current matrix size.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @return None.

****************************************************************************************************
*/
void eMatrix::clear(
    os_int row,
    os_int column)
{
    os_char *dataptr, *typeptr;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_FALSE);
    if (dataptr == OS_NULL) return;

    emptyobject(dataptr, typeptr);
}


/**
****************************************************************************************************

  @brief Get value from matrix.

  The eMatrix::get retrieves value from matrix. Row and column specify position in
  matrix. The first row is 0 and first columns is 0. If row/column specify position outside
  current matrix size, hasvalue is set to OS_FALSE and function returns zero.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  x Variable into which to store the value.
  @return OS_TRUE if value was retrieved, os OS_FALSE if value is empty.

****************************************************************************************************
*/
os_boolean eMatrix::getv(
    os_int row,
    os_int column,
    eVariable *x)
{
    os_char *dataptr, *typeptr;
    eMatrixObj *mo;
    os_long l;
    os_double d;
    os_float f;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) goto return_empty;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_FALSE);
    if (dataptr == OS_NULL) goto return_empty;

    switch (m_datatype)
    {
        case OS_OBJECT:
            mo = (eMatrixObj*)dataptr;
            switch (*typeptr)
            {
                case OS_LONG:
                    x->setl(mo->l);
                    break;

                case OS_DOUBLE:
                    x->setd(mo->d);
                    break;

                case OS_STR:
                    x->sets(mo->s);
                    break;

                case OS_OBJECT:
                    x->seto(mo->o);
                    break;

                default:
                    goto return_empty;
            }
            break;

        case OS_CHAR:
            l = *((os_char*)dataptr);
            if (l == OS_CHAR_MAX) goto return_empty;
            x->setl(l);
            break;

        case OS_SHORT:
            l = *((os_short*)dataptr);
            if (l == OS_SHORT_MAX) goto return_empty;
            x->setl(l);
            break;

        case OS_INT:
            l = *((os_int*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            x->setl(l);
            break;

        case OS_LONG:
            l = *((os_long*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            x->setl(l);
            break;

        case OS_FLOAT:
            f = *((os_float*)dataptr);
            if (f == OS_FLOAT_MAX) goto return_empty;
            x->setd(f);
            break;

        case OS_DOUBLE:
            d = *((os_double*)dataptr);
            if (d == OS_DOUBLE_MAX) goto return_empty;
            x->setd(d);
            break;

        default:
            goto return_empty;
    }
    return OS_TRUE;

return_empty:
    x->clear();
    return OS_FALSE;
}


/**
****************************************************************************************************

  @brief Get integer value from matrix.

  The eMatrix::getl retrieves an integer value from matrix. Row and column specify position in
  matrix. The first row is 0 and first columns is 0. If row/column specify position outside
  current matrix size, hasvalue is set to OS_FALSE and function returns zero.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  hasvalue Set to OS_TRUE if matrix element has value, ot to oe_FALSE if element is empty.
  @return Value.

****************************************************************************************************
*/
os_long eMatrix::getl(
    os_int row,
    os_int column,
    os_boolean *hasvalue)
{
    os_char *dataptr, *typeptr;
    eMatrixObj *mo;
    os_long l;
    os_double d;
    os_float f;

    if (hasvalue) *hasvalue = OS_FALSE;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return 0;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_FALSE);
    if (dataptr == OS_NULL) return 0;

    switch (m_datatype)
    {
        case OS_OBJECT:
            mo = (eMatrixObj*)dataptr;
            switch (*typeptr)
            {
                case OS_LONG:
                    l = mo->l;
                    break;

                case OS_DOUBLE:
                    l = eround_double_to_long(mo->d);
                    break;

                case OS_STR:
                    l = osal_str_to_int(mo->s, OS_NULL);
                    break;

                default:
                    goto return_empty;
            }
            break;

        case OS_CHAR:
            l = *((os_char*)dataptr);
            if (l == OS_CHAR_MAX) goto return_empty;
            break;

        case OS_SHORT:
            l = *((os_short*)dataptr);
            if (l == OS_SHORT_MAX) goto return_empty;
            break;

        case OS_INT:
            l = *((os_int*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            break;

        case OS_LONG:
            l = *((os_long*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            break;

        case OS_FLOAT:
            f = *((os_float*)dataptr);
            if (f == OS_FLOAT_MAX) goto return_empty;
            l = eround_float_to_long(f);
            break;

        case OS_DOUBLE:
            d = *((os_double*)dataptr);
            if (d == OS_DOUBLE_MAX) goto return_empty;
            l = eround_double_to_long(d);
            break;

        default:
            goto return_empty;
    }

    if (hasvalue) *hasvalue = OS_TRUE;
    return l;

return_empty:
    return 0;
}


/**
****************************************************************************************************

  @brief Get double value from matrix.

  The eMatrix::getd retrieves a double precifion floating point number from matrix. Row and
  column specify position in matrix. The first row is 0 and first columns is 0. If row/column
  specify position outside current matrix size, hasvalue is set to OS_FALSE and function returns
  zero.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  hasvalue Set to OS_TRUE if matrix element has value, ot to oe_FALSE if element is empty.
  @return Value.

****************************************************************************************************
*/
os_double eMatrix::getd(
    os_int row,
    os_int column,
    os_boolean *hasvalue)
{
    os_char *dataptr, *typeptr;
    eMatrixObj *mo;
    os_long l;
    os_double d;
    os_float f;

    if (hasvalue) *hasvalue = OS_FALSE;

    /* Make sure that row and column are not negative.
     */
    if (checknegative(row, column)) return 0;

    /* Get pointer to data and if matrix data type is OS_OBJECT, also to
       data type of item.
     */
    dataptr = getptrs(row, column, &typeptr, OS_FALSE);
    if (dataptr == OS_NULL) return 0;

    switch (m_datatype)
    {
        case OS_OBJECT:
            mo = (eMatrixObj*)dataptr;
            switch (*typeptr)
            {
                case OS_LONG:
                    d = (os_double)mo->l;
                    break;

                case OS_DOUBLE:
                    d = mo->d;
                    break;

                case OS_STR:
                    d = osal_string_to_double(mo->s, OS_NULL);
                    break;

                default:
                    goto return_empty;
            }
            break;

        case OS_CHAR:
            l = *((os_char*)dataptr);
            if (l == OS_CHAR_MAX) goto return_empty;
            d = (os_double)l;
            break;

        case OS_SHORT:
            l = *((os_short*)dataptr);
            if (l == OS_SHORT_MAX) goto return_empty;
            d = (os_double)l;
            break;

        case OS_INT:
            l = *((os_int*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            d = (os_double)l;
            break;

        case OS_LONG:
            l = *((os_long*)dataptr);
            if (l == OS_INT_MAX) goto return_empty;
            d = (os_double)l;
            break;

        case OS_FLOAT:
            f = *((os_float*)dataptr);
            if (f == OS_FLOAT_MAX) goto return_empty;
            d = f;
            break;

        case OS_DOUBLE:
            d = *((os_double*)dataptr);
            if (d == OS_DOUBLE_MAX) goto return_empty;
            break;

        default:
            goto return_empty;
    }

    if (hasvalue) *hasvalue = OS_TRUE;
    return d;

return_empty:
    return 0;
}


/**
****************************************************************************************************

  @brief Resize the matrix.

  The eMatrix::resize function changes matrix size. Data in matrix is preserved.

  @param  nrows New number of rows.
  @param  ncolumns New number of columns.
  @return None.

****************************************************************************************************
*/
void eMatrix::resize(
    os_int nrows,
    os_int ncolumns)
{
    eBuffer *buffer, *nextbuffer;
    eVariable *tmp;
    eMatrix *m;
    os_int elem_ix, buffer_nr, minrows, mincolumns, row, column;

    /* If we need to reorganize, do it the hard way. This is slow, application
       should be written in such way that this is not needed repeatedly.
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
                if (getv(row, column, tmp))
                {
                    m->setv(row, column, tmp);
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


/**
****************************************************************************************************

  @brief Get pointer to data.

  The eMatrix::getptrs function retrieves pointer to data for element and if m_type is
  OS_OBJECT also element type pointer.

  If isset is OS_TRUE: New eBuffer objects are allocated as needed. The matrix is expanded
  if row/column is outside the matrix and matrix element is emptied.

  If isset is OS_FALSE: No new eBuffer objects are allocated, matrix is not expanded and
  the element value is not emptied.

  @param  row Row number, 0...
  @param  column Column number, 0...
  @param  typeptr Where to store pointer to data type.
  @param  isset Controls how this function works.
  @param  pbuffer Pointer to eBuffer containing the matrix element is stored here.
          OS_NULL if not needed.
  @return Pointer to element data.

****************************************************************************************************
*/
os_char *eMatrix::getptrs(
    os_int row,
    os_int column,
    os_char **typeptr,
    os_boolean isset,
    eBuffer **pbuffer)
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
    if (pbuffer) *pbuffer = buffer;
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
        m_elems_per_block = (os_int)(buffer->allocated() / bytes_per_elem);
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
                case OS_STR:
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
                case OS_STR:
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
    return (os_short)osal_typeid_size(datatype);
}
