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
#define EBIND_INTERTHREAD 4096 /* no need to give as argument */
// #define EBIND_XXX 8192

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
#define E_BINDPRM_PROPERTYNAME 2
#define E_BINDPRM_VALUE 3

/* Maximum number of forwards befoew waiting for acknowledge.
 */
#define EBIND_MAX_ACK_COUNT 3



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

    /* Clone object.
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
        osal_debug_error("ebinding, newobj(): Cannot create abstract class.");
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
        eSet *parameters);

    virtual void get_bind_parameters(eSet *set) {};

    /* Virtual base class to bind the server end.
     */
    virtual void srvbind(
        eObject *obj,
        eEnvelope *envelope) {};

    /* Base classs function to bind the server end.
     */
    void srvbind_base(
        eEnvelope *envelopee,
        eObject *reply);

    /* Virtual base class to complete client end of binding.
     */
    virtual void cbindok(
        eObject *obj,
        eEnvelope *envelope) {};

    /* Complete the client end of binding by base class.
     */
    void cbindok_base(
        eEnvelope *envelope);

    /* Mark property value, etc changed.
     */
    inline void setchanged() 
    {
        m_bflags |= EBIND_CHANGED;
    }

    /* Mark property value, has not been changed after forwarding it.
     */
    inline void forwarddone() 
    {
        m_bflags &= ~EBIND_CHANGED;
        m_ackcount++;
    }

    /* Check if property value should be fowrarded now?
     */
    inline os_int forwardnow() 
    {
        return (m_bflags & EBIND_CHANGED) && 
                m_state == E_BINDING_OK &&
                (m_ackcount < EBIND_MAX_ACK_COUNT || 
                 (m_bflags & EBIND_NOFLOWCLT) || 
                 (m_bflags & EBIND_INTERTHREAD) == 0);
    }

    /* Cirtual function to forward property value trough binding.
     */
    virtual void forward(
        eVariable *x = OS_NULL,
        os_boolean delete_x = OS_FALSE) {}

    /* Update to property value has been received.
     */
    virtual void update(
        eEnvelope *envelope) {}

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
