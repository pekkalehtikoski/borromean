/**

  @file    ebinding.h
  @brief   Binding properties, DB tables and files.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The binding object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EBINDING_INCLUDED
#define EBINDING_INCLUDED


/* Binding type flag values for bind()
 */
#define EBIND_PROPERTY 0
#define EBIND_TABLE 1
#define EBIND_FILE 2
#define EBIND_CONTAINER 3

/* Flags for bind()
 */
#define EBIND_DEFAULT 0
#define EBIND_CLIENTINIT 8
#define EBIND_NOFLOWCLT 16
#define EBIND_METADATA 32
#define EBIND_TEMPORARY 256
#define EBIND_CLIENT 1024 /* no need to give as argument */
#define EBIND_CHANGED 2048 /* no need to give as argument */

#define EBIND_TYPE_MASK 7
#define EBIND_SER_MASK (EBIND_TYPE_MASK|EBIND_CLIENTINIT|EBIND_NOFLOWCLT|EBIND_METADATA)

/* Binding states
 */
#define E_BINDING_UNUSED 0
#define E_BINDING_NOW 1
#define E_BINDING_OK 2

/* Enumeration of binding parameters
 */
#define E_BINDPRM_FLAGS 1


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eBinding : public eObject
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
	eBinding(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eBinding();

    /* Clone an obejct.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid oid = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eBinding pointer.
     */
	inline static eBinding *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_BINDING)
		return (eBinding*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_BINDING;
    }

    /* Static constructor function for generating instance by class list.
     */
    static eBinding *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eBinding(parent, oid, flags);
    }

    /*@}*/

	/** 
	************************************************************************************************

	  @name Virtual function overrides.

	  X... 

	************************************************************************************************
	*/
	/*@{*/
    /* Write binding content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read binding content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Virtual function overrides.

	  X... 

	************************************************************************************************
	*/
	/*@{*/

    /* Connect client eBinding to server eBinding.
     */
    void bind(
        os_char *objpath,
        os_int bflags);

    virtual void get_bind_parameters(eSet *set) {};

    /* Bind the server end.
     */
    void srvbind(
        eEnvelope *envelope);

    virtual void get_srvbind_parameters(eSet *set) {};

    /* Complete client end of binding.
     */
    void cbindok(
        eEnvelope *envelope);

    /* Mark that value has changed.
     */
    inline void changed() 
    {
        m_bflags |= EBIND_CHANGED;
    }

    void forward(
        eEnvelope *envelope);

    void sendack(
        eEnvelope *envelope);

    void ack(
        eEnvelope *envelope);

    /* Process received messages
     */
    virtual void onmessage(
        eEnvelope *envelope);

    /*@}*/

protected:

    /* Save object path.
     */
    void set_objpath(
        os_char *objpath);

    /* Set bind path.
     */
    void set_bindpath(
        os_char *bindpath);

    /* Disconnect the binding and release allocated memory.
     */
    void disconnect(
        os_boolean keep_objpath = OS_FALSE);


	/** 
	************************************************************************************************

	  @name Member variables.

	  X... 

	************************************************************************************************
	*/
	/*@{*/
        /* Client: Path to object to bind to as given as argument to bind(). 
           Server: Always OS_NULL.
         */
        os_char *m_objpath;

        /* Unique path to eBinding which we are bound to. 
         */
        os_char *m_bindpath;

        /** Binding flags.
         */
        os_short m_bflags;

        /* Size of object path allocation in bytes.
         */
        os_short m_objpathsz;

        /* Size of bind path allocation in bytes.
         */
        os_short m_bindpathsz;

        /** Number of ECMD_FWRD messages sent but have not been acknowledged.
         */
        os_char m_ackcount;

        /** Binding state, one of: E_BINDING_UNUSED (0), E_BINDING_NOW (1) or E_BINDING_OK.
         */
        os_char m_state;

    /*@}*/
};

#endif
