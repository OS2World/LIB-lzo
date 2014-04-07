/* lzo_test.c -- comprehensive test driver for the LZO library

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


#include <lzoconf.h>

#if defined(LZO_HAVE_CONFIG_AC)
#  include <config.ac>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>
#if defined(__DJGPP__) || defined(__BORLANDC__)
#  include <dir.h>
#endif
#if defined(HAVE_UNISTD_H) || defined(__DJGPP__) || defined(__EMX__)
#  include <unistd.h>
#endif

#include <time.h>
#if !defined(CLOCKS_PER_SEC) && defined(CLK_TCK)
#  define CLOCKS_PER_SEC	CLK_TCK
#endif
#if defined(UCLOCKS_PER_SEC)
#  undef clock
#  undef clock_t
#  undef CLOCKS_PER_SEC
#  define clock()			uclock()
#  define clock_t			uclock_t
#  define CLOCKS_PER_SEC	UCLOCKS_PER_SEC
#endif


/*************************************************************************
// include section
**************************************************************************/

#define HAVE_LZO1_H
#define HAVE_LZO1A_H
#define HAVE_LZO1B_H
#define HAVE_LZO1C_H
#define HAVE_LZO1F_H
#define HAVE_LZO1X_H
#define HAVE_LZO1Y_H
#define HAVE_LZO2A_H

#if defined(__LZO_MSDOS16)
#undef HAVE_LZO1_H
#undef HAVE_LZO1A_H
#undef HAVE_LZO1C_H
#undef HAVE_LZO2A_H
#undef HAVE_LZO2B_H
#endif

#if 1 && !defined(HAVE_ZLIB_H) && defined(MFX)
#define HAVE_ZLIB_H
#endif


/* LZO algorithms */
#if defined(HAVE_LZO1_H)
#  include <lzo1.h>
#endif
#if defined(HAVE_LZO1A_H)
#  include <lzo1a.h>
#endif
#if defined(HAVE_LZO1B_H)
#  include <lzo1b.h>
#endif
#if defined(HAVE_LZO1C_H)
#  include <lzo1c.h>
#endif
#if defined(HAVE_LZO1F_H)
#  include <lzo1f.h>
#endif
#if defined(HAVE_LZO1X_H)
#  include <lzo1x.h>
#endif
#if defined(HAVE_LZO1Y_H)
#  include <lzo1y.h>
#endif
#if defined(HAVE_LZO2A_H)
#  include <lzo2a.h>
#endif
#if defined(HAVE_LZO2B_H)
#  include <lzo2b.h>
#endif

/* other compressors */
#if defined(HAVE_ZLIB_H)
#  include <zlib.h>
#endif
#if defined(MFX)
#  include "contrib/t_config.ch"
#endif


/*************************************************************************
// define all methods
**************************************************************************/

enum {
/* compression algorithms */
	M_LZO1B_1  =     1,
	M_LZO1B_2, M_LZO1B_3, M_LZO1B_4, M_LZO1B_5,
	M_LZO1B_6, M_LZO1B_7, M_LZO1B_8, M_LZO1B_9,

	M_LZO1C_1  =    11,
	M_LZO1C_2, M_LZO1C_3, M_LZO1C_4, M_LZO1C_5,
	M_LZO1C_6, M_LZO1C_7, M_LZO1C_8, M_LZO1C_9,

	M_LZO1     =    21,
	M_LZO1A    =    31,

	M_LZO1B_99 =   901,
	M_LZO1B_999=   902,
	M_LZO1C_99 =   911,
	M_LZO1C_999=   912,
	M_LZO1_99  =   921,
	M_LZO1A_99 =   931,

	M_LZO1F_1  =    61,
	M_LZO1F_999=   962,
	M_LZO1X_1  =    71,
	M_LZO1X_999=   972,
	M_LZO1Y_1  =    81,
	M_LZO1Y_999=   982,

	M_LZO2A_999=   942,
	M_LZO2B_999=   952,

	M_LAST_LZO_COMPRESSOR = 998,

/* other compressors */
	M_ZLIB_8_1 =  1101,
	M_ZLIB_8_2, M_ZLIB_8_3, M_ZLIB_8_4, M_ZLIB_8_5,
	M_ZLIB_8_6, M_ZLIB_8_7, M_ZLIB_8_8, M_ZLIB_8_9,

/* dummy compressor - for speed comparision */
	M_MEMCPY   =   999,

	M_LAST_COMPRESSOR = 4999,

/* dummy algorithms - for speed comparision */
	M_MEMSET   =  5001,

/* checksum algorithms - for speed comparision */
	M_ADLER32  =  6001,

	M_UNUSED
};


#if defined(LZO_99_UNSUPPORTED)
#define M_LZO1_99		(-1)
#define M_LZO1A_99		(-1)
#define M_LZO1B_99		(-1)
#define M_LZO1C_99		(-1)
#endif
#if defined(LZO_999_UNSUPPORTED)
#define M_LZO1B_999		(-1)
#define M_LZO1C_999		(-1)
#define M_LZO1F_999		(-1)
#define M_LZO1X_999		(-1)
#define M_LZO1Y_999		(-1)
#define M_LZO2A_999		(-1)
#define M_LZO2B_999		(-1)
#endif



/*************************************************************************
// options
**************************************************************************/

#if defined(HAVE_LZO1X_H)
int default_method = M_LZO1X_1;
#elif defined(HAVE_LZO1B_H)
int default_method = M_LZO1B_1;
#elif defined(HAVE_LZO1C_H)
int default_method = M_LZO1C_1;
#elif defined(HAVE_LZO1F_H)
int default_method = M_LZO1F_1;
#else
int default_method = 0;
#endif

int verbose = 2;

lzo_bool use_safe_decompressor = 0;
lzo_bool use_asm_decompressor = 0;
lzo_bool optimize_compressed_data = 0;

lzo_bool try_to_compress_0_bytes = 1;

/* set these to 1 to measure the speed impact of a checksum (option '-a') */
lzo_bool compute_adler32 = 0;
lzo_bool compute_crc32 = 0;

static lzo_uint32 adler_in, adler_out;


static const short all_methods[] = {
	M_LZO1,
	M_LZO1A,
	M_LZO1B_1, M_LZO1B_2, M_LZO1B_3, M_LZO1B_4, M_LZO1B_5,
	M_LZO1B_6, M_LZO1B_7, M_LZO1B_8, M_LZO1B_9,
	M_LZO1C_1, M_LZO1C_2, M_LZO1C_3, M_LZO1C_4, M_LZO1C_5,
	M_LZO1C_6, M_LZO1C_7, M_LZO1C_8, M_LZO1C_9,
	M_LZO1F_1,
	M_LZO1X_1,
	M_LZO1_99, M_LZO1A_99, M_LZO1B_99, M_LZO1C_99,
	M_LZO1B_999, M_LZO1C_999, M_LZO1F_999, M_LZO1X_999, M_LZO1Y_999,
	M_LZO2A_999,
	0
};

static const short benchmark_methods[] = {
	M_LZO1B_1, M_LZO1B_9,
	M_LZO1C_1, M_LZO1C_9,
	M_LZO1F_1,
	M_LZO1X_1,
	0
};

static const short x1_methods[] = {
	M_LZO1, M_LZO1A, M_LZO1B_1, M_LZO1C_1, M_LZO1F_1, M_LZO1X_1,
	0
};

static const short x99_methods[] = {
	M_LZO1_99, M_LZO1A_99, M_LZO1B_99, M_LZO1C_99,
	0
};

static const short x999_methods[] = {
	M_LZO1B_999, M_LZO1C_999, M_LZO1F_999, M_LZO1X_999, M_LZO1Y_999,
	M_LZO2A_999,
	0
};

static const short zlib_methods[] = {
	M_ZLIB_8_1, M_ZLIB_8_2, M_ZLIB_8_3, M_ZLIB_8_4, M_ZLIB_8_5,
	M_ZLIB_8_6, M_ZLIB_8_7, M_ZLIB_8_8, M_ZLIB_8_9,
	0
};

static const short avail_methods[] = {
	0
};


/* exit codes of this test program */
#define EXIT_OK			0
#define EXIT_USAGE		1
#define EXIT_FILE		2
#define EXIT_MEM		3
#define EXIT_ADLER		4
#define EXIT_LZO_ERROR	5
#define EXIT_LZO_INIT	6
#define EXIT_INTERNAL	7


/*************************************************************************
// memory setup
**************************************************************************/

#if (UINT_MAX >= 0xffffffffL)		/* 32 bit or more */

#define BLOCK_SIZE		(256*1024l)
#define MAX_BLOCK_SIZE	(4*BLOCK_SIZE)
#define DATA_LEN		(1024*1024l)
#define WORK_LEN		(768*1024l)

#elif defined(__LZO_ATARI16)

/* adjust memory so that it works on a 4 meg Atari ST */
#define BLOCK_SIZE		(256*1024l)
#define MAX_BLOCK_SIZE	(1*BLOCK_SIZE)
#define DATA_LEN		(1024*1024l)
#define WORK_LEN		(768*1024l)

#else								/* 16 bit MSDOS */

/* have to squeeze everything into ~600 kB  */
#if 0
#define BLOCK_SIZE		(256*1024l)
#define MAX_BLOCK_SIZE	(256*1024l)
#define DATA_LEN		(128*1024l)
#else
#define BLOCK_SIZE		32000u
#define MAX_BLOCK_SIZE	32000u
#define DATA_LEN		(60*1024l)
#endif
#define WORK_LEN		(64*1024l)

#endif


static lzo_uint block_size = BLOCK_SIZE;

#ifndef LZO_MEMALLOC_T
#define LZO_MEMALLOC_T	lzo_byte
#endif
static LZO_MEMALLOC_T	_block1[MAX_BLOCK_SIZE * 9L / 8 + 256 + 1024];
static LZO_MEMALLOC_T	_block2[MAX_BLOCK_SIZE * 9L / 8 + 256 + 1024];
static LZO_MEMALLOC_T	_data[DATA_LEN + 256];
static LZO_MEMALLOC_T	_wrkmem[WORK_LEN + 256];

/* align memory blocks (cache issues) */
static lzo_byte *block1 = NULL;
static lzo_byte *block2 = NULL;
static lzo_byte *data = NULL;
static lzo_byte *wrkmem = NULL;

static void align_mem(void)
{
	block1 = (lzo_byte *) LZO_ALIGN(_block1,256);
	block2 = (lzo_byte *) LZO_ALIGN(_block2,256);
	data   = (lzo_byte *) LZO_ALIGN(_data,256);
	wrkmem = (lzo_byte *) LZO_ALIGN(_wrkmem,256);
}


/*************************************************************************
// zlib wrapper
**************************************************************************/

#if defined(HAVE_ZLIB_H)

int zlib_compress       ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem,
								int c, int level )
{
	/* use the undocumented feature to suppress the zlib header */
	z_stream stream;
	int err;

	stream.next_in = (lzo_byte *) src;		/* UNCONST */
	stream.avail_in = src_len;
	stream.next_out = dst;
	stream.avail_out = *dst_len;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	*dst_len = 0;
#if 0
	err = deflateInit(&stream, level);
#else
	err = deflateInit2(&stream, level, c, -MAX_WBITS,
	                   MAX_MEM_LEVEL > 8 ? 8 : MAX_MEM_LEVEL,
	                   Z_DEFAULT_STRATEGY);
#endif
	if (err == Z_OK)
	{
		err = deflate(&stream, Z_FINISH);
		if (err != Z_STREAM_END)
		{
			deflateEnd(&stream);
			err = (err == Z_OK) ? Z_BUF_ERROR : err;
		}
		else
		{
			*dst_len = (lzo_uint) stream.total_out;
			err = deflateEnd(&stream);
		}
	}
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return (err == Z_OK) ? 0 : -1;
}


int zlib_decompress     ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	/* use the undocumented feature to suppress the zlib header */
	z_stream stream;
	int err;

	stream.next_in = (lzo_byte *) src;		/* UNCONST */
	stream.avail_in = src_len;
	stream.next_out = dst;
	stream.avail_out = *dst_len;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	*dst_len = 0;
#if 0
	err = inflateInit(&stream);
#else
	err = inflateInit2(&stream, -MAX_WBITS);
#endif
	if (err == Z_OK)
	{
		err = inflate(&stream, Z_FINISH);
		if (err == Z_OK)		/* sometimes returns Z_OK !!! */
		{
			*dst_len = (lzo_uint) stream.total_out;
			err = inflateEnd(&stream);
		}
		else if (err == Z_STREAM_END)
		{
			*dst_len = (lzo_uint) stream.total_out;
			err = inflateEnd(&stream);
		}
		else
			inflateEnd(&stream);
	}
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return (err == Z_OK) ? 0 : -1;
}


int zlib_8_1_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,1); }

int zlib_8_2_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,2); }

int zlib_8_3_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,3); }

int zlib_8_4_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,4); }

int zlib_8_5_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,5); }

int zlib_8_6_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,6); }

int zlib_8_7_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,7); }

int zlib_8_8_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,8); }

int zlib_8_9_compress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{ return zlib_compress(src,src_len,dst,dst_len,wrkmem,Z_DEFLATED,9); }


#endif /* HAVE_ZLIB_H */


/*************************************************************************
// other wrappers
**************************************************************************/

int memcpy_compress     ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_memcpy(dst,src,src_len);
	*dst_len = src_len;
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}

int memcpy_decompress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_memcpy(dst,src,src_len);
	*dst_len = src_len;
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}


int memset_compress     ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_memset(dst,0,src_len);
	*dst_len = src_len;
	if (src) src = 0;		/* avoid warning */
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}

int memset_decompress   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_memset(dst,0,src_len);
	*dst_len = src_len;
	if (src) src = 0;		/* avoid warning */
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}


int adler32_compress    ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_uint32 adler;
	adler = lzo_adler32(0, NULL, 0);
	adler = lzo_adler32(adler, src, src_len);
	*dst_len = src_len;
	if (dst) dst = 0;		/* avoid warning */
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}

int adler32_decompress  ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem )
{
	lzo_uint32 adler;
	adler = lzo_adler32(0, NULL, 0);
	adler = lzo_adler32(adler, dst, src_len);
	*dst_len = src_len;
	if (src) src = 0;		/* avoid warning */
	if (wrkmem) wrkmem = 0;	/* avoid warning */
	return 0;
}

#if defined(MFX)
#  include "contrib/t_wrap.ch"
#endif


/*************************************************************************
// compression database
**************************************************************************/

typedef struct
{
	const char *		name;
	int 				id;
	lzo_uint			mem_compress;
	lzo_uint			mem_decompress;
	lzo_compress_t		compress;
	lzo_optimize_t		optimize;
	lzo_decompress_t	decompress;
	lzo_decompress_t	decompress_safe;
	lzo_decompress_t	decompress_asm;
	lzo_decompress_t	decompress_asm_safe;
}
compress_t;


#if !defined(LZO1B_DECOMPRESS_ASM)
#  define lzo1b_decompress_asm			0
#  define lzo1b_decompress_asm_safe		0
#endif
#if !defined(LZO1C_DECOMPRESS_ASM)
#  define lzo1c_decompress_asm			0
#  define lzo1c_decompress_asm_safe		0
#endif
#if !defined(LZO1F_DECOMPRESS_ASM)
#  define lzo1f_decompress_asm			0
#  define lzo1f_decompress_asm_safe		0
#endif
#if !defined(LZO1X_DECOMPRESS_ASM)
#  define lzo1x_decompress_asm			0
#  define lzo1x_decompress_asm_safe		0
#endif
#if !defined(LZO1Y_DECOMPRESS_ASM)
#  define lzo1y_decompress_asm			0
#  define lzo1y_decompress_asm_safe		0
#endif


static const compress_t compress_database[] = {

#if defined(HAVE_LZO1_H)
{ "LZO1-1", M_LZO1, LZO1_MEM_COMPRESS, LZO1_MEM_DECOMPRESS,
  lzo1_compress,            0,
  lzo1_decompress,          0,
  0,                        0 },
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1-99", M_LZO1_99, LZO1_99_MEM_COMPRESS, LZO1_MEM_DECOMPRESS,
  lzo1_99_compress,         0,
  lzo1_decompress,          0,
  0,                        0 },
#endif
#endif

#if defined(HAVE_LZO1A_H)
{ "LZO1A-1", M_LZO1A, LZO1A_MEM_COMPRESS, LZO1A_MEM_DECOMPRESS,
  lzo1a_compress,           0,
  lzo1a_decompress,         0,
  0,                        0 },
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1A-99", M_LZO1A_99, LZO1A_99_MEM_COMPRESS, LZO1A_MEM_DECOMPRESS,
  lzo1a_99_compress,        0,
  lzo1a_decompress,         0,
  0,                        0 },
#endif
#endif

#if defined(HAVE_LZO1B_H)
{ "LZO1B-1", M_LZO1B_1, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_1_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-2", M_LZO1B_2, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_2_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-3", M_LZO1B_3, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_3_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-4", M_LZO1B_4, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_4_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-5", M_LZO1B_5, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_5_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-6", M_LZO1B_6, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_6_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-7", M_LZO1B_7, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_7_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-8", M_LZO1B_8, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_8_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
{ "LZO1B-9", M_LZO1B_9, LZO1B_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_9_compress,         0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
#if !defined(LZO_99_UNSUPPORTED)
{ "LZO1B-99", M_LZO1B_99, LZO1B_99_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_99_compress,        0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
#endif
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1B-999", M_LZO1B_999, LZO1B_999_MEM_COMPRESS, LZO1B_MEM_DECOMPRESS,
  lzo1b_999_compress,       0,
  lzo1b_decompress,         lzo1b_decompress_safe,
  lzo1b_decompress_asm,     lzo1b_decompress_asm_safe },
#endif
#endif

#if defined(HAVE_LZO1C_H)
{ "LZO1C-1", M_LZO1C_1, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_1_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-2", M_LZO1C_2, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_2_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-3", M_LZO1C_3, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_3_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-4", M_LZO1C_4, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_4_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-5", M_LZO1C_5, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_5_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-6", M_LZO1C_6, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_6_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-7", M_LZO1C_7, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_7_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-8", M_LZO1C_8, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_8_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
{ "LZO1C-9", M_LZO1C_9, LZO1C_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_9_compress,         0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
#if !defined(LZO_99_UNSUPPORTED)
{ "LZO1C-99", M_LZO1C_99, LZO1C_99_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_99_compress,        0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
#endif
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1C-999", M_LZO1C_999, LZO1C_999_MEM_COMPRESS, LZO1C_MEM_DECOMPRESS,
  lzo1c_999_compress,       0,
  lzo1c_decompress,         lzo1c_decompress_safe,
  lzo1c_decompress_asm,     lzo1c_decompress_asm_safe },
#endif
#endif

#if defined(HAVE_LZO1F_H)
{ "LZO1F-1", M_LZO1F_1, LZO1F_MEM_COMPRESS, LZO1F_MEM_DECOMPRESS,
  lzo1f_1_compress,         0,
  lzo1f_decompress,         lzo1f_decompress_safe,
  lzo1f_decompress_asm,     lzo1f_decompress_asm_safe },
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1F-999", M_LZO1F_999, LZO1F_999_MEM_COMPRESS, LZO1F_MEM_DECOMPRESS,
  lzo1f_999_compress,       0,
  lzo1f_decompress,         lzo1f_decompress_safe,
  lzo1f_decompress_asm,     lzo1f_decompress_asm_safe },
#endif
#endif

#if defined(HAVE_LZO1X_H)
{ "LZO1X-1", M_LZO1X_1, LZO1X_MEM_COMPRESS, LZO1X_MEM_DECOMPRESS,
  lzo1x_1_compress,         lzo1x_optimize,
  lzo1x_decompress,         lzo1x_decompress_safe,
  lzo1x_decompress_asm,     lzo1x_decompress_asm_safe },
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1X-999", M_LZO1X_999, LZO1X_999_MEM_COMPRESS, LZO1X_MEM_DECOMPRESS,
  lzo1x_999_compress,       lzo1x_optimize,
  lzo1x_decompress,         lzo1x_decompress_safe,
  lzo1x_decompress_asm,     lzo1x_decompress_asm_safe },
#endif
#endif

#if defined(HAVE_LZO1Y_H)
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO1Y-999", M_LZO1Y_999, LZO1Y_999_MEM_COMPRESS, LZO1Y_MEM_DECOMPRESS,
  lzo1y_999_compress,       lzo1y_optimize,
  lzo1y_decompress,         lzo1y_decompress_safe,
  lzo1y_decompress_asm,     lzo1y_decompress_asm_safe },
#endif
#endif

#if defined(HAVE_LZO2A_H)
#if !defined(LZO_999_UNSUPPORTED)
{ "LZO2A-999", M_LZO2A_999, LZO2A_999_MEM_COMPRESS, LZO2A_MEM_DECOMPRESS,
  lzo2a_999_compress,       0,
  lzo2a_decompress,         lzo2a_decompress_safe,
  0,                        0 },
#endif
#endif

#if defined(HAVE_ZLIB_H)
{ "zlib-8/1", M_ZLIB_8_1, 0, 0, zlib_8_1_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/2", M_ZLIB_8_2, 0, 0, zlib_8_2_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/3", M_ZLIB_8_3, 0, 0, zlib_8_3_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/4", M_ZLIB_8_4, 0, 0, zlib_8_4_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/5", M_ZLIB_8_5, 0, 0, zlib_8_5_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/6", M_ZLIB_8_6, 0, 0, zlib_8_6_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/7", M_ZLIB_8_7, 0, 0, zlib_8_7_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/8", M_ZLIB_8_8, 0, 0, zlib_8_8_compress, 0,
                                zlib_decompress, 0, 0, 0 },
{ "zlib-8/9", M_ZLIB_8_9, 0, 0, zlib_8_9_compress, 0,
                                zlib_decompress, 0, 0, 0 },
#endif

{ "memcpy()", M_MEMCPY, 0, 0,   memcpy_compress, 0,
                                memcpy_decompress, 0, 0, 0 },
{ "memset()", M_MEMSET, 0, 0,   memset_compress, 0,
                                memset_decompress, 0, 0, 0 },
{ "adler32()", M_ADLER32, 0, 0, adler32_compress, 0,
                                adler32_decompress, 0, 0, 0 },

#if defined(MFX)
#  include "contrib/t_db.ch"
#endif

{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};


/*************************************************************************
// method info
**************************************************************************/

static lzo_decompress_t decomp_info(const compress_t *c, const char **nn)
{
	lzo_decompress_t d = NULL;
	const char *n = NULL;

	if (!d && use_safe_decompressor && use_asm_decompressor)
	{
		d = c->decompress_asm_safe;
		n = " [as]";
	}
	if (!d && use_safe_decompressor)
	{
		d = c->decompress_safe;
		n = " [s]";
	}
	if (!d && use_asm_decompressor)
	{
		d = c->decompress_asm;
		n = " [a]";
	}
	if (!d)
	{
		d = c->decompress;
		n = "";
	}
	if (!d)
	{
		d = NULL;
		n = NULL;
	}

	if (nn)
		*nn = n;
	return d;
}


static const compress_t *info(int method, FILE *f)
{
	const compress_t *c = NULL;
	const compress_t *db = compress_database;
	size_t size = sizeof(compress_database) / sizeof(*(compress_database));
	size_t i;

	if (method > 0)
	{
		for (i = 0; i < size && db->name != NULL; i++, db++)
		{
			if (method == db->id &&
			    WORK_LEN >= db->mem_compress &&
			    WORK_LEN >= db->mem_decompress)
			{
				c = db;
				break;
			}
		}
	}

	if (f != NULL)
	{
		if (c == NULL)
		{
			fprintf(f,"invalid method %d !\n",method);
			exit(EXIT_USAGE);
		}
		else
		{
			const char *n;
			decomp_info(c,&n);
			fprintf(f,"%s%s",c->name,n);
		}
	}
	return c;
}


/*************************************************************************
// compress a block
**************************************************************************/

static
int do_compress         ( const compress_t *c,
                          const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len )
{
	int r = -100;

	if (c && c->compress && WORK_LEN >= c->mem_compress)
		r = c->compress(src,src_len,dst,dst_len,wrkmem);

	if (r == 0 && compute_adler32)
	{
		lzo_uint32 adler;
		adler = lzo_adler32(0, NULL, 0);
		adler = lzo_adler32(adler, src, src_len);
		adler_in = adler;
	}
#if defined(HAVE_ZLIB_H)
	if (r == 0 && compute_crc32)
	{
		uLong crc;
		crc = crc32(0L, Z_NULL, 0);
		crc = crc32(crc, src, src_len);
	}
#endif

	return r;
}


/*************************************************************************
// decompress a block
**************************************************************************/

static
int do_decompress       ( const compress_t *c, lzo_decompress_t d,
                          const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len )
{
	int r = -100;

   	if (c && d && WORK_LEN >= c->mem_decompress)
		r = d(src,src_len,dst,dst_len,wrkmem);

	if (r == 0 && compute_adler32)
	{
		lzo_uint32 adler;
		adler = lzo_adler32(0, NULL, 0);
		adler = lzo_adler32(adler, dst, *dst_len);
		adler_out = adler;
	}
#if defined(HAVE_ZLIB_H)
	if (r == 0 && compute_crc32)
	{
		uLong crc;
		crc = crc32(0L, Z_NULL, 0);
		crc = crc32(crc, dst, *dst_len);
	}
#endif

	return r;
}


/*************************************************************************
// optimize a block
**************************************************************************/

int do_optimize         ( const compress_t *c,
                                lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len )
{
	if (c && c->optimize && WORK_LEN >= c->mem_decompress)
		return c->optimize(src,src_len,dst,dst_len,wrkmem);
	else
		return 0;
}


/***********************************************************************
// portable fread
************************************************************************/

lzo_uint my_fread(FILE *f, lzo_voidp s, lzo_uint len)
{
#if (LZO_UINT_MAX <= UINT_MAX)
	return fread(s,1,len,f);
#else
	lzo_byte *p = (lzo_byte *) s;
	lzo_uint l = 0;
	size_t k;
	unsigned char buf[512], *b;

	while (l < len)
	{
		k = len - l > sizeof(buf) ? sizeof(buf) : (size_t) (len - l);
		k = fread(buf,1,k,f);
		if (k <= 0)
			break;
		l += k;
		b = buf;
		do *p++ = *b++; while (--k > 0);
	}
	return l;
#endif
}


/*************************************************************************
// compress and decompress a file
**************************************************************************/

static int do_file(int method, const char *file_name,
                   int c_loops, int d_loops, lzo_uint32 *ad)
{
	const compress_t *c;
	lzo_decompress_t decompress;
	FILE *f;
	int i, r = 0;
	lzo_uint l = 0, t_len = 0;
	unsigned long ll;
	int blocks = 0;
	int t_loops = 1;
	clock_t t_time = 0, c_time = 0, d_time = 0;
	clock_t t_start, c_start, d_start;
	unsigned long t_bytes, c_bytes, d_bytes;
	double t_kbs, c_kbs, d_kbs;
	double t_secs, c_secs, d_secs;
	double perc, bits;
	lzo_uint32 adler;
	char method_name[32];
	const char *n;

	adler_in = adler_out = 0;
	if (ad)
		*ad = 0;

	c = info(method,NULL);
	if (c == NULL || c->name == NULL || c->compress == NULL)
		return EXIT_INTERNAL;
	decompress = decomp_info(c,&n);
	if (decompress == NULL || n == NULL || WORK_LEN < c->mem_decompress)
		return EXIT_INTERNAL;
	strcpy(method_name,c->name);
	strcat(method_name,n);

	if (c_loops < 1)  c_loops = 1;
	if (d_loops < 1)  d_loops = 1;

	fflush(stdout);
	fflush(stderr);

	/* read the whole file */
	f = fopen(file_name,"rb");
	if (f == NULL)
	{
		fprintf(stderr,"%s: ",file_name);
		perror("fopen");
		return 0;				/* do not raise an error */
	}
	l = my_fread(f,data,DATA_LEN);
	if (fclose(f) != 0)
	{
		fprintf(stderr,"%s: ",file_name);
		perror("fclose");
		return EXIT_FILE;
	}

	adler = lzo_adler32(0, NULL, 0);
	adler = lzo_adler32(adler, data, l);
	if (ad)
		*ad = adler;

	if (verbose >= 2)
	{
		printf("File %s: %lu bytes   adler32 = 0x%08lx\n",
		        file_name, (long) l, (long) adler);
		printf("  compressing %lu bytes (%d/%d/%d loops, %lu block-size)\n",
		        (long) l, t_loops, c_loops, d_loops, (long) block_size);
		printf("  %s\n", method_name);
	}

	/* compress the file */

	t_start = clock();
	for (i = 0; i < t_loops; i++)
	{
		lzo_uint len, c_len, c_len_max, d_len;
		lzo_byte *d = data;

		len = l;
		t_len = 0;
		blocks = 0;
		if (len > 0 || try_to_compress_0_bytes) do
		{
			int j;
			lzo_byte *dd = d;
			lzo_byte *b1 = block1;
			lzo_byte *b2 = block2;
			const lzo_uint bl = len > block_size ? block_size : len;
			lzo_uint bl_overwrite = bl;
#if !defined(__BOUNDS_CHECKING_ON)
			unsigned char random_byte;
#endif

			if (use_asm_decompressor)
				bl_overwrite += 3;		/* may overwrite 3 bytes past the end */

			blocks++;

#if defined(__BOUNDS_CHECKING_ON)
			/* malloc a block of the exact block size to detect
		 	* any overun.
		 	*/
			dd = malloc(bl_overwrite);
			if (dd == NULL)
				return EXIT_MEM;
			memcpy(dd,d,bl);
			b2 = dd;
#endif

		/* compress a block */
			c_len = c_len_max = 0;
			c_start = clock();
			for (j = r = 0; r == 0 && j < c_loops; j++)
			{
				c_len = sizeof(_block1) - 256;
				r = do_compress(c,dd,bl,b1,&c_len);
				if (r == 0 && c_len > c_len_max)
					c_len_max = c_len;
			}
			c_time += clock() - c_start;
			if (r != 0)
			{
				printf("  compression failed in block %d (%d) (%lu %lu)\n",
					blocks, r, (long)c_len, (long)bl);
				return EXIT_LZO_ERROR;
			}

		/* optimize the block */
			r = 0;
			d_len = bl;
			if (optimize_compressed_data)
				r = do_optimize(c,b1,c_len,b2,&d_len);
			if (r != 0 || d_len != bl)
			{
				printf("  optimization failed in block %d (%d) "
					"(%lu %lu %lu)\n", blocks, r,
					(long)c_len, (long)d_len, (long)bl);
				return EXIT_LZO_ERROR;
			}

		/* decompress the block and verify */
#if defined(__BOUNDS_CHECKING_ON)
			lzo_memset(b2,0,bl);
#else
			lzo_memset(b2,0,bl_overwrite);
			/* check that no data past the end gets overwritten */
			random_byte = (unsigned char) c_start;
			b2[bl_overwrite] = random_byte;
#endif
			d_start = clock();
			for (j = r = 0; r == 0 && j < d_loops; j++)
			{
				d_len = bl;
				r = do_decompress(c,decompress,b1,c_len,b2,&d_len);
				if (d_len != bl)
					break;
			}
			d_time += clock() - d_start;
			if (r != 0)
			{
				printf("  decompression failed in block %d (%d) "
					"(%lu %lu %lu)\n", blocks, r,
					(long)c_len, (long)d_len, (long)bl);
				return EXIT_LZO_ERROR;
			}
			if (d_len != bl)
			{
				printf("  decompression size error in block %d (%lu %lu %lu)\n",
					blocks, (long)c_len, (long)d_len, (long)bl);
				return EXIT_LZO_ERROR;
			}
			if (method <= M_LAST_COMPRESSOR)
			{
				if (lzo_memcmp(d,b2,bl) != 0)
				{
					lzo_uint x = 0;
					while (x < bl && b2[x] == d[x])
						x++;
					printf("  decompression data error in block %d at offset "
						"%lu (%lu %lu)\n", blocks, (long)x,
						(long)c_len, (long)d_len);
					if (compute_adler32)
						printf("      checksum: 0x%08lx 0x%08lx\n",
							(long)adler_in, (long)adler_out);
#if 0
					printf("Orig:  ");
					for (j = (x >= 10 ? -10 : 0); j <= 10 && x + j < bl; j++)
						printf(" %02x", (int)b2[x+j]);
					printf("\nDecomp:");
					for (j = (x >= 10 ? -10 : 0); j <= 10 && x + j < bl; j++)
						printf(" %02x", (int)d[x+j]);
					printf("\n");
#endif
					return EXIT_LZO_ERROR;
				}
				if (compute_adler32 && adler_in != adler_out)
				{
					printf("  checksum error in block %d (%lu %lu)\n",
						blocks, (long)c_len, (long)d_len);
					printf("      checksum: 0x%08lx 0x%08lx\n",
						(long)adler_in, (long)adler_out);
					return EXIT_LZO_ERROR;
				}
			}
#if !defined(__BOUNDS_CHECKING_ON)
			if (b2[bl_overwrite] != random_byte)
			{
				printf("  decompression overwrite error in block %d "
					"(%lu %lu %lu)\n",
					blocks, (long)c_len, (long)d_len, (long)bl);
				return EXIT_LZO_ERROR;
			}
#endif

#if defined(__BOUNDS_CHECKING_ON)
			if (dd != d)
				free(dd);
#endif
			d += bl;
			len -= bl;
			t_len += c_len_max;
		}
		while (len > 0);
	}
	t_time += clock() - t_start;


	/* print some statistics */

	ll = l;
	perc = (ll > 0) ? t_len * 100.0 / ll : 0;
	bits = perc * 0.08;
	t_secs = t_time / (double)(CLOCKS_PER_SEC);
	t_bytes = (ll * c_loops + ll * d_loops) * t_loops;
	t_kbs = (t_secs > 0.001) ? (t_bytes / t_secs) / 1000.0 : 0;
	c_secs = c_time / (double)(CLOCKS_PER_SEC);
	c_bytes = ll * c_loops * t_loops;
	c_kbs = (c_secs > 0.001) ? (c_bytes / c_secs) / 1000.0 : 0;
	d_secs = d_time / (double)(CLOCKS_PER_SEC);
	d_bytes = ll * d_loops * t_loops;
	d_kbs = (d_secs > 0.001) ? (d_bytes / d_secs) / 1000.0 : 0;

	if (verbose >= 2)
	{
		printf("  compressed into %lu bytes, %.2f%%   %.3f\n",
			(long) t_len, perc, bits);

		printf("%-15s %5d: ","overall", t_loops);
		printf("%10lu bytes, %8.2f secs, %8.2f KB/sec\n",
		        t_bytes, t_secs, t_kbs);
		printf("%-15s %5d: ","compress", c_loops);
		printf("%10lu bytes, %8.2f secs, %8.2f KB/sec\n",
		        c_bytes, c_secs, c_kbs);
		printf("%-15s %5d: ","decompress", d_loops);
		printf("%10lu bytes, %8.2f secs, %8.2f KB/sec\n",
		        d_bytes, d_secs, d_kbs);
		printf("\n");
	}

	/* create a line for util/table.pl */

	if (verbose >= 1)
	{
		/* get basename */
		const char *n, *nn;
		for (nn = n = file_name; *nn; nn++)
			if (*nn == '/' || *nn == '\\' || *nn == ':')
				n = nn + 1;

		printf("%-12s | %-14s %7ld %4d %7ld %6.1f %9.2f %9.2f |\n",
			method_name,
			n, (long) l, blocks, (long) t_len, perc, c_kbs, d_kbs);
	}

	if (verbose >= 2)
		printf("\n");

	return EXIT_OK;
}


/*************************************************************************
// Calgary Corpus test suite driver
**************************************************************************/

static const struct
{
	const char *name;
	int loops;
	lzo_uint32 adler;
}
corpus[] =
{
	{ "bib",       8,  0x4bd09e98L },
	{ "book1",     1,  0xd4d3613eL },
	{ "book2",     1,  0x6fe14cc3L },
	{ "geo",       6,  0xf3cc5be0L },
	{ "news",      2,  0x2ed405b8L },
	{ "obj1",     35,  0x3887dd2cL },
	{ "obj2",      4,  0xf89407c4L },
	{ "paper1",   17,  0xfe65ce62L },
	{ "paper2",   11,  0x1238b7c2L },
	{ "pic",       4,  0xf61a5702L },
	{ "progc",    25,  0x4c00ba45L },
	{ "progl",    20,  0x4cba738eL },
	{ "progp",    28,  0x7495b92bL },
	{ "trans",    15,  0x52a2cec8L },
	{ NULL,        0,  0x00000000L },
	{ "paper3",   30,  0x50b727a9L },
	{ "paper4",   30,  0xcb4a305fL },
	{ "paper5",   30,  0x2ca8a6f3L },
	{ "paper6",   30,  0x9ddbcfa4L },
	{ NULL,        0,  0x00000000L }
};


static int do_corpus(int method, const char *path, int c_loops, int d_loops)
{
	size_t i, n, size;
	char name[256];

	if (path == NULL || strlen(path) >= sizeof(name) - 12)
		return EXIT_USAGE;

	strcpy(name,path);
	n = strlen(name);
	if (n > 0 && name[n-1] != '/' && name[n-1] != '\\')
	{
		strcat(name,"/");
		n++;
	}

	size = sizeof(corpus) / sizeof(*(corpus));
	for (i = 0; i < size && corpus[i].name != NULL; i++)
	{
		lzo_uint32 adler;
		int c = c_loops * corpus[i].loops;
		int d = d_loops * corpus[i].loops;
		int r;

		strcpy(name+n,corpus[i].name);
		r = do_file(method, name, c, d, &adler);
		if (r != 0)
			return r;
		if (adler != corpus[i].adler)
		{
			printf("  invalid test suite\n");
			return EXIT_ADLER;
		}
	}
	return EXIT_OK;
}


/*************************************************************************
//
**************************************************************************/

static
void usage(const char *name, int exit_code, lzo_bool show_methods)
{
	FILE *f;
	int i;

	f = stdout;

	fflush(stdout); fflush(stderr);

	fprintf(f,"Usage: %s [option..] file...\n", name);
	fprintf(f,"\n");
	fprintf(f,"Options:\n");
	fprintf(f,"  -m#     compression method\n");
	fprintf(f,"  -b#     set input block size (default %ld)\n",
		(long) block_size);
	fprintf(f,"  -n#     number of compression/decompression runs\n");
	fprintf(f,"  -c#     number of compression runs\n");
	fprintf(f,"  -d#     number of decompression runs\n");
	fprintf(f,"  -S      use safe decompressor (if available)\n");
	fprintf(f,"  -A      use assembler decompressor (if available)\n");
	fprintf(f,"  -O      optimize compressed data (if available)\n");
	fprintf(f,"  -s dir  process Calgary Corpus test suite in directory 'dir'\n");
	fprintf(f,"  -@      read list of files to compress from stdin\n");
	fprintf(f,"  -q      be quiet\n");
	fprintf(f,"  -Q      be very quiet\n");
	fprintf(f,"  -L      display software license\n");

	if (show_methods)
	{
		fprintf(f,"\n\n");
		fprintf(f,"The following compression methods are available:\n");
		fprintf(f,"\n");
		fprintf(f,"  usage   name                available extras\n");
		fprintf(f,"  -----   ----                ----------------\n");

		for (i = 0; i <= M_LAST_COMPRESSOR; i++)
		{
			const compress_t *c;
			c = info(i,NULL);
			if (c)
			{
				char n[16];
				static const char * const s[3] = {"          ", ", ", ""};
				int j = 0;

				sprintf(n,"-m%d",i);
				fprintf(f,"  %-6s  %-10s",n,c->name);
				if (c->decompress_safe)
					fprintf(f, "%s%s", j++ == 0 ? s[0] : s[1], "safe");
				if (c->decompress_asm)
					fprintf(f, "%s%s", j++ == 0 ? s[0] : s[1], "asm");
				if (c->decompress_asm_safe)
					fprintf(f, "%s%s", j++ == 0 ? s[0] : s[1], "asm+safe");
				if (c->optimize)
					fprintf(f, "%s%s", j++ == 0 ? s[0] : s[1], "optimize");
				if (j > 0)
					fprintf(f, s[2]);
				fprintf(f,"\n");
			}
		}
	}
	else
	{
		fprintf(f,"\n");
		fprintf(f,"Type '%s -m' to list all available methods.\n", name);
	}

	fflush(f);
	if (exit_code < 0)
		exit_code = EXIT_USAGE;
	exit(exit_code);
}


static
void license(void)
{
	FILE *f;

	f = stdout;

	fflush(stdout); fflush(stderr);

fprintf(f,
"   The LZO library is free software; you can redistribute it and/or\n"
"   modify it under the terms of the GNU General Public License as\n"
"   published by the Free Software Foundation; either version 2 of\n"
"   the License, or (at your option) any later version.\n"
"\n"
"   The LZO library is distributed in the hope that it will be useful,\n"
"   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"   GNU General Public License for more details.\n"
"\n"
"   You should have received a copy of the GNU General Public License\n"
"   along with the LZO library; see the file COPYING.\n"
"   If not, write to the Free Software Foundation, Inc.,\n"
"   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.\n"
"\n"
"   Markus F.X.J. Oberhumer\n"
"   markus.oberhumer@jk.uni-linz.ac.at\n"
"\n"
	);

	fflush(f);
	exit(EXIT_OK);
}


/*************************************************************************
//
**************************************************************************/

int main(int argc, char *argv[])
{
	int i = 1, ii, j = 0;
	int c_loops = 0;
	int d_loops = 0;
	int r = EXIT_OK;
	int method = default_method;
	const short *methods = NULL;
	const char *corpus_path = NULL;
	lzo_bool read_from_stdin = 0;
	int a_method = 0;
	const char *argv0 = "";
	time_t t_total;

#if defined(__EMX__)
	_response(&argc,&argv);
	_wildcard(&argc,&argv);
#endif

	argv0 = argv[0];
	align_mem();

	printf("\nLZO real-time data compression library (v%s, %s).\n",
			LZO_VERSION_STRING, LZO_VERSION_DATE);
	printf("Copyright (C) 1996, 1997 Markus Franz Xaver Johannes Oberhumer\n\n");

	if (lzo_init() != LZO_E_OK)
	{
		printf("lzo_init() failed !!!\n");
		exit(EXIT_LZO_INIT);
	}

	if (argc < 2)
		usage(argv0,-1,0);

	if (i < argc && isdigit(argv[i][0]) && argv[i][0] > '0')
		method = atoi(&argv[i++][0]), methods = NULL;

	for ( ; i < argc && argv[i][0] == '-'; i++)
	{
		const char *p = argv[i];

		if (strcmp(p,"--") == 0)
		{
			i++;
			break;
		}
		else if (isdigit(p[1]))
			method = atoi(p+1), methods = NULL;
		else if (strncmp(p,"-m",2) == 0)
		{
			if (isdigit(p[2]))
				method = atoi(p+2), methods = NULL;
			else if (strcmp(p,"-mall") == 0)
				methods = all_methods;
			else if (strcmp(p,"-mavail") == 0)
				methods = avail_methods;
			else if (strcmp(p,"-mbench") == 0)
				methods = benchmark_methods;
			else if (strcmp(p,"-mx1") == 0)
				methods = x1_methods;
			else if (strcmp(p,"-mx99") == 0)
				methods = x99_methods;
			else if (strcmp(p,"-mx999") == 0)
				methods = x999_methods;
			else if (strcmp(p,"-mzlib") == 0)
				methods = zlib_methods;
#if defined(MFX)
#  include "contrib/t_opt_m.ch"
#endif
			else
				usage(argv0,-1,1);
		}
		else if (strcmp(p,"-A") == 0)
			use_asm_decompressor = 1;
		else if (strncmp(p,"--adler",7) == 0)
			compute_adler32 = 1;
		else if (strncmp(p,"-b",2) == 0)
		{
			if (isdigit(p[2]))
				block_size = atoi(p+2);
			else
				block_size = MAX_BLOCK_SIZE;
		}
		else if (strncmp(p,"-c",2) == 0 && isdigit(p[2]))
			c_loops = atoi(p+2);
#if defined(HAVE_ZLIB_H)
		else if (strncmp(p,"--crc",5) == 0)
			compute_crc32 = 1;
#endif
		else if (strncmp(p,"-d",2) == 0 && isdigit(p[2]))
			d_loops = atoi(p+2);
		else if (strcmp(p,"-h") == 0 || strcmp(p,"-?") == 0)
			usage(argv0,EXIT_OK,0);
		else if (strcmp(p,"--help") == 0)
			usage(argv0,EXIT_OK,0);
		else if (strcmp(p,"-L") == 0 || strcmp(p,"--license") == 0)
			license();
		else if (strncmp(p,"-n",2) == 0 && isdigit(p[2]))
			c_loops = atoi(p+2), d_loops = c_loops;
		else if (strcmp(p,"-O") == 0)
			optimize_compressed_data = 1;
		else if (strcmp(p,"-q") == 0)
			verbose = 1;
		else if (strcmp(p,"-Q") == 0)
			verbose = 0;
		else if (strncmp(p,"-s",2) == 0 && i + 1 < argc)
		{
			if (isdigit(p[2]))
				c_loops = atoi(p+2), d_loops = c_loops;
			corpus_path = argv[++i];
		}
		else if (strcmp(p,"-S") == 0)
			use_safe_decompressor = 1;
		else if (strcmp(p,"-V") == 0 || strcmp(p,"--version") == 0)
			exit(EXIT_OK);
		else if (strcmp(p,"-@") == 0)
			read_from_stdin = 1;
		else
		{
			printf("%s: invalid option '%s'\n\n",argv0,p);
			exit(EXIT_USAGE);
		}
	}

	if (methods != NULL && read_from_stdin)
	{
		printf("%s: cannot use multiple methods and '-@'\n", argv0);
		exit(EXIT_USAGE);
	}

	if (block_size < 16)
		block_size = 16;
	if (block_size > MAX_BLOCK_SIZE)
		block_size = MAX_BLOCK_SIZE;

	t_total = time(NULL);
	ii = i;
	do {
		i = ii;
		if (i >= argc && corpus_path == NULL && !read_from_stdin)
			usage(argv0,-1,0);
		if (methods != NULL && j == 0 && verbose >= 1)
			printf("%lu block-size\n\n", (long) block_size);

		if (methods == avail_methods)
		{
			lzo_bool found = 0;
			while (!found && a_method <= M_LAST_COMPRESSOR)
			{
				method = a_method++;
				found = (info(method,NULL) != NULL);
			}
			if (!found)
				break;
		}
		else if (methods != NULL)
		{
			while ((method = *methods++) < 0)
				;
			if (method == 0)
				break;
			if (!info(method,NULL))
				info(method,stdout);
		}
		else
		{
			if (!info(method,NULL))
				info(method,stdout);
			if (verbose >= 1)
			{
				info(method,stdout);
				printf(" algorithm   %lu block-size\n\n", (long) block_size);
			}
		}

		if (corpus_path != NULL)
			r = do_corpus(method,corpus_path,c_loops,d_loops);
		else
		{
			for ( ; i < argc && r == EXIT_OK; i++)
				r = do_file(method,argv[i],c_loops,d_loops,NULL);
			if (read_from_stdin)
			{
				char buf[512], *p;

				while (r == EXIT_OK && fgets(buf,sizeof(buf)-1,stdin) != NULL)
				{
					buf[sizeof(buf)-1] = 0;
					p = buf + strlen(buf);
					if (p > buf)
						while (--p > buf && isspace(((unsigned char) *p)))
							*p = 0;
					p = buf;
					while (*p && isspace(((unsigned char) *p)))
						p++;
					if (*p)
						r = do_file(method,p,c_loops,d_loops,NULL);
				}
				read_from_stdin = 0;
			}
		}
		j++;
	}
	while (r == EXIT_OK && methods != NULL);

	if (r == EXIT_OK && methods != NULL && verbose >= 1)
	{
		t_total = time(NULL) - t_total;
		printf("\nexecution time: %lu seconds\n", (long) t_total);
	}

	return r;
}

/*
vi:ts=4
*/

