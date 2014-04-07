/* lzo1f_c.c -- standalone LZO1F-1 compressor

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


#include <lzo1f.h>
#define NDEBUG
#include <assert.h>
#include "lzo_conf.h"


/***********************************************************************
//
************************************************************************/

#define M2_MAX_OFFSET	0x0800
#define M3_MAX_OFFSET	0x3fff
#define M3_MARKER		224


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


/***********************************************************************
// compress a block of data.
************************************************************************/

static
int do_compress          ( const lzo_byte *in , lzo_uint  in_len,
                                 lzo_byte *out, lzo_uint *out_len,
                                 lzo_voidp wrkmem )
{
#if 1 && defined(__GNUC__) && defined(__i386__)
	register const lzo_byte *ip __asm__("%esi");
#else
	register const lzo_byte *ip;
#endif
	lzo_uint32 dv;
	lzo_byte *op;
	const lzo_byte * const in_end = in + in_len;
	const lzo_byte * const ip_end = in + in_len - 9;
	const lzo_byte *ii;
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
#if 1 && defined(__GNUC__) && defined(__i386__)
		register const lzo_byte *m_pos __asm__("%edi");
#else
		register const lzo_byte *m_pos;
#endif
		lzo_uint m_len;
		lzo_ptrdiff_t m_off;
		lzo_uint lit;

		{
			lzo_uint dindex = DINDEX(dv,ip);
			m_pos = dict[dindex];
			UPDATE_I(dict,cycle,dindex,ip);
		}


		if (LZO_CHECK_MPOS_NON_DET(m_pos,m_off,in,ip,M3_MAX_OFFSET))
		{
		}
#if defined(LZO_UNALIGNED_OK_2)
		else if (* (lzo_ushortp) m_pos != * (lzo_ushortp) ip)
#else
		else if (m_pos[0] != ip[0] || m_pos[1] != ip[1]) 
#endif
		{
		}
		else
		{
			if (m_pos[2] == ip[2])
			{
				m_pos += 3;
				if (m_off <= M2_MAX_OFFSET)
					goto match;
#if 1
				if (ip - ii <= 3)
					goto match;
#else
				if (ip - ii == 3)		/* better compression, but slower */
					goto match;
#endif
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
		lit = ip - ii;
		if (lit > 0)
		{
			register lzo_uint t = lit;

			if (t < 4 && op > out)
				op[-2] |= LZO_BYTE(t);
			else if (t <= 31)
				*op++ = LZO_BYTE(t);
			else
			{
				register lzo_uint tt = t - 31;

				*op++ = 0;
				while (tt > 255)
				{
					tt -= 255;
					*op++ = 0;
				}
				assert(tt > 0);
				*op++ = LZO_BYTE(tt);
			}
			do *op++ = *ii++; while (--t > 0);
		}
		assert(ii == ip);


		/* code the match */
		ip += 3;
		if (*m_pos++ != *ip++ || *m_pos++ != *ip++ || *m_pos++ != *ip++ ||
		    *m_pos++ != *ip++ || *m_pos++ != *ip++ || *m_pos++ != *ip++)
		{
			--ip;
			m_len = ip - ii;
			assert(m_len >= 3); assert(m_len <= 8);

			if (m_off <= M2_MAX_OFFSET)
			{
				m_off -= 1;
				*op++ = LZO_BYTE(((m_len - 2) << 5) | ((m_off & 7) << 2));
				*op++ = LZO_BYTE(m_off >> 3);
			}
			else if (m_len == 3 && m_off <= 2*M2_MAX_OFFSET && lit > 0)
			{
				m_off -= 1;
				/* m_off -= M2_MAX_OFFSET; */
				*op++ = LZO_BYTE(((m_off & 7) << 2));
				*op++ = LZO_BYTE(m_off >> 3);
			}
			else
			{
				*op++ = LZO_BYTE(M3_MARKER | (m_len - 2));
				*op++ = LZO_BYTE((m_off & 63) << 2);
				*op++ = LZO_BYTE(m_off >> 6);
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
			assert(m_len >= 3);

			if (m_len <= 33)
				*op++ = LZO_BYTE(M3_MARKER | (m_len - 2));
			else
			{
				m_len -= 33;
				*op++ = M3_MARKER | 0;
				while (m_len > 255)
				{
					m_len -= 255;
					*op++ = 0;
				}
				assert(m_len > 0);
				*op++ = LZO_BYTE(m_len);
			}
			*op++ = LZO_BYTE((m_off & 63) << 2);
			*op++ = LZO_BYTE(m_off >> 6);
		}

		ii = ip;
		if (ip >= ip_end)
			break;
		DVAL_FIRST(dv,ip);
	}


	/* store final literal run */
	if (in_end - ii > 0)
	{
		register lzo_uint t = in_end - ii;

		if (t < 4 && op > out)
			op[-2] |= LZO_BYTE(t);
		else if (t <= 31)
			*op++ = LZO_BYTE(t);
		else
		{
			register lzo_uint tt = t - 31;

			*op++ = 0;
			while (tt > 255)
			{
				tt -= 255;
				*op++ = 0;
			}
			assert(tt > 0);
			*op++ = LZO_BYTE(tt);
		}
		do *op++ = *ii++; while (--t > 0);
	}

	*out_len = op - out;
	return LZO_E_OK;
}


/***********************************************************************
// public entry point
************************************************************************/

LZO_PUBLIC(int)
lzo1f_1_compress ( const lzo_byte *in , lzo_uint  in_len,
                         lzo_byte *out, lzo_uint *out_len,
                         lzo_voidp wrkmem )
{
	lzo_byte *op = out;
	int r = LZO_E_OK;

	if (in_len <= 0)
		*out_len = 0;
	else if (in_len <= 10)
	{
		*op++ = LZO_BYTE(in_len);
		do *op++ = *in++; while (--in_len > 0);
		*out_len = op - out;
	}
	else
		r = do_compress(in,in_len,out,out_len,wrkmem);

	if (r == LZO_E_OK)
	{
		op = out + *out_len;
		*op++ = M3_MARKER | 1;
		*op++ = 0;
		*op++ = 0;
		*out_len += 3;
	}

	return r;
}


/*
vi:ts=4
*/

