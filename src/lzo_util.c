/* lzo_util.c -- utilities for the the LZO library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   markus.oberhumer@jk.uni-linz.ac.at
 */


#include <stdio.h>

#include "lzo_conf.h"
#include "lzo_util.h"



/***********************************************************************
// slow but portable <string.h> stuff, only used in assertions
************************************************************************/

LZO_PUBLIC(int)
lzo_memcmp(const lzo_voidp s1, const lzo_voidp s2, lzo_uint len)
{
#if (LZO_UINT_MAX <= UINT_MAX) && !defined(MFX_MEMCMP_BROKEN)
	return memcmp(s1,s2,len);
#else
	const lzo_byte *p1 = (const lzo_byte *) s1;
	const lzo_byte *p2 = (const lzo_byte *) s2;
	int d;

	if (len > 0) do
	{
		d = *p1 - *p2;
		if (d != 0)
			return d;
		p1++;
		p2++;
	}
	while (--len > 0);
	return 0;
#endif
}


LZO_PUBLIC(lzo_voidp)
lzo_memcpy(lzo_voidp dest, const lzo_voidp src, lzo_uint len)
{
#if (LZO_UINT_MAX <= UINT_MAX)
	return memcpy(dest,src,len);
#else
	lzo_byte *p1 = (lzo_byte *) dest;
	const lzo_byte *p2 = (const lzo_byte *) src;

	if (len <= 0 || p1 == p2)
		return dest;
	do
		*p1++ = *p2++;
	while (--len > 0);
	return dest;
#endif
}


LZO_PUBLIC(lzo_voidp)
lzo_memmove(lzo_voidp dest, const lzo_voidp src, lzo_uint len)
{
#if (LZO_UINT_MAX <= UINT_MAX) && !defined(MFX_MEMMOVE_BROKEN)
	return memmove(dest,src,len);
#else
	lzo_byte *p1 = (lzo_byte *) dest;
	const lzo_byte *p2 = (const lzo_byte *) src;

	if (len <= 0 || p1 == p2)
		return dest;

	if (p1 < p2)
	{
		do
			*p1++ = *p2++;
		while (--len > 0);
	}
	else
	{
		p1 += len;
		p2 += len;
		do
			*--p1 = *--p2;
		while (--len > 0);
	}
	return dest;
#endif
}


LZO_PUBLIC(lzo_voidp)
lzo_memset(lzo_voidp s, int c, lzo_uint len)
{
#if (LZO_UINT_MAX <= UINT_MAX)
	return memset(s,c,len);
#else
	lzo_byte *p = (lzo_byte *) s;

	if (len > 0) do
		*p++ = LZO_BYTE(c);
	while (--len > 0);
	return s;
#endif
}


/***********************************************************************
// adler32 checksum
// adapted from free code by Mark Adler <madler@alumni.caltech.edu>
// see http://quest.jpl.nasa.gov/zlib
************************************************************************/

#define LZO_BASE 65521u /* largest prime smaller than 65536 */
#define LZO_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define LZO_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define LZO_DO2(buf,i)  LZO_DO1(buf,i); LZO_DO1(buf,i+1);
#define LZO_DO4(buf,i)  LZO_DO2(buf,i); LZO_DO2(buf,i+2);
#define LZO_DO8(buf,i)  LZO_DO4(buf,i); LZO_DO4(buf,i+4);
#define LZO_DO16(buf)   LZO_DO8(buf,0); LZO_DO8(buf,8);

LZO_PUBLIC(lzo_uint32)
lzo_adler32(lzo_uint32 adler, const lzo_byte *buf, lzo_uint len)
{
	lzo_uint32 s1 = adler & 0xffff;
	lzo_uint32 s2 = (adler >> 16) & 0xffff;
	int k;

	if (buf == NULL)
		return 1;

	while (len > 0)
	{
		k = len < LZO_NMAX ? (int) len : LZO_NMAX;
		len -= k;
		if (k >= 16) do
		{
			LZO_DO16(buf);
			buf += 16;
			k -= 16;
		} while (k >= 16);
		if (k != 0) do
		{
			s1 += *buf++;
			s2 += s1;
        } while (--k);
		s1 %= LZO_BASE;
		s2 %= LZO_BASE;
	}
	return (s2 << 16) | s1;
}

#undef LZO_BASE
#undef LZO_NMAX
#undef LZO_DO1
#undef LZO_DO2
#undef LZO_DO4
#undef LZO_DO8
#undef LZO_DO16


/***********************************************************************
// Runtime check of the assumptions about the size of builtin types,
// memory model, byte order and other low-level constructs.
// We are really paranoid here.
// Because of inlining much of this function evaluates to nothing.
************************************************************************/

#ifndef __cplusplus			/* work around a bug (?) in g++ 2.6.3 */
__inline__
#endif
LZO_PUBLIC(lzo_bool)
lzo_assert(int expr)
{
	return (expr) ? 1 : 0;
}

static lzo_bool strength_reduce_works_ok(int *);	/* avoid inlining */

LZO_PUBLIC(int)
_lzo_config_check(void)
{
	lzo_bool r = 1;
	int i;
	lzo_uint32 adler;
	char _wrkmem[8 * sizeof(lzo_byte *) + 16];
	const lzo_bytepp const dict = (const lzo_bytepp) LZO_ALIGN(_wrkmem,16);
	lzo_byte *wrkmem = (lzo_byte *) dict;

	r &= lzo_assert(sizeof(lzo_uint32) >= 4);
	r &= lzo_assert(sizeof(lzo_uint32) >= sizeof(lzo_uint));
	r &= lzo_assert(sizeof(lzo_uint) >= sizeof(unsigned));

	r &= lzo_assert(sizeof(lzo_ptrdiff_t) >= 4);
	r &= lzo_assert(sizeof(lzo_ptrdiff_t) >= sizeof(ptrdiff_t));

	r &= lzo_assert(sizeof(lzo_voidp) >= sizeof(lzo_uint));
	r &= lzo_assert(sizeof(lzo_voidp) == sizeof(lzo_byte *));
	r &= lzo_assert(sizeof(lzo_voidp) == sizeof(lzo_voidpp));
	r &= lzo_assert(sizeof(lzo_voidp) == sizeof(lzo_bytepp));
	r &= lzo_assert(sizeof(lzo_voidp) == sizeof(dict[0]));

	r &= lzo_assert(sizeof(lzo_void_ptr_t) == sizeof(lzo_voidp));

	r &= lzo_assert(sizeof(lzo_ptr_t) >= 4);
	r &= lzo_assert(sizeof(lzo_ptr_t) >= sizeof(lzo_voidp));
	r &= lzo_assert(sizeof(lzo_ptr_t) >= sizeof(ptrdiff_t));
	r &= lzo_assert(sizeof(lzo_ptr_t) >= sizeof(lzo_ptrdiff_t));

#if defined(SIZEOF_CHAR_P)
	r &= lzo_assert(SIZEOF_CHAR_P == sizeof(char *));
	r &= lzo_assert(sizeof(lzo_voidp) == sizeof(char *));
#endif
#if defined(SIZEOF_UNSIGNED)
	r &= lzo_assert(SIZEOF_UNSIGNED == sizeof(unsigned));
#endif
#if defined(SIZEOF_UNSIGNED_LONG)
	r &= lzo_assert(SIZEOF_UNSIGNED_LONG == sizeof(unsigned long));
#endif
#if defined(SIZEOF_UNSIGNED_SHORT)
	r &= lzo_assert(SIZEOF_UNSIGNED_SHORT == sizeof(unsigned short));
#endif
#if defined(SIZEOF_PTRDIFF_T)
#if (SIZEOF_PTRDIFF_T > 0)
	r &= lzo_assert(SIZEOF_PTRDIFF_T == sizeof(ptrdiff_t));
#endif
#endif
#if defined(SIZEOF_SIZE_T)
#if (SIZEOF_SIZE_T > 0)
	r &= lzo_assert(SIZEOF_SIZE_T == sizeof(size_t));
#endif
#endif

	/* assert the signedness of the integral types */
#define LZO_HIGHBIT(type)	((type) (1ul << (CHAR_BIT * sizeof(type) - 1)))
	r &= lzo_assert(LZO_HIGHBIT(lzo_uint32)      > 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_int32)       < 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_uint)        > 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_int)         < 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_ptrdiff_t)   < 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_ptr_t)       > 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_sptr_t)      < 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_void_ptr_t)  > 0);
	r &= lzo_assert(LZO_HIGHBIT(lzo_void_sptr_t) < 0);
#undef LZO_HIGHBIT

	/* sanity check of the memory model */
	if (r == 1)
	{
		for (i = 0; i < 8; i++)
			r &= lzo_assert((lzo_voidp) (&dict[i]) ==
			                (lzo_voidp) (&wrkmem[i * sizeof(lzo_byte *)]));
	}

	/* check LZO_BYTE_ORDER */
#if defined(LZO_BYTE_ORDER)
	if (r == 1)
	{
		union {
			unsigned char x[16];
			lzo_uint32 a;
			unsigned short b;
		} u;

		for (i = 0; i < 8; i++)
			u.x[i] = LZO_BYTE(i);

#  if (LZO_BYTE_ORDER == LZO_LITTLE_ENDIAN)
		r &= lzo_assert((u.a & 0xffffffffL) == 0x03020100L);
		r &= lzo_assert((u.b & 0xffff) == 0x0100);
#  elif (LZO_BYTE_ORDER == LZO_BIG_ENDIAN)
		r &= lzo_assert((u.a & 0xffffffffL) == 0x00010203L);
		r &= lzo_assert((u.b & 0xffff) == 0x0001);
#  else
#    error invalid LZO_BYTE_ORDER
#  endif
	}
#endif

	/* check that unaligned memory access works as expected */
#if defined(LZO_UNALIGNED_OK_2)
	r &= lzo_assert(sizeof(short) == 2);
	if (r == 1)
	{
		unsigned char x[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		unsigned short b[4];

		for (i = 0; i < 4; i++)
			b[i] = * (unsigned short *) &x[i];

#  if (LZO_BYTE_ORDER == LZO_LITTLE_ENDIAN)
		r &= lzo_assert(b[0] == 0x0100);
		r &= lzo_assert(b[1] == 0x0201);
		r &= lzo_assert(b[2] == 0x0302);
		r &= lzo_assert(b[3] == 0x0403);
#  elif (LZO_BYTE_ORDER == LZO_BIG_ENDIAN)
		r &= lzo_assert(b[0] == 0x0001);
		r &= lzo_assert(b[1] == 0x0102);
		r &= lzo_assert(b[2] == 0x0203);
		r &= lzo_assert(b[3] == 0x0304);
#  endif
	}
#endif

#if defined(LZO_UNALIGNED_OK_4)
	r &= lzo_assert(sizeof(lzo_uint32) == 4);
	if (r == 1)
	{
		unsigned char x[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		lzo_uint32 a[4];

		for (i = 0; i < 4; i++)
			a[i] = * (lzo_uint32 *) &x[i];

#  if (LZO_BYTE_ORDER == LZO_LITTLE_ENDIAN)
		r &= lzo_assert(a[0] == 0x03020100L);
		r &= lzo_assert(a[1] == 0x04030201L);
		r &= lzo_assert(a[2] == 0x05040302L);
		r &= lzo_assert(a[3] == 0x06050403L);
#  elif (LZO_BYTE_ORDER == LZO_BIG_ENDIAN)
		r &= lzo_assert(a[0] == 0x00010203L);
		r &= lzo_assert(a[1] == 0x01020304L);
		r &= lzo_assert(a[2] == 0x02030405L);
		r &= lzo_assert(a[3] == 0x03040506L);
#  endif
	}
#endif

	/* check BZERO8_PTR and that NULL == 0 */
	r &= lzo_assert(NULL == 0);
	if (r == 1)
	{
		BZERO8_PTR(dict,8);
		for (i = 0; i < 8; i++)
			r &= lzo_assert(dict[i] == NULL);
	}

	/* check the lzo_adler32() function */
	if (r == 1)
	{
		adler = lzo_adler32(0, NULL, 0);
		adler = lzo_adler32(adler, __lzo_copyright, 144);
		r &= lzo_assert(adler == 0x845c30a2L);
	}

	/* check for the gcc strength-reduction optimization bug */
	if (r == 1)
	{
		static int x[3];
		static unsigned xn = 3;
		register unsigned j;

		for (j = 0; j < xn; j++)
			x[j] = (int)j - 3;
		r &= lzo_assert(strength_reduce_works_ok(x));
	}

	return r == 1 ? LZO_E_OK : LZO_E_ERROR;
}


static lzo_bool strength_reduce_works_ok(int *x)
{
	return x[0] == -3 && x[1] == -2 && x[2] == -1;
}


/***********************************************************************
//
************************************************************************/

/* If you use the LZO library in a product, you *must* keep this
 * copyright string in the executable of your product.
 */

const lzo_byte __lzo_copyright[] =
	"\n\n\n"
	"LZO real-time data compression library.\n"
	"Copyright (C) 1996, 1997 Markus Franz Xaver Johannes Oberhumer\n"
	"<markus.oberhumer@jk.uni-linz.ac.at>\n"
	"\n"
	"LZO version: v" LZO_VERSION_STRING ", " LZO_VERSION_DATE "\n"
	"LZO build date: " __DATE__ " " __TIME__ "\n"
	"LZO special compilation options:\n"
#ifdef __cplusplus
	" __cplusplus\n"
#endif
#if (UINT_MAX < 0xffffffffL)
	" 16BIT\n"
#elif (UINT_MAX > 0xffffffffL)
	" 64BIT\n"
#endif
#if (LZO_UINT_MAX < 0xffffffffL)
	" LZO_16BIT\n"
#endif
#if defined(LZO_BYTE_ORDER)
	" LZO_BYTE_ORDER=" _LZO_MEXPAND(LZO_BYTE_ORDER) "\n"
#endif
#if defined(LZO_UNALIGNED_OK_2)
	" LZO_UNALIGNED_OK_2\n"
#endif
#if defined(LZO_UNALIGNED_OK_4)
	" LZO_UNALIGNED_OK_4\n"
#endif
	"\n\n";


LZO_PUBLIC(unsigned)
lzo_version(void)
{
	return LZO_VERSION;
}

LZO_PUBLIC(const char *)
lzo_version_string(void)
{
	return LZO_VERSION_STRING;
}

LZO_PUBLIC(const char *)
lzo_version_date(void)
{
	return LZO_VERSION_DATE;
}


/***********************************************************************
//
************************************************************************/

LZO_PUBLIC(int)
lzo_init(void)
{
	int r;

	r = _lzo_config_check();
	if (r != LZO_E_OK)
		return r;

	return r;
}


/*
vi:ts=4
*/
