/**

  @file    epropertybinding.h
  @brief   Simple object propertybinding.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  The propertybinding object is like a box holding a set of child objects.

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EPROPERTYBINDING_INCLUDED
#define EPROPERTYBINDING_INCLUDED

/**
****************************************************************************************************

  @brief Property binding class.

  The ePropertyBinding is class derived from eBinding. It implements property binding specific
  functionality.


****************************************************************************************************
*/
class ePropertyBinding : public eBinding
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
	ePropertyBinding(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
	virtual ~ePropertyBinding();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to ePropertyBinding pointer.
     */
	inline static ePropertyBinding *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_PROPERTY_BINDING)
		return (ePropertyBinding*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_PROPERTY_BINDING;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static ePropertyBinding *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new ePropertyBinding(parent, id, flags);
    }

    /* Write propertybinding content to stream.
     */
    virtual eStatus writer(
        eStream *stream, 
        os_int flags);

    /* Read propertybinding content from stream.
     */
    virtual eStatus reader(
        eStream *stream, 
        os_int flags);

    /* Process received messages
     */
    virtual void onmessage(
        eEnvelope *envelope);

    /*@}*/


	/** 
	************************************************************************************************

	  @name Property binding functions

	  These functions implement property finding functionality.

	************************************************************************************************
	*/
	/*@{*/

    /* Bind property.
     */
    void bind(
        os_int localpropertynr,
        const os_char *remotepath,
        const os_char *remoteproperty,
        os_int bflags);

    void bind2(
        const os_char *remotepath);

    /* Bind the server end.
     */
    void srvbind(
        eObject *obj,
        eEnvelope *envelope);

    /* Mark property value changed (may forward it immediately).
     */
    void changed(
        os_int propertynr,
        eVariable *x,
        os_boolean delete_x);

protected:

    /* Finish the client end of binding.
     */
    void cbindok(
        eObject *obj,
        eEnvelope *envelope);

    /* Cirtual function to forward property value trough binding.
     */
    virtual void forward(
        eVariable *x = OS_NULL,
        os_boolean delete_x = OS_FALSE);


    /* Update to property value has been received.
     */
    void update(
        eEnvelope *envelope);

    void sendack(
        eEnvelope *envelope);

    void ack(
        eEnvelope *envelope);

    /* Set object's property from binding.
     */
    os_boolean binding_setproperty(
        eVariable *x);

    /* Get object's property value from binding.
     */
    os_boolean binding_getproperty(
        eVariable *x);

    /* List attributes (subproperties like "x.min") for the property.
     */
    os_boolean listattr(
        os_int propertynr,
        eVariable *x);

    /* Save property name.
     */
    void set_propertyname(
        const os_char *propertyname);

    /*@}*/

    /** 
	************************************************************************************************

	  @name Member variables.

	  The member variables hold information where to bind (for client binding) and current 
      binding state. 

	************************************************************************************************
	*/
	/*@{*/

    /** Client: Path to peropery name on remote object to bind to.
        Server: Always OS_NULL.
     */
    os_char *m_propertyname;

    /** Size of property name allocation in bytes.
     */
    os_short m_propertynamesz;

    /** Which property of local object is bound.
     */
    os_int m_localpropertynr;

    
    /*@}*/

};

#endif
