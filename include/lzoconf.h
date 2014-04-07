/* lzoconf.h -- configuration for the LZO real-time data compression library

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


#ifndef __LZOCONF_H
#define __LZOCONF_H

#define LZO_VERSION             0x0280
#define LZO_VERSION_STRING      "0.28"
#define LZO_VERSION_DATE        "22 Feb 1997"


/* internal autoconf configuration file - only used when building LZO */
#if defined(LZO_HAVE_CONFIG_AC)
#  include <config.ac>
#endif

#include <limits.h>             /* CHAR_BIT, UINT_MAX, ULONG_MAX, USHRT_MAX */
#if !defined(CHAR_BIT) || (CHAR_BIT != 8)
#  error invalid CHAR_BIT
#endif

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
// architecture defines
************************************************************************/

#if defined(__MSDOS__) || defined(MSDOS)
#  define __LZO_MSDOS
#  if (UINT_MAX < 0xffffffffL)
#    define __LZO_MSDOS16
#  endif
#elif defined(__TOS__)
#  if (UINT_MAX < 0xffffffffL)
#    define __LZO_ATARI16
#  endif
#endif


/***********************************************************************
// integral and pointer types
************************************************************************/

/* Improve code readability */
typedef int lzo_bool;

/* Integral types with 32 bits or more */
#if (UINT_MAX >= 0xffffffffL)
   typedef unsigned int         lzo_uint32;
   typedef int                  lzo_int32;
#  define LZO_UINT32_MAX        UINT_MAX
#elif (ULONG_MAX >= 0xffffffffL)
   typedef unsigned long        lzo_uint32;
   typedef long                 lzo_int32;
#  define LZO_UINT32_MAX        ULONG_MAX
#else
#  error lzo_uint32
#endif

/* lzo_uint is used like size_t */
#if (UINT_MAX >= 0xffffffffL)
   typedef unsigned int         lzo_uint;
   typedef int                  lzo_int;
#  define LZO_UINT_MAX          UINT_MAX
#elif (ULONG_MAX >= 0xffffffffL)
   typedef unsigned long        lzo_uint;
   typedef long                 lzo_int;
#  define LZO_UINT_MAX          ULONG_MAX
#else
#  error lzo_uint
#endif


/* Memory model that allows to access memory at offsets of lzo_uint.
 * Huge pointers (16 bit MSDOS) are somewhat slow, but they work
 * fine and I really don't care about 16 bit compiler
 * optimizations nowadays.
 */
#if (LZO_UINT_MAX <= UINT_MAX)
#  define __LZO_MMODEL
#elif defined(__LZO_MSDOS16)
#  define __LZO_MMODEL          __huge
#  define __LZO_EXPORT          __far __cdecl
#  define LZO_999_UNSUPPORTED
#elif defined(__LZO_ATARI16)
#  define __LZO_MMODEL
#else
#  error __LZO_MMODEL
#endif

/* no typedef here because of const-pointer issues */
#define lzo_byte                unsigned char __LZO_MMODEL
#define lzo_bytep               unsigned char __LZO_MMODEL *
#define lzo_voidp               void __LZO_MMODEL *
#define lzo_shortp              short __LZO_MMODEL *
#define lzo_ushortp             unsigned short __LZO_MMODEL *
#define lzo_uint32p             lzo_uint32 __LZO_MMODEL *
#define lzo_int32p              lzo_int32 __LZO_MMODEL *
#define lzo_uintp               lzo_uint __LZO_MMODEL *
#define lzo_intp                lzo_int __LZO_MMODEL *
#define lzo_voidpp              lzo_voidp __LZO_MMODEL *
#define lzo_bytepp              lzo_bytep __LZO_MMODEL *


/* Unsigned and signed types that can store all bits of a lzo_voidp */
typedef unsigned long           lzo_ptr_t;
typedef long                    lzo_sptr_t;

/* Align a char pointer on a boundary that is a multiple of `size' */
#define LZO_ALIGN(ptr,size) \
    ((ptr) + (unsigned) ( \
        (((lzo_ptr_t)(ptr) + (size)-1) & ~((lzo_ptr_t)((size)-1))) - \
        (lzo_ptr_t)(ptr) ))


/***********************************************************************
// function types
************************************************************************/

#ifdef __cplusplus
#  define LZO_EXTERN_C          extern "C"
#else
#  define LZO_EXTERN_C          extern
#endif

#if !defined(__LZO_ENTRY)       /* calling convention */
#  define __LZO_ENTRY
#endif
#if !defined(__LZO_EXPORT)      /* DLL export information */
#  define __LZO_EXPORT
#endif

#if !defined(LZO_EXTERN)
#  define LZO_EXTERN(_rettype)  LZO_EXTERN_C _rettype __LZO_ENTRY __LZO_EXPORT
#endif
#if !defined(LZO_PUBLIC)
#  define LZO_PUBLIC(_rettype)  _rettype __LZO_ENTRY __LZO_EXPORT
#endif
#if !defined(LZO_PRIVATE)
#  define LZO_PRIVATE(_rettype) static _rettype __LZO_ENTRY __LZO_EXPORT
#endif


typedef int __LZO_ENTRY
(__LZO_EXPORT *lzo_compress_t)  ( const lzo_byte *src, lzo_uint  src_len,
                                        lzo_byte *dst, lzo_uint *dst_len,
                                        lzo_voidp wrkmem );

typedef int __LZO_ENTRY
(__LZO_EXPORT *lzo_decompress_t)( const lzo_byte *src, lzo_uint  src_len,
                                        lzo_byte *dst, lzo_uint *dst_len,
                                        lzo_voidp wrkmem );

typedef int __LZO_ENTRY
(__LZO_EXPORT *lzo_optimize_t)  (       lzo_byte *src, lzo_uint  src_len,
                                        lzo_byte *dst, lzo_uint *dst_len,
                                        lzo_voidp wrkmem );


/* a progress indicator callback function */
typedef void __LZO_ENTRY
(__LZO_EXPORT *lzo_progress_callback_t)(lzo_uint,lzo_uint);


/***********************************************************************
// error codes and prototypes
************************************************************************/

/* Error codes for the compression/decompression functions. Negative
 * values are errors, positive values will be used for special but
 * normal events.
 */
#define LZO_E_OK                    0
#define LZO_E_ERROR                 (-1)
#define LZO_E_NOT_COMPRESSIBLE      (-2)    /* not used right now */
#define LZO_E_EOF_NOT_FOUND         (-3)
#define LZO_E_INPUT_OVERRUN         (-4)
#define LZO_E_OUTPUT_OVERRUN        (-5)
#define LZO_E_LOOKBEHIND_OVERRUN    (-6)
#define LZO_E_OUT_OF_MEMORY         (-7)    /* not used right now */


/* this should be the first function you call. Check the return code ! */
LZO_EXTERN(int) lzo_init(void);

/* version functions (useful for shared libraries) */
LZO_EXTERN(unsigned) lzo_version(void);
LZO_EXTERN(const char *) lzo_version_string(void);
LZO_EXTERN(const char *) lzo_version_date(void);

/* string functions */
LZO_EXTERN(int)
lzo_memcmp(const lzo_voidp _s1, const lzo_voidp _s2, lzo_uint _len);
LZO_EXTERN(lzo_voidp)
lzo_memcpy(lzo_voidp _dest, const lzo_voidp _src, lzo_uint _len);
LZO_EXTERN(lzo_voidp)
lzo_memmove(lzo_voidp _dest, const lzo_voidp _src, lzo_uint _len);
LZO_EXTERN(lzo_voidp)
lzo_memset(lzo_voidp _s, int _c, lzo_uint _len);

/* checksum functions */
LZO_EXTERN(lzo_uint32)
lzo_adler32(lzo_uint32 _adler, const lzo_byte *_buf, lzo_uint _len);

/* misc. */
LZO_EXTERN(lzo_bool) lzo_assert(int _expr);
LZO_EXTERN(int) _lzo_config_check(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */

/*
vi:ts=4
*/
