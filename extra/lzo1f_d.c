/* lzo1f_d.c -- standalone LZO1F decompressor

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

#if 1
#  define TEST_IP				1
#else
#  define TEST_IP				(ip < ip_end)
#endif


/***********************************************************************
// decompress a block of data.
************************************************************************/

LZO_PUBLIC(int)
lzo1f_decompress     ( const lzo_byte *in , lzo_uint  in_len,
                             lzo_byte *out, lzo_uint *out_len,
                             lzo_voidp wrkmem )
{
#if 0 && defined(__GNUC__) && defined(__i386__)
	register lzo_byte *op __asm__("%edi");
	register const lzo_byte *ip __asm__("%esi");
	register lzo_uint t __asm__("%ecx");
	register const lzo_byte *m_pos __asm__("%ebx");
#else
	register lzo_byte *op;
	register const lzo_byte *ip;
	register lzo_uint t;
	register const lzo_byte *m_pos;
#endif
	const lzo_byte * const ip_end = in + in_len;

	*out_len = 0;

	op = out;
	ip = in;

	while (TEST_IP)
	{
		t = *ip++;	
		if (t > 31)
			goto match;

		/* a literal run */
		if (t == 0)
		{
			t = 31;
			while (*ip == 0)
				t += 255, ip++;
			t += *ip++;
		}
		/* copy literals */
		assert(t > 0);
		do *op++ = *ip++; while (--t > 0);
		t = *ip++;

		while (TEST_IP)
		{
			/* handle matches */
			if (t < 32)
			{
				/* a R1 match */
				m_pos = op - 1 - 0x800;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos++;
			}
			else
			{
match:
				if (t < 224)
				{
					m_pos = op - 1;
					m_pos -= (t >> 2) & 7;
					m_pos -= *ip++ << 3;
					t >>= 5;
				}
				else
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
					m_pos -= *ip++ >> 2;
					m_pos -= *ip++ << 6;
					if (m_pos == op)
						goto eof_found;
				}
				assert(t > 0);
				*op++ = *m_pos++; *op++ = *m_pos++;
				do *op++ = *m_pos++; while (--t > 0);
			}
			t = ip[-2] & 3;
			if (t == 0)
				break;

			/* copy literals */
			do *op++ = *ip++; while (--t > 0);
			t = *ip++;
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

