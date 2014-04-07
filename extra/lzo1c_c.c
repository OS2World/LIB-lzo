/* lzo1c_c.c -- standalone LZO1C-1 compressor

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


#include <lzo1c.h>
#include "lzo_conf.h"


/***********************************************************************
//
************************************************************************/

#define M2_MAX_OFFSET	0x2000
#define M3_MAX_OFFSET	0x3fff


#define _DV2(p,shift1,shift2) \
		(((( (lzo_uint32)(p[0]) << shift1) ^ p[1]) << shift2) ^ p[2])
#define DVAL_NEXT(dv,p) \
		dv ^= (lzo_uint32)(p[-1]) << (2*5); dv = (((dv) << 5) ^ p[2])
#define _DV(p,shift) 		_DV2(p,shift,shift)
#define DVAL_FIRST(dv,p)	dv = _DV((p),5)
#define _DINDEX(dv,p)		((40799u * (dv)) >> 5)
#define DINDEX(dv,p)		(((_DINDEX(dv,p)) & 0x3fff) << 0)
#define UPDATE_D(dict,cycle,dv,p)		dict[ DINDEX(dv,p) ] = (p)
#define UPDATE_I(dict,cycle,index,p)	dict[index] = (p)


static lzo_byte *
STORE_RUN ( lzo_byte * const oo, const lzo_byte * const ii, lzo_uint r_len );


/***********************************************************************
// compress a block of data.
************************************************************************/

static
int do_compress          ( const lzo_byte *in , lzo_uint  in_len,
                                 lzo_byte *out, lzo_uint *out_len,
                                 lzo_voidp wrkmem )
{
#if defined(__GNUC__) && defined(__i386__)
	register const lzo_byte *ip __asm__("%esi");
#else
	register const lzo_byte *ip;
#endif
	lzo_uint32 dv;
	lzo_byte *op;
	const lzo_byte * const in_end = in + in_len;
	const lzo_byte * const ip_end = in + in_len - 9;
	const lzo_byte *ii;
	const lzo_byte *r1 = ip_end;
	lzo_byte *m3 = out + 1;
	const lzo_bytepp const dict = (const lzo_bytepp) wrkmem;

	op = out;
	ip = in;
	ii = ip;

	DVAL_FIRST(dv,ip);
	UPDATE_D(dict,cycle,dv,ip);
	ip++;
	DVAL_NEXT(dv,ip);


	while (1)
	{
		const lzo_byte *m_pos;
		lzo_uint m_len;
		lzo_ptrdiff_t m_off;

		{
			lzo_uint dindex = DINDEX(dv,ip);
			m_pos = dict[dindex];
			UPDATE_I(dict,cycle,dindex,ip);
		}


		if (LZO_CHECK_MPOS_NON_DET(m_pos,m_off,in,ip,M3_MAX_OFFSET) ||
		    (*m_pos++ != ip[0] || *m_pos++ != ip[1]))
		{
		}
		else
		{
			if (*m_pos++ == ip[2])
			{
				if (m_off <= M2_MAX_OFFSET)
					goto match;
				if (*m_pos == ip[3])
					goto match;
			}
		}


	/* a literal */
		++ip;
		if (ip >= ip_end)
			break;
		DVAL_NEXT(dv,ip);
		continue;


	/* a match */
match:

		/* store current literal run */
		if (ip - ii > 0)
		{
			lzo_uint t = ip - ii;

			if (ip == r1)
			{
				op[-2] &= 31;
				*op++ = *ii++;
				r1 = ip + 4;
			}
			else if (t < 32)
			{
				if (t < 4 && op == m3)
					m3[-2] |= LZO_BYTE(t << 6);
				else
					*op++ = LZO_BYTE(t);
				do *op++ = *ii++; while (--t > 0);
				r1 = ip + 4;
			}
			else if (t < 280)
			{
				*op++ = 0; *op++ = LZO_BYTE(t - 32);
				do *op++ = *ii++; while (--t > 0);
				r1 = ip + 4;
			}
			else
			{
				op = STORE_RUN(op,ii,t);
				ii = ip;
			}
		}


		/* code the match */
		ip += 3;
		if (*m_pos++ != *ip++ || *m_pos++ != *ip++ || *m_pos++ != *ip++ ||
		    *m_pos++ != *ip++ || *m_pos++ != *ip++ || *m_pos++ != *ip++)
		{
			--ip;
			m_len = ip - ii;

			if (m_off <= M2_MAX_OFFSET)
			{
				m_off -= 1;
				*op++ = LZO_BYTE(((m_len - 1) << 5) | (m_off & 31));
				*op++ = LZO_BYTE(m_off >> 5);
			}
			else
			{
				*op++ = LZO_BYTE(32 | (m_len - 3));
				*op++ = LZO_BYTE(m_off & 63);
				*op++ = LZO_BYTE(m_off >> 6);
				m3 = op;
			}
		}
		else
		{
			{
				const lzo_byte *end;
				end = in_end;
				while (ip < end && *m_pos == *ip)
					m_pos++, ip++;
				m_len = (ip - ii);
			}

			if (m_len <= 34)
				*op++ = LZO_BYTE(32 | (m_len - 3));
			else
			{
				m_len -= 34;
				*op++ = 32;
				while (m_len > 255)
				{
					m_len -= 255;
					*op++ = 0;
				}
				*op++ = LZO_BYTE(m_len);
			}
			*op++ = LZO_BYTE(m_off & 63);
			*op++ = LZO_BYTE(m_off >> 6);
			m3 = op;
		}

		ii = ip;
		if (ip >= ip_end)
			break;
		DVAL_FIRST(dv,ip);
	}


	if (in_end - ii > 0)
	{
		lzo_uint t = in_end - ii;
		op = STORE_RUN(op,ii,t);
	}

	*out_len = op - out;
	return LZO_E_OK;
}


/***********************************************************************
// public entry point
************************************************************************/

LZO_PUBLIC(int)
lzo1c_1_compress ( const lzo_byte *in , lzo_uint  in_len,
                         lzo_byte *out, lzo_uint *out_len,
                         lzo_voidp wrkmem )
{
	int r;

	if (in_len <= 0)
	{
		*out_len = 0;
		r = LZO_E_OK;
	}
	else if (in_len <= 10)
	{
		*out_len = STORE_RUN(out,in,in_len) - out;
		r = (*out_len > in_len) ? LZO_E_OK : LZO_E_ERROR;
	}
	else
		r = do_compress(in,in_len,out,out_len,wrkmem);

	if (r == LZO_E_OK)
	{
		lzo_byte *op = out + *out_len;
		*op++ = 32 | 1;
		*op++ = 0;
		*op++ = 0;
		*out_len += 3;
	}

	return r;
}


/***********************************************************************
// store a run
************************************************************************/

static lzo_byte *
STORE_RUN ( lzo_byte * const oo, const lzo_byte * const ii, lzo_uint r_len )
{
	register lzo_byte *op;
	register const lzo_byte *ip;
	register lzo_uint t;

	op = oo;
	ip = ii;

	if (r_len >= 512)
	{
		unsigned r_bits = 6;
		lzo_uint tt = 32768u;

		while (r_len >= (t = tt))
		{
			r_len -= t;
			*op++ = 0; *op++ = 255;
			do *op++ = *ip++; while (--t > 0);
		}
		tt >>= 1;
		do {
			if (r_len >= (t = tt))
			{
				r_len -= t;
				*op++ = 0; *op++ = LZO_BYTE(248 + r_bits);
				do *op++ = *ip++; while (--t > 0);
			}
			tt >>= 1;
		} while (--r_bits > 0);
	}

	while (r_len >= (t = 280))
	{
		r_len -= t;
		*op++ = 0; *op++ = 248;
		do *op++ = *ip++; while (--t > 0);
	}

	t = r_len;
	if (t >= 32)
	{
		*op++ = 0; *op++ = LZO_BYTE(t - 32);
		do *op++ = *ip++; while (--t > 0);
	}
	else if (t > 0)
	{
		*op++ = LZO_BYTE(t);
		do *op++ = *ip++; while (--t > 0);
	}

	return op;
}



/*
vi:ts=4
*/

