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

/** Reply to attemp to sens message to invalid path.
 */

#define ECMD_EXIT_THREAD -30

#define ECMD_NO_TARGET -50

/*@}*/


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

	  @name Constructor, destructor, etc.

	  X...

	************************************************************************************************
	*/
	/*@{*/
public:
    /** Constructor.
	 */
	eEnvelope(
		eObject *parent = OS_NULL,
		e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~eEnvelope();

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

    /* Static constructor function for generating instance by class list.
     */
    static eEnvelope *newobj(
        eObject *parent,
        e_oid oid = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eEnvelope(parent, oid, flags);
    }

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

    void settarget(
        os_char *target);

    void settarget(
        eVariable *target);

    /** The eEnvelope::gettarget() function returns remaining path to destination. The target path
        gets shorter when the envelope passess throug messaging (and source path longer).
        If this is final destination, the function returns OS_NULL.
     */
    inline os_char *target()
    {
        if (m_target == OS_NULL) return "";
        return m_target + m_target_pos;
    }

    /* Get next name from target string.
     */
    void nexttarget(eVariable *x);

    inline void move_target_pos(
        os_short nchars) 
    {
        m_target_pos += nchars;
    }

    inline void move_target_over_objname(
        os_short objname_nchars) 
    {
        m_target_pos += objname_nchars; 
        if (m_target[m_target_pos] == '/') m_target_pos++;
    }

//    os_boolean nexttargetis(char *name);


/* SOURCE **************************************************************************************** */

    void appendsource(
        os_char *source);

    void appendsourceoix(
        eObject *o);

    inline os_char *source()
    {
        if (m_source == OS_NULL) return "";
        return m_source;
    }

 
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

    os_short m_mflags;

    os_short m_target_pos;
    os_short m_source_end;
    os_short m_source_alloc;

    /* Target path.
     */
    os_char *m_target;

    os_int m_target_alloc;

    /* Source path.
     */
    os_char *m_source;
};

#endif
