/**

  @file    eenvelope.h
  @brief   Envelope class.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    28.12.2016

  Messages are sent as envelope objects. The eEnvelope contains recipient and senders's addresses,
  command, message content and other data.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EENVELOPE_INCLUDED
#define EENVELOPE_INCLUDED

/**
****************************************************************************************************

  @name Command identifiers for messages.

  Commend identifiers used in envelopes and in by message(), onmessage(), etc. functions.
  All command identifiers used by eobjects library are negative, range -1 ... -999 is reserved
  for these. Application may use positive command identifiers or negative command identifiers 
  smaller than -999.
 
****************************************************************************************************
*/
/*@{*/

/* Messaging, no target.
 */
#define ECMD_NO_TARGET -1

/* Set property by message
 */
#define ECMD_SETPROPERTY -19

/* Binding related commands. 
 */
#define ECMD_BIND -20
#define ECMD_BIND_REPLY -21
#define ECMD_UNBIND -22
#define ECMD_SRV_UNBIND -23
#define ECMD_REBIND -24
#define ECMD_FWRD -25
#define ECMD_ACK -26

/* Thread control, exit thread.
 */
#define ECMD_EXIT_THREAD -30

/* Timer commands (timer hit and set timer period).
 */
#define ECMD_TIMER -50
#define ECMD_SETTIMER -51


/*@}*/


/* Enumeration of envelope properties.
 */
#define EENVP_COMMAND 2
#define EENVP_TARGET 8
#define EENVP_SOURCE 10
#define EENVP_CONTENT 12
#define EENVP_CONTEXT 14

/* Variable envelope names.
 */
extern os_char
    eenvp_command[],
    eenvp_target[],
    eenvp_source[],
    eenvp_content[],
    eenvp_context[];

/* Source and target string presentations
 */
typedef struct 
{
    os_char *str;
    os_short str_pos;
    os_short str_alloc;
}
eEnvelopePath;

/* Place name in front of the path.
 */
void eenvelope_prepend_name(
    eEnvelopePath *path,
    const os_char *name);

/* Clear the path and release memory allocated for it.
 */
void eenvelope_clear_path(
    eEnvelopePath *path);


/**
****************************************************************************************************

  @brief Object base class.

  The eObject is base class for all eobject library objects. It defines basic functionality for
  networked objects.


****************************************************************************************************
*/
class eEnvelope : public eObject
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
	eEnvelope(
		eObject *parent = OS_NULL,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eEnvelope();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eEnvelope pointer.
        */
	inline static eEnvelope *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_ENVELOPE)
		return (eEnvelope*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() {return ECLASSID_ENVELOPE;}

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eEnvelope *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eEnvelope(parent, id, flags);
    }

    /* Called when property value changes.
     */
    virtual void onpropertychange(
        os_int propertynr, 
        eVariable *x, 
        os_int flags);

    /* Get value of simple property.
     */
    virtual eStatus simpleproperty(
        os_int propertynr, 
        eVariable *x);

    /* Write envelope to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read envelope from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /*@}*/

	/** 
	************************************************************************************************

	  @name Envelope functions

	  X... 

	************************************************************************************************
	*/
	/*@{*/

/* COMMAND AND FLAGS ***************************************************************************** */

    inline void setcommand(
        os_int command) 
    {
        m_command = command;
    }

    inline os_int command()
    {
        return m_command;
    }

    /** Set message flags.
     */
    inline void setmflags(
        os_int mflags) 
    {
        m_mflags = mflags;
    }

    /** Set specified message flags.
     */
    inline void addmflags(
		os_int mflags)
    {
        m_mflags |= mflags;
    }

    /** Clear specified message flags.
     */
    inline void clearmflags(
		os_int mflags)
    {
        m_mflags &= ~mflags;
    }

    /** Get message flags.
     */
    inline os_int mflags() 
    {
        return m_mflags;
    }


/* TARGET **************************************************************************************** */

    inline void settarget(
        const os_char *target)
    {
        osal_debug_assert(m_target.str == OS_NULL);
        eenvelope_prepend_name(&m_target, target);
    }

    void settarget(
        eVariable *target);

    /** The eEnvelope::gettarget() function returns remaining path to destination. The target path
        gets shorter when the envelope passess throug messaging (and source path longer).
        If this is final destination, the function returns OS_NULL.
     */
    inline os_char *target()
    {
        if (m_target.str == OS_NULL) return (os_char*)"";
        return m_target.str + m_target.str_pos;
    }

    /* Get next name from target string.
     */
    void nexttarget(
        eVariable *x);

    inline void move_target_pos(
        os_short nchars) 
    {
        m_target.str_pos += nchars;
    }

    inline void move_target_over_objname(
        os_short objname_nchars) 
    {
        m_target.str_pos += objname_nchars; 
        if (m_target.str) if (m_target.str[m_target.str_pos] == '/') m_target.str_pos++;
    }

    /* Prepend target with with name
     */
    inline void prependtarget(
        const os_char *name)
    {
        eenvelope_prepend_name(&m_target, name);
    }

//    os_boolean nexttargetis(char *name);


/* SOURCE **************************************************************************************** */

    /* Prepend target with with name
     */
    inline void prependsource(
        const os_char *name)
    {
        eenvelope_prepend_name(&m_source, name);
    }

    /* void appendsource(
        const os_char *source); */

    void prependsourceoix(
        eObject *o);

    inline os_char *source()
    {
        if (m_source.str == OS_NULL) return (os_char*)"";
        return m_source.str + m_source.str_pos;
    }

    /* os_char *lastsource()
    {
        os_char *p, *e;
        p = source();
        e = os_strechr(p, '/');
        return e ? e + 1 : p;
    } */

 
/* CONTENT AND CONTEXT ************************************************************************** */

    void setcontent(
        eObject *o,
        os_int mflags);

    void setcontext(
        eObject *o,
        os_int mflags);

    inline eObject *content() 
    {
        return first(EOID_CONTENT);
    }

    inline eObject *context() 
    {
        return first(EOID_CONTEXT);
    }

    /*@}*/

private:
    /** Command.
     */
    os_int m_command;

    /* Combination of bits EMSG_KEEP_CONTENT (0), EMGS_NO_REPLIES, EMSG_NO_RESOLVE, 
       EMSG_NO_NEW_SOURCE_OIX, EMSG_NO_ERROR_MSGS and EMSG_INTERTHREAD.
    */
    os_short m_mflags;

    eEnvelopePath m_target;

    eEnvelopePath m_source;
};

#endif
