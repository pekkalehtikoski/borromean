/**

@file    string/osal_intser.c
@brief   Serialize integers.
@author  Pekka Lehtikoski
@version 1.0
@date    9.11.2011

Copyright 2012 Pekka Lehtikoski. This file is part of the eobjects project and shall only be used,
modified, and distributed under the terms of the project licensing. By continuing to use, modify,
or distribute this file you indicate that you have read the license and understand and accept
it fully.

****************************************************************************************************
*/
#include "eosal/osalx.h"

/* 
First byte NNNSxxxx:

- NNN number of follower bytes.
- S sign
- xxxx Least significant 4 data bits.
*/


os_int osal_intser_writer(
	os_uchar *buf,
	os_long x)
{
	os_memsz 
		n;

	os_uchar 
		*p;

	if (x < 0x10)
	{
		if (x >= 0)
		{
			*buf = (os_uchar)x;
			return 1;
		}

		x = -x;
		if (x < 0x10)
		{
			*buf = (os_uchar)x | 0x10;
			return 1;
		}

		*buf = (os_uchar)(x & 0xF) | 0x10;
	}
	else
	{
		*buf = (os_uchar)x & 0xF;
	}

	p = buf;
	x >>= 4;

	do
	{
		*(++p) = (os_uchar)x;
		x >>= 8;
	}
	while (x > 0);

	n = p - buf;
	*buf |= ((os_uchar)n << 5);

	return (os_int)n+1;
}


os_int osal_intser_reader(
	os_uchar *buf,
	os_long *x)
{
	os_int 
		shift;

	os_uchar
		c,
		count,
		*p;

	os_long
		y;

	c = *buf;

	if ((c & 0xF) == 0)
	{
		*x = c;
		return 1;
	}

	y = c & 0xF;
	count = c >> 5;
	shift = 4;
	p = buf;
	while (count--)
	{
		y |= ((os_long)*(++p)) << shift;
		shift += 8;
	}

	if (c & 0x10) *x = -y;
	else *x = y;

	return (os_int)(p - buf + 1);
}
