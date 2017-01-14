/**

  @file    serialize/windows/osal_floatr.c
  @brief   Convert floating point number to two integers and vice versa.
  @author  Pekka Lehtikoski
  @version 1.0
  @date    9.11.2011

  Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
#include "eosal/eosalx.h"

#if OSAL_SMALL_ENDIAN
    #define ODFLOAT_LOW 0
    #define ODFLOAT_HIGH 1
#else
    #define ODFLOAT_LOW 1
    #define ODFLOAT_HIGH 0
 #endif


os_boolean osal_ints2double(
    os_double *x,
    os_long m,
    os_long e)
{
    os_long sign;
    os_boolean rval;

    /* If mantissa is zero, the floating point value is zero.
     */
    if (m == 0)
    {
        *x = 0.0;
        return OS_TRUE;
    }

    /* If mantissa is negative, set sign flag and turn mantissa to positive.
     */
    if (m < 0)
    {
        sign = 0x8000000000000000;
        m = -m;
    }
    else
    {
        sign = 0;
    }

    /* Assume success as return value.
     */
    rval = OS_TRUE;

    /* If underflow, use smallest possible value.
     */
    if (e < -1023)
    {
        e = -1023;
        m = 0x10000000000000;
    }

    /* If overflow, use biggest possible value and set return
       value to failed.
     */
    if (e > 1023)
    {
        e = 1023;
        m = 0x1FFFFFFFFFFFFF;
        rval = OS_FALSE;
    }

    /* Move leading 1 to right position.
     */
    while (m >= 0x20000000000000) m >>= 1;
    while (m < 0x10000000000000) m <<= 1;

    /* Merge exponent and sign into mantissa.
     */
    e += 1023;
    m |= ((os_long)e << 52) | sign;

    /* Return the result.
     */
    *x = *(os_double*)&m;
    return rval;
}

void osal_double2ints(
	os_double x,
	os_long *m,
    os_long *e)
{
    os_long sign, v;

    v = *(os_long*)&x;

    /* If this is zero?
     */
    if (v == 0x10000000000000)
    {
        *m = *e = 0;
        return;
    }

    /* Get sign.
     */
    sign = v & 0x8000000000000000;
    v &= 0x7FFF000000000000;

    /* Return exponent and mantissa
     */
    *e = (v >> 52) - 1023;

    /* Shift right, until rightmost bit is 1. We want the integer mantissa 
       to be as small as possible.
     */
    v &= 0x000FFFFFFFFFFFFF;
    while (v & 1) v >>= 1;

    /* If we need to put sign back?
     */
    if (sign) v = -v;
    *m = v;
}


os_boolean osal_ints2float(
    os_float *x,
    os_long m,
    os_long e)
{
    os_int sign, mm;
    os_boolean rval;

    /* If mantissa is zero, the floating point value is zero.
     */
    if (m == 0)
    {
        *x = 0.0;
        return OS_TRUE;
    }

    /* If mantissa is negative, set sign flag and turn mantissa to positive.
     */
    if (m < 0)
    {
        sign = 0x80000000;
        m = -m;
    }
    else
    {
        sign = 0;
    }

    /* Assume success as return value.
     */
    rval = OS_TRUE;

    /* If underflow, use smallest possible value.
     */
    if (e < -127)
    {
        e = -127;
        m = 0x800000;
    }

    /* If overflow, use biggest posible value and set return.
       value to failed.
     */
    if (e > 127)
    {
        e = 127;
        m = 0xFFFFFF;
        rval = OS_FALSE;
    }

    /* Move leading 1 to right position.
     */
    while (m >= 0x1000000) m >>= 1;
    while (m < 0x800000) m <<= 1;

    /* Merge exponent and sign into mantissa.
     */
    e += 127;
    mm = (os_int)m | ((os_int)e << 23) | sign;

    /* Return the result.
     */
    *x = *(os_float*)&mm;
    return rval;
}


void osal_float2ints(
    os_float x,
    os_long *m,
    os_long *e)
{
    os_int sign, v;

    v = *(os_int*)&x;

    /* If this is zero?
     */
    if (v == 0x800000)
    {
        *m = *e = 0;
        return;
    }

    /* Get sign.
     */
    sign = v &0x80000000;
    v &= 0x7FFF0000;

    /* Return exponent and mantissa
     */
    *e = (v >> 23) - 127;

    /* Shift right, until rightmost bit is 1. We want the integer mantissa 
       to be as small as possible.
     */
    v &= 0x007FFFFF;
    while (v & 1) v >>= 1;

    /* If we need to put sign back?
     */
    if (sign) v = -v;
    *m = v;
}

