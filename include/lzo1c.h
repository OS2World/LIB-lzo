/* lzo1c.h -- public interface of the LZO1C compression algorithm

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


#ifndef __LZO1C_H
#define __LZO1C_H

#ifndef __LZOCONF_H
#include <lzoconf.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
//
************************************************************************/

/* Memory required for the wrkmem parameter.
 * When the required size is 0, you can also pass a NULL pointer.
 */

#define LZO1C_MEM_COMPRESS      ((lzo_uint) (16384L * sizeof(lzo_byte *)))
#define LZO1C_MEM_DECOMPRESS    (0)


/* compression levels */
#define LZO1C_BEST_SPEED             1
#define LZO1C_BEST_COMPRESSION       9
#define LZO1C_DEFAULT_COMPRESSION  (-1)     /* fastest by default */


LZO_EXTERN(int)
lzo1c_compress          ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem,
                                int compression_level );

/* fast decompression */
LZO_EXTERN(int)
lzo1c_decompress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );

/* safe decompression with overrun testing */
LZO_EXTERN(int)
lzo1c_decompress_safe   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );

/* decompression in assembler - see notes in source code */
#if defined(__GNUC__) && defined(__i386__)
#  define LZO1C_DECOMPRESS_ASM
#endif
#if defined(LZO1C_DECOMPRESS_ASM)
LZO_EXTERN(int)
lzo1c_decompress_asm    ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );
LZO_EXTERN(int)
lzo1c_decompress_asm_safe(const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );
#endif


/***********************************************************************
//
************************************************************************/

LZO_EXTERN(int)
lzo1c_1_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_2_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_3_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_4_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_5_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_6_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_7_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_8_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
LZO_EXTERN(int)
lzo1c_9_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );


/***********************************************************************
// better compression ratio at the cost of more memory and time
************************************************************************/

#define LZO1C_99_MEM_COMPRESS   ((lzo_uint) (65536L * sizeof(lzo_byte *)))

#if !defined(LZO_99_UNSUPPORTED)
LZO_EXTERN(int)
lzo1c_99_compress       ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
#endif


#define LZO1C_999_MEM_COMPRESS  ((lzo_uint) (5 * 16384L * sizeof(short)))

#if !defined(LZO_999_UNSUPPORTED)
LZO_EXTERN(int)
lzo1c_999_compress      ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );
#endif



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */

/*
vi:ts=4
*/
