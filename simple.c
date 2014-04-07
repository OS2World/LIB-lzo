/* simple.c -- very simple test program for the LZO library

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


#include <stdio.h>
#include <stdlib.h>

#include <lzo1b.h>


/*************************************************************************
//
**************************************************************************/

static lzo_byte _wrkmem[LZO1B_MEM_COMPRESS + 16];

static lzo_byte in[128*1024L];
static lzo_byte out[8*1024];


int main(int argc, char *argv[])
{
	int r;
	lzo_uint in_len, out_len;
	lzo_byte *wrkmem;

	if (argc < 0 || argv == NULL)	/* avoid warning about unused args */
		return 0;

	if (lzo_init() != LZO_E_OK)
	{
		printf("lzo_init() failed !!!\n");
		return 4;
	}

/* setup work memory */
	wrkmem = (lzo_byte *) LZO_ALIGN(_wrkmem,16);

/* prepare input block */
	in_len = sizeof(in);
	lzo_memset(in,0,in_len);
	out_len = sizeof(out);

/* test that the 'huge' memory model works */
#if (UINT_MAX < 0xffffffffL)			/* 16 bit */
	if (in_len == 128*1024L)
	{
		lzo_uint32 adler;

		adler = lzo_adler32(0, NULL, 0);
		adler = lzo_adler32(adler, in, in_len);
		if (adler != 0x001e0001L)
		{
			printf("memory model problem !!! (0x%08lx)\n", (long) adler);
			return 3;
		}
	}
#endif

/* compress */
	r = lzo1b_1_compress(in,in_len,out,&out_len,wrkmem);
	if (r == LZO_E_OK)
		printf("compressed %lu bytes into %lu bytes\n",
			(long) in_len, (long) out_len);
	else
	{
		printf("compression failed: %d\n", r);
		return 2;
	}

/* decompress */
	r = lzo1b_decompress(out,out_len,in,&in_len,NULL);
	if (r == LZO_E_OK)
		printf("decompressed %lu bytes into %lu bytes\n",
			(long) out_len, (long) in_len);
	else
	{
		printf("decompression failed: %d\n", r);
		return 1;
	}

	return 0;
}

/*
vi:ts=4
*/

