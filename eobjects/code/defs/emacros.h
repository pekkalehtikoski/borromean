/**

  @file    emacros.h
  @brief   Common general purpose macros.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used, 
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept 
  it fully.

****************************************************************************************************
*/
#ifndef EMACROS_INCLUDED
#define EMACROS_INCLUDED

inline os_long eround_double_to_long(
    os_double x)
{
    return (os_long)(x >= 0.0 ? x + 0.5 : x - 0.5);
}

inline os_int eround_double_to_int(
    os_double x)
{
    return (os_int)(x >= 0.0 ? x + 0.5 : x - 0.5);
}

inline os_short eround_double_to_short(
    os_double x)
{
    return (os_short)(x >= 0.0 ? x + 0.5 : x - 0.5);
}

inline os_char eround_double_to_char(
    os_double x)
{
    return (os_char)(x >= 0.0 ? x + 0.5 : x - 0.5);
}

inline os_long eround_float_to_long(
    os_float x)
{
    return (os_long)(x >= 0.0F ? x + 0.5F : x - 0.5F);
}

inline os_int eround_float_to_int(
    os_float x)
{
    return (os_int)(x >= 0.0F ? x + 0.5F : x - 0.5F);
}

inline os_short eround_float_to_short(
    os_float x)
{
    return (os_short)(x >= 0.0F ? x + 0.5F : x - 0.5F);
}

inline os_char eround_float_to_char(
    os_float x)
{
    return (os_char)(x >= 0.0F ? x + 0.5F : x - 0.5F);
}

#endif
