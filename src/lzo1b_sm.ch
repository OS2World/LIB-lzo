/* lzo1b_sm.ch -- implementation of the LZO1B compression algorithm

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



/***********************************************************************
// search for a match
************************************************************************/

#if (DD_BITS == 0)

		/* search ip in the dictionary */
		{
			lzo_uint dindex;

			dindex = DINDEX(dv,ip);
			m_pos = dict[dindex];
			UPDATE_I(dict,cycle,dindex,ip);

#if !defined(NDEBUG)
			DVAL_ASSERT(dv,ip);
#if defined(LZO_DETERMINISTIC)
			assert(m_pos == NULL || m_pos >= in);
			assert(m_pos == NULL || m_pos < ip);
#endif
			m_pos_sav = m_pos;
#endif
		}



#else /* (DD_BITS == 0) */



		/* search ip in the deepened dictionary */
		{
			const lzo_bytepp d = &dict [ DINDEX(dv,ip) ];
			const lzo_byte *ip_sav;
			unsigned j = DD_SIZE;
			lzo_uint x_len;
			lzo_ptrdiff_t x_off;

			DVAL_ASSERT(dv,ip);

			ip_sav = ip;
			m_len = 0;
			do {
				assert(ip == ip_sav);
				m_pos = *d;
#if defined(LZO_DETERMINISTIC)
				assert(m_pos == NULL || m_pos >= in);
				assert(m_pos == NULL || m_pos < ip);
#endif

				if (LZO_CHECK_MPOS(m_pos,x_off,in,ip,MAX_OFFSET))
#if (CLEVEL == 9)
					*d = ip;
#else
					((void)(0));
#endif
				else if (m_pos[m_len] != ip[m_len])
					((void)(0));
				else if (*m_pos++ == *ip++ && *m_pos++ == *ip++ && *m_pos++ == *ip++)
				{
					/* a match */
					if (MATCH_M2)
					{
						x_len = (ip - 1) - ip_sav;
						if (x_len > m_len)
						{
							m_len = x_len;
							m_off = x_off;
							assert((m_pos_sav = *d) != NULL);
						}
#if (CLEVEL == 9)
						/* try to find a closer match */
						else if (x_len == m_len && x_off < m_off)
						{
							m_off = x_off;
							assert((m_pos_sav = *d) != NULL);
						}
#endif
					}
					else
					{
						assert((ip - ip_sav) == M2_MAX_LEN + 1);
#if (CLEVEL == 9)
#if defined(MATCH_IP_END)
						{
							const lzo_byte *end;
							end = MATCH_IP_END;
							while (ip < end  &&  *m_pos == *ip)
								m_pos++, ip++;
							assert(ip <= in_end);
							x_len = ip - ip_sav;
						}
						if (x_len > m_len)
						{
							m_len = x_len;
							m_off = x_off;
							assert((m_pos_sav = *d) != NULL);
						}
						else if (x_len == m_len && x_off < m_off)
						{
							m_off = x_off;
							assert((m_pos_sav = *d) != NULL);
						}
#else
						/* try to find a closer match */
						if (m_len < M2_MAX_LEN + 1 || x_off < m_off)
						{
							m_len = M2_MAX_LEN + 1;
							m_off = x_off;
							assert((m_pos_sav = *d) != NULL);
						}
#endif
#else
						/* don't search for a longer/closer match */
						m_len = M2_MAX_LEN + 1;
						m_off = x_off;
						assert((m_pos_sav = *d) != NULL);
						ip = ip_sav;
						d -= DD_SIZE - j;
						assert(d == &dict [ DINDEX(dv,ip) ]);
						UPDATE_P(d,cycle,ip);
						goto match;
#endif
					}
					ip = ip_sav;
				}
				else
					ip = ip_sav;
				d++;
			} while (--j > 0);
			assert(ip == ip_sav);

			d -= DD_SIZE;
			assert(d == &dict [ DINDEX(dv,ip) ]);
			UPDATE_P(d,cycle,ip);
		}

#endif /* (DD_BITS == 0) */


/*
vi:ts=4
*/
