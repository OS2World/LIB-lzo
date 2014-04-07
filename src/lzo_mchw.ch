/* lzo_mchw.ch -- matching functions using a window

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




/***********************************************************************
//
************************************************************************/

typedef struct
{
	int init;

	lzo_uint look;			/* bytes in lookahead buffer */

	lzo_uint m_len;
	lzo_uint m_off;

	const lzo_byte *ip;
	const lzo_byte *in;
	const lzo_byte *in_end;
	lzo_byte *out;

	lzo_progress_callback_t cb;

	lzo_uint textsize;		/* text size counter */
	lzo_uint codesize;		/* code size counter */
	lzo_uint printcount;	/* counter for reporting progress every 1K bytes */

	/* some stats */
	unsigned long lit_bytes;
	unsigned long match_bytes;
	unsigned long rep_bytes;
	unsigned long lazy;

#if defined(LZO1B)
	lzo_uint r1_m_len;

	/* some stats */
	unsigned long r1_r, m3_r, m2_m, m3_m;
#endif

#if defined(LZO1C)
	lzo_uint r1_m_len;
	lzo_byte *m3;

	/* some stats */
	unsigned long r1_r, m3_r, m2_m, m3_m;
#endif

#if defined(LZO1F)
	lzo_uint r1_lit;
	lzo_uint r1_m_len;

	/* some stats */
	unsigned long r1_r, m2_m, m3_m;
#endif

#if defined(LZO1X) || defined(LZO1Y)
	lzo_uint r1_lit;
	lzo_uint r1_m_len;

	/* some stats */
	unsigned long m1a_m, m1b_m, m2_m, m3_m, m4_m;
	unsigned long lit1_r, lit2_r, lit3_r;
#endif

#if defined(LZO2A)
	/* some stats */
	unsigned long m1, m2, m3, m4;
#endif

#if defined(LZO2B)
	/* some stats */
	unsigned long m1, m2_3, m2_45,  m2_6;
	unsigned long lit_run, lit_r[16];
#endif
}
LZO_COMPRESS_T;



#if defined(__LZO_ATARI16)
/* the cast is needed to work around a bug in Pure C */
#define getbyte(c) 	((c).ip < (c).in_end ? (unsigned) *((c).ip)++ : (-1))
#else
#define getbyte(c) 	((c).ip < (c).in_end ? *((c).ip)++ : (-1))
#endif

#include "lzo_swd.ch"



/***********************************************************************
//
************************************************************************/

static int
match_init ( LZO_COMPRESS_T *c, lzo_swd_t *s )
{
	s->c = c;

	c->init = 1;
	c->textsize = c->codesize = c->printcount = 0;
	c->lit_bytes = c->match_bytes = c->rep_bytes = 0;
	c->lazy = 0;

	return swd_init(s);
}


/***********************************************************************
//
************************************************************************/

static int
find_match ( LZO_COMPRESS_T *c, lzo_swd_t *s,
			 lzo_uint this_len, lzo_uint skip )
{
	int r;

	if (!c->init)
	{
		assert(this_len == 0);
		assert(skip == 0);
		r = match_init(c,s);
		if (r != LZO_E_OK)
		{
			c->look = 0;
			c->m_len = 0;
			return r;
		}
	}
	else
	{
		assert(this_len > 0);
	}

	if (skip > 0)
	{
		assert(this_len >= skip);
		swd_accept(s, this_len - skip);
		c->textsize += this_len - skip + 1;
	}
	else
	{
		assert(this_len <= 1);
		c->textsize += this_len - skip;
	}

	s->m_len = THRESHOLD;
	swd_findbest(s);
	c->m_len = s->m_len;
	c->m_off = s->m_pos + 1;

	if (s->swd_char < 0)
	{
		c->look = 0;
		c->m_len = 0;
	}
	else
	{
		c->look = s->look + 1;
	}

	if (c->cb && c->textsize > c->printcount)
	{
		(*c->cb)(c->textsize,c->codesize);
		c->printcount += 1024;
	}

	return LZO_E_OK;
}


/*
vi:ts=4
*/

