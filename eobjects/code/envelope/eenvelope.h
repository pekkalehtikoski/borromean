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

    inline void setcommand(os_int command) {m_command = command;}

    inline void setmflags(os_int mflags) {m_mflags = mflags;}

    /** Clear specified message flags.
     */
    inline void clearmflags(
		os_int mflags)
    {
        m_mflags &= ~mflags;
    }

    void settarget(
        os_char *target);

    void settarget(
        eVariable *target);

    void setsource(
        os_char *source);

    void setcontent(
        eObject *content,
        os_int mflags);

    void setcontext(
        eObject *context,
        os_int mflags);

    inline os_int command() {return m_command;}

    inline os_int mflags() {return m_mflags;}

    /** The eEnvelope::gettarget() function returns remaining path to destination. The target path
        gets shorter when the envelope passess throug messaging (and source path longer).
        If this is final destination, the function returns OS_NULL.
     */
    inline os_char *target()
    {
        if (m_target == OS_NULL) return "";
        return m_target + m_target_pos;
    }

    os_boolean nexttargetis(char *name);

    void nexttarget(eVariable *x);

    inline eObject *content() {return m_content;}

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


    /* Message content.
     */
    eObject *m_content;

    /* Context to be returned with reply as is.
     */
    eObject *m_context;
};

#endif
