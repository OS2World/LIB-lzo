/* lzo1c_d.c -- standalone LZO1C decompressor

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
#define NDEBUG
#include <assert.h>


/***********************************************************************
// decompress a block of data.
************************************************************************/

LZO_PUBLIC(int)
lzo1c_decompress     ( const lzo_byte *in , lzo_uint  in_len,
                             lzo_byte *out, lzo_uint *out_len,
                             lzo_voidp wrkmem )
{
	register lzo_byte *op;
	register const lzo_byte *ip;
	register lzo_uint t;
	register const lzo_byte *m_pos;
	const lzo_byte * const ip_end = in + in_len;

	op = out;
	ip = in;

	while (1)
	{
		t = *ip++;	

		if (t < 32)	
		{
			if (t == 0)
			{
				t = *ip++;
				if (t >= 280 - 32)		
				{
					t -= 280 - 32;
					t = (t == 0) ? 280 : 256u << ((unsigned) t);
					do *op++ = *ip++; while (--t > 0);
					continue;
				}
				t += 32;
			}

literal1:
			do *op++ = *ip++; while (--t > 0);

			while ((t = *ip++) < 32)
			{
				m_pos = op - 1;
				m_pos -= t | (((lzo_uint) *ip++) << 5);
				*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos++;
				*op++ = *ip++;
			}
		}

		if (t >= 64)				/* a M2 match */
		{
			m_pos = op - 1;
			m_pos -= (t & 31) | (((lzo_uint) *ip++) << 5);
			t = (t >> 5) - 1;
			*op++ = *m_pos++; *op++ = *m_pos++;
			do *op++ = *m_pos++; while (--t > 0);
		}
		else						/* a M3 or M4 match */
		{
			t &= 31;
			if (t == 0)	
			{
				t = 31;
				while (*ip == 0)
					t += 255, ip++;
				t += *ip++;
			}
			m_pos = op;
			m_pos -= *ip++ & 63;
			m_pos -= (lzo_uint)(*ip++) << 6;
			if (m_pos == op)
				goto eof_found;
			*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos++;
			do *op++ = *m_pos++; while (--t > 0);

			t = ip[-2] >> 6;
			if (t)
				goto literal1;
		}
	}

	/* ip == ip_end and no EOF code was found */
	*out_len = op - out;
	return (ip == ip_end ? LZO_E_EOF_NOT_FOUND : LZO_E_ERROR);

eof_found:
	assert(t == 1);
	*out_len = op - out;
	return (ip == ip_end ? LZO_E_OK : LZO_E_ERROR);
}


/*
vi:ts=4
*/

