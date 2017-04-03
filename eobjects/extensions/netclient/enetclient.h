/**

  @file    enetclient.h
  @brief   enet client implementation.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef ENETCLIENT_INCLUDED
#define ENETCLIENT_INCLUDED

/**
****************************************************************************************************

  @brief eNetClient class.
  X...

****************************************************************************************************
*/
class eNetClient : public eThread
{
public:
    /* Constructor.
	 */
    eNetClient(
		eObject *parent = OS_NULL,
        e_oid id = EOID_RITEM,
		os_int flags = EOBJ_DEFAULT);

	/* Virtual destructor.
 	 */
    virtual ~eNetClient();

    /* Clone object.
     */
    virtual eObject *clone(
        eObject *parent, 
        e_oid id = EOID_CHILD,
		os_int aflags = 0);

    /* Casting eObject pointer to eNetClient pointer.
     */
    inline static eNetClient *cast(
		eObject *o) 
	{ 
        e_assert_type(o, ECLASSID_NETCLIENT)
        return (eNetClient*)o;
	}

    /* Get class identifier.
     */
    virtual os_int classid() 
    {
        return ECLASSID_NETCLIENT;
    }

    /* Static function to add class to propertysets and class list.
     */
    static void setupclass();

    /* Static constructor function for generating instance by class list.
     */
    static eNetClient *newobj(
        eObject *parent,
        e_oid id = EOID_ITEM,
		os_int flags = EOBJ_DEFAULT)
    {
        return new eNetClient(parent, id, flags);
    }
};

#endif
