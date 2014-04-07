/* lzo1b_tm.ch -- implementation of the LZO1B compression algorithm

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
// test for a potential match 
************************************************************************/


#if (DD_BITS == 0)

	/* test potential match in m_pos:
	 * range must fit, first 3 (M2_MIN_LEN) bytes must match */

		if (LZO_CHECK_MPOS(m_pos,m_off,in,ip,MAX_OFFSET) ||
		    (*m_pos++ != ip[0] || *m_pos++ != ip[1]))
		{
			/* could experiment with a M1 match here */
		}
		else
		{
			if (*m_pos++ == ip[2])
			{
#if (_MAX_OFFSET == _M2_MAX_OFFSET)
				goto match;
#else
				if (m_off <= M2_MAX_OFFSET)
					goto match;
				if (*m_pos == ip[3])
					goto match;
#if 0 && (M3_MIN_LEN == M2_MIN_LEN)
				if (ip == ii)
					goto match;
#endif
#endif
			}
		}


#else /* (DD_BITS == 0) */


	/* test potential match */

		if (m_len > M2_MIN_LEN)
			goto match;
		if (m_len == M2_MIN_LEN)
		{
#if (_MAX_OFFSET == _M2_MAX_OFFSET)
			goto match;
#else
			if (m_off <= M2_MAX_OFFSET)
				goto match;
#if 0 && (M3_MIN_LEN == M2_MIN_LEN)
			if (ip == ii)
				goto match;
#endif
#endif
		}


#endif /* (DD_BITS == 0) */



/*
vi:ts=4
*/
