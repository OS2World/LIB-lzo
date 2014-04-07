/* lzo_dict.h -- dictionary definitions for the the LZO library

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


/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the library and is subject
   to change.
 */


#ifndef __LZO_DICT_H
#define __LZO_DICT_H

#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************************
// dictionary
************************************************************************/

/* dictionary needed for compression */
#if !defined(D_BITS) && defined(DBITS)
#  define D_BITS		DBITS
#endif
#if !defined(D_BITS)
#  error D_BITS is not defined
#endif
#if (D_BITS < 16)
#  define D_SIZE		LZO_SIZE(D_BITS)
#  define D_MASK		LZO_MASK(D_BITS)
#else
#  define D_SIZE		LZO_USIZE(D_BITS)
#  define D_MASK		LZO_UMASK(D_BITS)
#endif

/* dictionary depth */
#if !defined(DD_BITS)
#  define DD_BITS		0
#endif
#define DD_SIZE			LZO_SIZE(DD_BITS)
#define DD_MASK			LZO_MASK(DD_BITS)

/* dictionary length */
#if !defined(DL_BITS)
#  define DL_BITS		(D_BITS - DD_BITS)
#endif
#if (DL_BITS < 16)
#  define DL_SIZE		LZO_SIZE(DL_BITS)
#  define DL_MASK		LZO_MASK(DL_BITS)
#else
#  define DL_SIZE		LZO_USIZE(DL_BITS)
#  define DL_MASK		LZO_UMASK(DL_BITS)
#endif


#if (D_BITS != DL_BITS + DD_BITS)
#  error D_BITS does not match
#endif
#if (D_BITS < 8 || D_BITS > 20)
#  error invalid D_BITS
#endif
#if (DL_BITS < 8 || DL_BITS > 20)
#  error invalid DL_BITS
#endif
#if (DD_BITS < 0 || DD_BITS > 6)
#  error invalid DD_BITS
#endif


#if !defined(DL_MIN_LEN)
#  define DL_MIN_LEN	3
#endif
#if !defined(DL_SHIFT)
#  define DL_SHIFT		((DL_BITS + (DL_MIN_LEN - 1)) / DL_MIN_LEN)
#endif



/***********************************************************************
// hashing
************************************************************************/

/* Incremental hashing is used to find a match of 3 (DL_MIN_LEN) bytes
 * in the dictionary.
 */

#if defined(LZO_HASH_LZO_INCREMENTAL_A) || defined(LZO_HASH_LZO_INCREMENTAL_B)
#  define __LZO_HASH_INCREMENTAL
#endif
#if defined(LZO_HASH_GZIP_INCREMENTAL)
#  define __LZO_HASH_INCREMENTAL
#endif


#if (DL_MIN_LEN == 3)
#  define _DV2_A(p,shift1,shift2) \
		(((( (lzo_uint32)(p[0]) << shift1) ^ p[1]) << shift2) ^ p[2])
#  define _DV2_B(p,shift1,shift2) \
		(((( (lzo_uint32)(p[2]) << shift1) ^ p[1]) << shift2) ^ p[0])
#elif (DL_MIN_LEN == 2)
#  define _DV2_A(p,shift1,shift2) \
		(( (lzo_uint32)(p[0]) << shift1) ^ p[1])
#  define _DV2_B(p,shift1,shift2) \
		(( (lzo_uint32)(p[1]) << shift1) ^ p[2])
#else
#  error invalid DL_MIN_LEN
#endif

#define _DV_A(p,shift) 	_DV2_A(p,shift,shift)
#define _DV_B(p,shift) 	_DV2_B(p,shift,shift)



#if defined(LZO_HASH_LZO)
   /* LZO hash function */
#  define _DINDEX(dv,p)		((40799u * _DV_A((p),5)) >> 5)

#elif defined(LZO_HASH_LZO_INCREMENTAL_A)
   /* incremental LZO hash */
#  define DVAL_FIRST(dv,p)	dv = _DV_A((p),5)
#  define DVAL_NEXT(dv,p) \
				dv ^= (lzo_uint32)(p[-1]) << (2*5); dv = (((dv) << 5) ^ p[2])
#  define _DINDEX(dv,p)		((40799u * (dv)) >> 5)
#  define DVAL_LOOKAHEAD	DL_MIN_LEN

#elif defined(LZO_HASH_LZO_INCREMENTAL_B)
   /* incremental LZO hash */
#  define DVAL_FIRST(dv,p)	dv = _DV_B((p),5)
#  define DVAL_NEXT(dv,p) \
				dv ^= p[-1]; dv = (((dv) >> 5) ^ ((lzo_uint32)(p[2]) << (2*5)))
#  define _DINDEX(dv,p)		((40799u * (dv)) >> 5)
#  define DVAL_LOOKAHEAD	DL_MIN_LEN

#elif defined(LZO_HASH_GZIP)
   /* hash function like in gzip (deflate) */
#  define _DINDEX(dv,p)		(_DV_A((p),DL_SHIFT))

#elif defined(LZO_HASH_GZIP_INCREMENTAL)
   /* incremental hash like in gzip (deflate) */
#  define DVAL_FIRST(dv,p)	dv = _DV_A((p),DL_SHIFT)
#  define DVAL_NEXT(dv,p)	dv = (((dv) << DL_SHIFT) ^ p[2])
#  define _DINDEX(dv,p)		(dv)
#  define DVAL_LOOKAHEAD	DL_MIN_LEN

#else
#  error choose a hashing strategy
#endif


#ifndef DINDEX
#define DINDEX(dv,p)		(((_DINDEX(dv,p)) & DL_MASK) << DD_BITS)
#endif


#if !defined(__LZO_HASH_INCREMENTAL)
#  define DVAL_FIRST(dv,p)	((void) 0)
#  define DVAL_NEXT(dv,p)	((void) 0)
#  define DVAL_LOOKAHEAD	0
#endif


#if !defined(DVAL_ASSERT)
#if defined(__LZO_HASH_INCREMENTAL) && !defined(NDEBUG)
static void DVAL_ASSERT(lzo_uint32 dv, const lzo_byte *p)
{
	lzo_uint32 df;
	DVAL_FIRST(df,(p));
	assert(DINDEX(dv,p) == DINDEX(df,p));
}
#else
#  define DVAL_ASSERT(dv,p)	((void) 0)
#endif
#endif



/***********************************************************************
// dictionary updating
************************************************************************/

#if (DD_BITS == 0)

#  define UPDATE_D(dict,cycle,dv,p)		dict[ DINDEX(dv,p) ] = (p)
#  define UPDATE_P(ptr,cycle,p)			(ptr)[0] = (p)
#  define UPDATE_I(dict,cycle,index,p)	dict[index] = (p)

#else

#  define UPDATE_D(dict,cycle,dv,p)	\
		dict[ DINDEX(dv,p) + cycle++ ] = (p); cycle &= DD_MASK
#  define UPDATE_P(ptr,cycle,p)	\
		(ptr) [ cycle++ ] = (p); cycle &= DD_MASK
#  define UPDATE_I(dict,cycle,index,p)	\
		dict[ (index) + cycle++ ] = (p); cycle &= DD_MASK

#endif



/***********************************************************************
//
************************************************************************/

#if defined(LZO_DETERMINISTIC)
#  define LZO_CHECK_MPOS	LZO_CHECK_MPOS_DET
#else
#  define LZO_CHECK_MPOS	LZO_CHECK_MPOS_NON_DET
#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */

/*
vi:ts=4
*/

