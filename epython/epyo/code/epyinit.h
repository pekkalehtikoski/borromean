/**

  @file    epyinit.cpp
  @brief   Initialization code for eobjects python extension epylib.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#ifndef EPYINIT_INCLUDED
#define EPYINIT_INCLUDED


PyMODINIT_FUNC
PyInit_epyo(void);

static PyObject *
spam_system(PyObject *self, PyObject *args);


#endif
