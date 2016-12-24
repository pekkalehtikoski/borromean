/**

  @file    estream.h
  @brief   Virtual stream base class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    17.5.2016

  Stream base class.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ESTREAM_INCLUDED
#define ESTREAM_INCLUDED


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eStream : public eObject
{
	/** 
	************************************************************************************************

	  @name Constructors and destructor

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
		/** Constructor.
		 */
		eStream(
			eObject *parent = OS_NULL,
			e_oid oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT);

		/* Virtual destructor.
		 */
		virtual ~eStream();

        /* Casting eObject pointer to eStream pointer.
         */
		inline static eStream *cast(
			eObject *o) 
		{ 
			return (eStream*)o;
		}

		/* Get class identifier.
		*/
		virtual os_int getclassid() { return ECLASSID_STREAM; }

		/* Static constructor function.
		*/
		static eStream *newobj(
			eObject *parent,
			e_oid oid = EOID_ITEM,
			os_int flags = EOBJ_DEFAULT)
		{
			return new eStream(parent, oid, flags);
		}

    /*@}*/


	/** 
	************************************************************************************************

	  @name Stream functions for writing to stream.

	  X...

	************************************************************************************************
	*/
	/*@{*/
        virtual eStatus open(
            os_char *path, 
            os_int flags=0) {return ESTATUS_SUCCESS;}

        virtual eStatus close() {return ESTATUS_SUCCESS;}

        virtual eStatus flush() {return ESTATUS_SUCCESS;}

        virtual eStatus write(os_char *buf, os_long buf_sz, os_long *nwritten = OS_NULL)
            {if (nwritten != OS_NULL) *nwritten = 0; return ESTATUS_SUCCESS;}

        virtual eStatus read(os_char *buf, os_long buf_sz, os_long *nread = OS_NULL)
            {if (nread != OS_NULL) *nread = 0; return ESTATUS_SUCCESS;}

		/** Begin an object, etc. block. This is for versioning, block size may be changed.
		 */
        virtual eStatus write_begin_block(
            os_int version) {return ESTATUS_SUCCESS;}

		/** End an object, etc. block. This skips data added by later versions of object.
		 */
        virtual eStatus write_end_block() {return ESTATUS_SUCCESS;}

        virtual eStatus read_begin_block(
            os_int& version) {return ESTATUS_SUCCESS;}

		/** End an object, etc. block. This skips data added by later versions of object.
		 */
        virtual eStatus read_end_block() {return ESTATUS_SUCCESS;}

		/* Store long integer value to stream.
		 */
		eStatus setl(
            const os_long x);

        /* Store float value to stream.
         */
        eStatus setf(
	        const os_double x);

        /* Store double value to stream.
         */
        eStatus setd(
	        const os_double x);

        /* Set string value to variable.
         */
        eStatus sets(
	        const os_char *x);

		/* Set variable value as string.
		 */
        eStatus sets(
	        eVariable *x);

		/* Get long integer value from stream.
		 */
		eStatus getl(
            os_long *x);

		/* Get float value from stream.
		 */
		eStatus getf(
            os_float *x);

		/* Get double value from stream.
		 */
		eStatus getd(
            os_double *x);

		/* Get string value from stream into variable.
		 */
        eStatus gets(
            eVariable *x);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Operator overloads

	  The operator overloads are implemented for convinience, and map to the member functions. 
	  Using operator overloads may lead to more readable code, but may also confuse the
	  reader. 

	************************************************************************************************
	*/
	/*@{*/

        /** Operator "<<", storing values to stream.
         */
        inline eStatus operator<<(const os_char x) { return setl(x); }
        inline eStatus operator<<(const os_uchar x) { return setl(x); }
        inline eStatus operator<<(const os_short x) { return setl(x); }
        inline eStatus operator<<(const os_ushort x) { return setl(x); }
        inline eStatus operator<<(const os_int x) { return setl(x); }
        inline eStatus operator<<(const os_uint x) { return setl(x); }
        inline eStatus operator<<(const os_long x) { return setl(x); }
        inline eStatus operator<<(const os_float x) { return setf(x); }
        inline eStatus operator<<(const os_double x) { return setd(x); }
        inline eStatus operator<<(const os_char *x) { return sets(x); }
        inline eStatus operator<<(eVariable& x) { return sets(&x); }

        /** Operator ">>", getting values from stream.
         */
        inline eStatus operator>>(os_char& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_char)tx; return s; }
        inline eStatus operator>>(os_uchar& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_uchar)tx; return s; }
        inline eStatus operator>>(os_short& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_short)tx; return s; }
        inline eStatus operator>>(os_ushort& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_ushort)tx; return s; }
        inline eStatus operator>>(os_int& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_int)tx; return s; }
        inline eStatus operator>>(os_uint& x) { os_long tx; eStatus s; s = getl(&tx); x=(os_uint)tx; return s; }
        inline eStatus operator>>(os_long& x) { return getl(&x); }
        inline eStatus operator>>(os_float& x) { return getf(&x); }
        inline eStatus operator>>(os_double& x) { return getd(&x); }
        inline eStatus operator>>(eVariable& x) { return gets(&x); }

    /*@}*/

};

#endif
