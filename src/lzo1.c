/* lzo1.c -- implementation of the LZO1 algorithm

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
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


/* set tabsize to 4 */


#undef NDEBUG
#if 1 && !defined(NDEBUG) && !defined(LZO_DEBUG)
#define NDEBUG
#endif
#include <assert.h>

#include <lzo1.h>
#include "lzo_conf.h"


/***********************************************************************
// The next two defines can be changed to customize LZO1.
// The default version is LZO1-5/1.
************************************************************************/

/* run bits (3 - 5) - the compressor and the decompressor
 * must use the same value. */
#if !defined(RBITS)
#  define RBITS		5
#endif

/* compression level (1 - 9) - this only affects the compressor.
 * 1 is fastest, 9 is best compression ratio */
#if !defined(CLEVEL)
#  define CLEVEL	1			/* fastest by default */
#endif


/* check configuration */
#if (RBITS < 3 || RBITS > 5)
#  error invalid RBITS
#endif
#if (CLEVEL < 1 || CLEVEL > 9)
#  error invalid CLEVEL
#endif


/***********************************************************************
// You should not have to change anything below this line.
************************************************************************/

#undef LZO_ALWAYS_USE_MEMCPY
#undef LZO_ALWAYS_USE_MEMMOVE
#include "lzo_util.h"



/***********************************************************************
//
************************************************************************/

/*
     Format of the marker byte


	 76543210
	 --------
	 00000000	a long run (a 'R0' run) - there are short and long R0 runs
	 000rrrrr	a short run with len r
	 mmmooooo	a short match (len = 2+m, o = offset low bits)
	 111ooooo	a long match (o = offset low bits)
*/


#define RSIZE	(1 << RBITS)
#define RMASK	(RSIZE - 1)

#define OBITS	RBITS				/* offset and run-length use same bits */
#define OSIZE	(1 << OBITS)
#define OMASK	(OSIZE - 1)

#define MBITS	(8 - OBITS)
#define MSIZE	(1 << MBITS)
#define MMASK	(MSIZE - 1)


/* sanity checks */
#if (OBITS < 3 || OBITS > 5)
#  error invalid OBITS
#endif
#if (MBITS < 3 || MBITS > 5)
#  error invalid MBITS
#endif


/***********************************************************************
// some macros to improve readability
************************************************************************/

/* Minimum len of a match */
#define	MIN_MATCH			3
#define	THRESHOLD			(MIN_MATCH - 1)

/* Minimum len of match coded in 2 bytes */
#define	MIN_MATCH_SHORT		MIN_MATCH

/* Maximum len of match coded in 2 bytes */
#define	MAX_MATCH_SHORT		(THRESHOLD + (MSIZE - 2))
/* MSIZE - 2: 0 is used to indicate runs,
 *            MSIZE-1 is used to indicate a long match */

/* Minimum len of match coded in 3 bytes */
#define	MIN_MATCH_LONG		(MAX_MATCH_SHORT + 1)

/* Maximum len of match coded in 3 bytes */
#define	MAX_MATCH_LONG		(MIN_MATCH_LONG + 255)

/* Maximum offset of a match */
#define	MAX_OFFSET			(1 << (8 + OBITS))


/*

RBITS | MBITS  MIN  THR.  MSIZE  MAXS  MINL  MAXL   MAXO  R0MAX R0FAST
======+===============================================================
  3   |   5      3    2     32    32    33    288   2048    263   256
  4   |   4      3    2     16    16    17    272   4096    271   264
  5   |   3      3    2      8     8     9    264   8192    287   280

 */


/***********************************************************************
// internal configuration
// all of these affect compression only
************************************************************************/

/* return -1 instead of copying if the data cannot be compressed */
#undef LZO_RETURN_IF_NOT_COMPRESSIBLE


/* choose the hashing strategy */
#define LZO_HASH_LZO_INCREMENTAL_A

#define DBITS		(8 + RBITS)
#include "lzo_dict.h"
#define DVAL_LEN	DVAL_LOOKAHEAD


/***********************************************************************
// get algorithm info, return memory required for compression
************************************************************************/

LZO_EXTERN(lzo_uint) lzo1_info ( int *rbits, int *clevel );

LZO_PUBLIC(lzo_uint)
lzo1_info ( int *rbits, int *clevel )
{
	if (rbits)
		*rbits = RBITS;
	if (clevel)
		*clevel = CLEVEL;
	return D_SIZE * lzo_sizeof(lzo_byte *);
}


/***********************************************************************
// decode a R0 literal run (a long run)
************************************************************************/

#define	R0MIN	(RSIZE)				/* Minimum len of R0 run of literals */
#define	R0MAX	(R0MIN + 255)		/* Maximum len of R0 run of literals */
#define	R0FAST	(R0MAX & ~7u)		/* R0MAX aligned to 8 byte boundary */

#if (R0MAX - R0FAST != 7) || ((R0FAST & 7) != 0)
#  error something went wrong
#endif

/* 7 special codes from R0FAST+1 .. R0MAX
 * these codes mean long R0 runs with lengths
 * 512, 1024, 2048, 4096, 8192, 16384, 32768 */

static lzo_uint
unstore_long_r0(lzo_byte *op, const lzo_byte *ip, const unsigned t)
{
	/* this function is only for long R0 runs */
	assert(t <= (R0MAX - R0FAST));

	if (t == 0)
	{
		MEMCPY8_DS(op, ip, R0FAST);
		return R0FAST;
	}
	else
	{
#if 0
		lzo_uint tt = 256u << t;
		MEMCPY8_DS(op, ip, tt);
		return t;
#else
		lzo_uint tt = (256u/8) << t;
		MEMCPY8_DS(op, ip, tt * 8);
		return tt * 8;
#endif
	}
}


/***********************************************************************
// LZO1 decompress a block of data.
//
// Could be easily translated into assembly code.
//
// unstore_long_r0 has been moved to it's own function to improve
// performance on machines with few registers (like the Intel 80x86)
************************************************************************/

LZO_PUBLIC(int)
lzo1_decompress  ( const lzo_byte *in , lzo_uint  in_len,
                         lzo_byte *out, lzo_uint *out_len,
                         lzo_voidp LZO_UNUSED(wrkmem) )
{
	lzo_byte *op;
	const lzo_byte *ip;
	const lzo_byte * const ip_end = in + in_len;
	unsigned char t;

	op = out;
	ip = in;
	while (ip < ip_end)
	{
		t = *ip++;	/* get marker */

		if (t < R0MIN)			/* a literal run */
		{
			if (t == 0)				/* a R0 literal run */
			{
				t = *ip++;
				if (t < R0FAST - R0MIN)				/* a short R0 run */
				{
					lzo_uint tt = t + R0MIN;
					MEMCPY_DS(op,ip,tt);
				}
				else 								/* a long R0 run */
				{
					lzo_uint tt;
					t -= R0FAST - R0MIN;
					tt = unstore_long_r0(op,ip,(unsigned)t);
					op += tt;
					ip += tt;
				}
			}
			else
			{
				MEMCPY_DS(op,ip,t);
			}
		}
		else					/* a match */
		{
			lzo_uint tt;
			/* get match offset */
			const lzo_byte *m_pos = op - 1;
			m_pos -= (lzo_uint)(t & OMASK) | (((lzo_uint) *ip++) << OBITS);

			/* get match len */
			if (t >= ((MSIZE - 1) << OBITS))				/* all m-bits set */
				tt = (MIN_MATCH_LONG - THRESHOLD) + *ip++;	/* a long match */
			else
				tt = t >> OBITS;							/* a short match */

			assert(m_pos >= out);
			assert(m_pos <  op);
#if defined(LZO_ALWAYS_USE_MEMMOVE)
			MEMMOVE_DS(op,m_pos,tt + THRESHOLD);
#else
			/* a half unrolled loop */
			*op++ = *m_pos++;
			*op++ = *m_pos++;
			MEMMOVE_DS(op,m_pos,tt);
#endif
		}
	}

	*out_len = op - out;

	/* the next line is the only check in the decompressor ! */
	return (ip == ip_end ? 0 : -1);
}


/***********************************************************************
// code a literal run
************************************************************************/

static lzo_byte *
store_run(lzo_byte *op, const lzo_byte *ii, lzo_uint r_len)
{
	assert(r_len > 0);

	/* code a long R0 run */
	if (r_len >= 512)
	{
		unsigned r_bits = 7;		/* 256 << 7 == 32768 */
		do {
			while (r_len >= (256u << r_bits))
			{
				r_len -= (256u << r_bits);
				*op++ = 0; *op++ = LZO_BYTE((R0FAST - R0MIN) + r_bits);
				MEMCPY8_DS(op, ii, (256u << r_bits));
			}
		} while (--r_bits > 0);
	}
	while (r_len >= R0FAST)
	{
		r_len -= R0FAST;
		*op++ = 0; *op++ = R0FAST - R0MIN;
		MEMCPY8_DS(op, ii, R0FAST);
	}

	if (r_len >= R0MIN)
	{
		/* code a short R0 run */
		*op++ = 0; *op++ = LZO_BYTE(r_len - R0MIN);
		MEMCPY_DS(op, ii, r_len);
	}
	else if (r_len > 0)
	{
		/* code a 'normal' run */
		*op++ = LZO_BYTE(r_len);
		MEMCPY_DS(op, ii, r_len);
	}

	assert(r_len == 0);
	return op;
}



/***********************************************************************
// LZO1 compress a block of data.
//
// Could be translated into assembly code without too much effort.
//
// I apologize for the spaghetti code, but it really helps the optimizer.
************************************************************************/

static int
do_compress    ( const lzo_byte *in , lzo_uint  in_len,
                       lzo_byte *out, lzo_uint *out_len,
                       lzo_voidp wrkmem )
{
	const lzo_byte *ip;
#if defined(__LZO_HASH_INCREMENTAL)
	lzo_uint32 dv;
#endif
	lzo_byte *op;
	const lzo_byte *m_pos;
	const lzo_byte * const ip_end = in+in_len - DVAL_LEN - MIN_MATCH_LONG;
	const lzo_byte * const in_end = in+in_len - DVAL_LEN;
	const lzo_byte *ii;
	const lzo_bytepp const dict = (const lzo_bytepp) wrkmem;
	lzo_ptrdiff_t m_off;

#if !defined(NDEBUG)
	const lzo_byte *m_pos_sav;
#endif

	op = out;
	ip = in;
	ii = ip;				/* point to start of literal run */
	if (in_len <= MIN_MATCH_LONG + DVAL_LEN + 1)
		goto the_end;

	/* init dictionary */
#if defined(LZO_DETERMINISTIC)
	BZERO8_PTR(dict,D_SIZE);
#endif

	DVAL_FIRST(dv,ip);
	UPDATE_D(dict,cycle,dv,ip);
	ip++;
	DVAL_NEXT(dv,ip);

	do {

	dsearch:

		/* search ip in the dictionary */
		{
			lzo_uint dindex;

			dindex = DINDEX(dv,ip);
			m_pos = dict[dindex];
			UPDATE_I(dict,cycle,dindex,ip);
		}

#if !defined(NDEBUG)
		m_pos_sav = m_pos;
#endif

	/* check potential match in m_pos */
		if (LZO_CHECK_MPOS(m_pos,m_off,in,ip,MAX_OFFSET)
			|| *m_pos++ != ip[0] || *m_pos++ != ip[1] || *m_pos++ != ip[2])
		{
	/* a literal */
			/* OPTIMIZED: use a goto instead of '++ip' and end of do-loop */
			if (++ip < ip_end)
			{
				DVAL_NEXT(dv,ip);
				goto dsearch;
			}
		}
		else
		{
	/* else we have found a match (of at least length 3) */
			/* 1) store the current literal run */
			if (ip - ii > 0)
			{
#if 1
				/* OPTIMIZED: inline the copying of a short run */
				if (ip - ii < R0MIN)
				{
					lzo_uint t = ip - ii;
					*op++ = LZO_BYTE(t);
					MEMCPY_DS(op, ii, t);
				}
				else
#endif
					op = store_run(op,ii,(lzo_uint)(ip-ii));
			}

			/* 2a) compute match len */
			ii = ip;		/* point to start of current match */

			/* we already matched MIN_MATCH bytes,
			 * m_pos also already advanced MIN_MATCH bytes */
			ip += MIN_MATCH;
			assert(m_pos < ip);

			/* try to match another MIN_MATCH_LONG - MIN_MATCH bytes
			 * to see if we get a long match */

			/* the 'classic' PS macro :-) */
#define PS	*m_pos++ != *ip++

#if (MIN_MATCH_LONG - MIN_MATCH == 2)					/* MBITS == 2 */
			if (PS || PS)
#elif (MIN_MATCH_LONG - MIN_MATCH == 6)					/* MBITS == 3 */
			if (PS || PS || PS || PS || PS || PS)
#elif (MIN_MATCH_LONG - MIN_MATCH == 14)				/* MBITS == 4 */
			if (PS || PS || PS || PS || PS || PS || PS ||
			    PS || PS || PS || PS || PS || PS || PS)
#elif (MIN_MATCH_LONG - MIN_MATCH == 30)				/* MBITS == 5 */
			if (PS || PS || PS || PS || PS || PS || PS || PS ||
		    	PS || PS || PS || PS || PS || PS || PS || PS ||
		    	PS || PS || PS || PS || PS || PS || PS || PS ||
		    	PS || PS || PS || PS || PS || PS)
#else
#  error MBITS not yet implemented
#endif
			{
				lzo_uint m_len;

			/* 2b) code a short match */
					assert(ip-m_pos == m_off);
				--ip;	/* ran one too far, point back to non-match */
				m_len = ip - ii;
					assert(m_len >= MIN_MATCH_SHORT);
					assert(m_len <= MAX_MATCH_SHORT);
					assert(m_off > 0);
					assert(m_off <= MAX_OFFSET);
					assert(ii-m_off == m_pos_sav);
					assert(lzo_memcmp(m_pos_sav,ii,m_len) == 0);
				--m_off;
				/* code short match len + low offset bits */
				*op++ = LZO_BYTE(((m_len - THRESHOLD) << OBITS) |
				                 (m_off & OMASK));
				/* code high offset bits */
				*op++ = LZO_BYTE(m_off >> OBITS);


			/* 2c) Insert phrases (beginning with ii+1) into the dictionary. */

#define SI		/* nothing */
#define DI		++ii; DVAL_NEXT(dv,ii); UPDATE_D(dict,cycle,dv,ii);
#define XI		assert(ii < ip); ii = ip; DVAL_FIRST(dv,(ip));

#if (CLEVEL == 9) || (CLEVEL >= 7 && MBITS <= 4) || (CLEVEL >= 5 && MBITS <= 3)
			/* Insert the whole match (ii+1)..(ip-1) into dictionary.  */
				++ii;
				do {
					DVAL_NEXT(dv,ii);
					dict[ DINDEX(dv,ii) ] = ii;
				} while (++ii < ip);
				DVAL_NEXT(dv,ii);
				assert(ii == ip);
				DVAL_ASSERT(dv,ip);
#elif (CLEVEL >= 3)
				SI   DI DI   XI
#elif (CLEVEL >= 2)
				SI   DI      XI
#else
				             XI
#endif

			}
			else
			{
			/* we've found a long match - see how far we can still go */
				const lzo_byte *end;
				lzo_uint m_len;

				assert(ip <= in_end);
				assert(ii == ip - MIN_MATCH_LONG);

#if defined(__BOUNDS_CHECKING_ON)
				if (in_end - ip <= (MAX_MATCH_LONG - MIN_MATCH_LONG))
#else
				if (in_end <= ip + (MAX_MATCH_LONG - MIN_MATCH_LONG))
#endif
					end = in_end;
				else
				{
					end = ip + (MAX_MATCH_LONG - MIN_MATCH_LONG);
					assert(end < in_end);
				}

				while (ip < end  &&  *m_pos == *ip)
					m_pos++, ip++;
				assert(ip <= in_end);

			/* 2b) code the long match */
				m_len = ip - ii;
					assert(m_len >= MIN_MATCH_LONG);
					assert(m_len <= MAX_MATCH_LONG);
					assert(m_off > 0);
					assert(m_off <= MAX_OFFSET);
					assert(ii-m_off == m_pos_sav);
					assert(lzo_memcmp(m_pos_sav,ii,m_len) == 0);
					assert(ip-m_pos == m_off);
				--m_off;
				/* code long match flag + low offset bits */
				*op++ = LZO_BYTE(((MSIZE - 1) << OBITS) | (m_off & OMASK));
				/* code high offset bits */
				*op++ = LZO_BYTE(m_off >> OBITS);
				/* code match len */
				*op++ = LZO_BYTE(m_len - MIN_MATCH_LONG);


			/* 2c) Insert phrases (beginning with ii+1) into the dictionary. */
#if (CLEVEL == 9)
			/* Insert the whole match (ii+1)..(ip-1) into dictionary.  */
			/* This is not recommended because it is slow. */
				++ii;
				do {
					DVAL_NEXT(dv,ii);
					dict[ DINDEX(dv,ii) ] = ii;
				} while (++ii < ip);
				DVAL_NEXT(dv,ii);
				assert(ii == ip);
				DVAL_ASSERT(dv,ip);
#elif (CLEVEL >= 8)
				SI   DI DI DI DI DI DI DI DI   XI
#elif (CLEVEL >= 7)
				SI   DI DI DI DI DI DI DI      XI
#elif (CLEVEL >= 6)
				SI   DI DI DI DI DI DI         XI
#elif (CLEVEL >= 5)
				SI   DI DI DI DI               XI
#elif (CLEVEL >= 4)
				SI   DI DI DI                  XI
#elif (CLEVEL >= 3)
				SI   DI DI                     XI
#elif (CLEVEL >= 2)
				SI   DI                        XI
#else
				                               XI
#endif
			}

			/* ii now points to the start of next literal run */
			assert(ii == ip);
		}
	} while (ip < ip_end);



the_end:
	assert(ip <= in_end);


#if defined(LZO_RETURN_IF_NOT_COMPRESSIBLE)
	/* return -1 if op == out to indicate that we
	 * couldn't compress and didn't copy anything.
	 */
	if (op == out)
	{
		*out_len = 0;
		return LZO_E_NOT_COMPRESSIBLE;
	}
#endif


	/* store the final literal run */
	if (in_end + DVAL_LEN - ii > 0)
		op = store_run(op,ii,(lzo_uint)(in_end+DVAL_LEN-ii));

	*out_len = op - out;
	return 0;				/* compression went ok */
}


/***********************************************************************
// compress public entry point.
************************************************************************/

LZO_PUBLIC(int)
lzo1_compress ( const lzo_byte *in , lzo_uint  in_len,
                      lzo_byte *out, lzo_uint *out_len,
                      lzo_voidp wrkmem )
{
	int r;

	/* sanity check */
	if (!lzo_assert(LZO1_MEM_COMPRESS >= D_SIZE * lzo_sizeof(lzo_byte *)))
		return LZO_E_ERROR;

	/* don't try to compress a block that's too short */
	if (in_len <= MIN_MATCH_LONG + DVAL_LEN + 1)
	{
#if defined(LZO_RETURN_IF_NOT_COMPRESSIBLE)
		r = LZO_E_NOT_COMPRESSIBLE;
#else
		*out_len = store_run(out,in,in_len) - out;
		r =  0;				/* compression went ok */
#endif
	}
	else
		r = do_compress(in,in_len,out,out_len,wrkmem);

	if (r != 0)
		*out_len = 0;

	return r;
}


/*
vi:ts=4
*/
