/* lzo_swd.c -- sliding window dictionary 

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



/***********************************************************************
// This file is inspired by the sliding dictionary routines used
// in HA, a general purpose file archiver.
************************************************************************/

#if (LZO_UINT_MAX < 0xffffffffL)
#  error LZO_UINT_MAX
#endif

/* unsigned type for dictionary access - don't waste memory here */
#if (N + F + F < USHRT_MAX)
   typedef unsigned short	swd_uint;
#  define SWD_UINT_MAX		USHRT_MAX
#else
   typedef lzo_uint			swd_uint;
#  define SWD_UINT_MAX		LZO_UINT_MAX
#endif
#define SWD_UINT(x)			((swd_uint)(x))


#ifndef HSIZE
#define HSIZE		16384
#endif

#if 1
#define HASH3(b,p) \
	(((40799u*(((((lzo_uint32)b[p]<<5)^b[p+1])<<5)^b[p+2]))>>5) & (HSIZE-1))
#else
#define HASH3(b,p) \
	(((40799u*(((((lzo_uint32)b[p+2]<<5)^b[p+1])<<5)^b[p]))>>5) & (HSIZE-1))
#endif

#if (THRESHOLD == 1) || defined(NEED_HASH2)
#  define HASH2(b,p)	(b[p] | ((unsigned)b[p+1]<<8))
#  define NIL2			SWD_UINT_MAX
#endif

#ifndef SWD_MAXCNT
#define SWD_MAXCNT	1024
#endif


typedef struct
{
/* public: */
	lzo_uint m_len;
	lzo_uint m_pos;
	lzo_uint look;
	int swd_char;
	LZO_COMPRESS_T *c;

/* private: */
	lzo_uint b_size;
	lzo_uint hash_count;
	lzo_uint bbf, inptr;

	unsigned char b [ N + F + F - 1];

	swd_uint ll [ N + F ];
	swd_uint best [ N + F ];
	swd_uint ccnt [ HSIZE ];
	swd_uint cr3 [ HSIZE ];
#ifdef HASH2
	swd_uint cr2 [ 65536L ];
#endif
}
lzo_swd_t;



#if 0 && defined(LZO_UNALIGNED_OK_2)
#  if !defined(SWD_UNALIGNED_OK_2)
#    define SWD_UNALIGNED_OK_2
#  endif
#endif

#if 0 && defined(LZO_UNALIGNED_OK_4)
#  if !defined(SWD_UNALIGNED_OK_4)
#    define SWD_UNALIGNED_OK_4
#  endif
#endif


/***********************************************************************
//
************************************************************************/

static
int swd_init(lzo_swd_t *s)
{
	lzo_uint i;
	int c;

	s->b_size = N + F;
	if (s->b_size + F >= SWD_UINT_MAX)
		return LZO_E_ERROR;

	s->hash_count = N;
	for (i = 0; i < HSIZE; i++)
		s->ccnt[i] = 0;
#ifdef HASH2
	for (i = 0; i < 65536L; i++)
		s->cr2[i] = NIL2;
#endif
	s->bbf = s->look = s->inptr = 0;
	while (s->look < F)
	{
		if ((c = getbyte(*(s->c))) < 0)
			break;
		s->b[s->inptr++] = LZO_BYTE(c);
		s->look++;
	}
	s->m_len = THRESHOLD;
	return LZO_E_OK;
}


static 
void swd_getbyte(lzo_swd_t *s)
{
	int c;

	if (++s->bbf == s->b_size)
		s->bbf = 0;

	if ((c = getbyte(*(s->c))) < 0)
	{
		if (s->look > 0)
			--s->look;
		if (++s->inptr == s->b_size)
			s->inptr = 0;
	}
	else if (s->inptr < F - 1)
	{
		s->b[s->inptr + s->b_size] = s->b[s->inptr] = LZO_BYTE(c);
		++s->inptr;
	}
	else
	{
		s->b[s->inptr] = LZO_BYTE(c);
		if (++s->inptr == s->b_size)
			s->inptr = 0;
	}
}


/***********************************************************************
// remove inptr from hash chains
************************************************************************/

static __inline__
void _swd_remove_hash(lzo_swd_t *s)
{
	if (s->hash_count == 0)
	{
		lzo_uint i;

		i = HASH3(s->b,s->inptr);
		assert(s->ccnt[i] > 0);
		--s->ccnt[i];

#ifdef HASH2
		i = HASH2(s->b,s->inptr);
		assert(s->cr2[i] != NIL2);
		if (s->cr2[i] == s->inptr)
			s->cr2[i] = NIL2;
#endif
	}
	else
		--s->hash_count;
}


/***********************************************************************
//
************************************************************************/

static
void swd_accept(lzo_swd_t *s, lzo_uint j)
{
	while (j--)
	{
		lzo_uint i;

		_swd_remove_hash(s);

		/* add bbf into HASH3 */
		i = HASH3(s->b,s->bbf);
		s->ll[s->bbf] = s->cr3[i];
		s->cr3[i] = SWD_UINT(s->bbf);
		s->best[s->bbf] = F + 1;
		s->ccnt[i]++;
		assert(s->ccnt[i] < SWD_UINT_MAX);

#ifdef HASH2
		/* add bbf into HASH2 */
		i = HASH2(s->b,s->bbf);
		s->cr2[i] = SWD_UINT(s->bbf);
#endif

		swd_getbyte(s);
	}

	s->m_len = THRESHOLD;
}


/***********************************************************************
//
************************************************************************/

static
lzo_uint _swd_findbest(lzo_swd_t *s, lzo_uint cnt, lzo_uint ptr)
{
	register unsigned char *b = s->b;
	register unsigned char *pbbf = b + s->bbf;
	lzo_uint m_len = s->m_len;
	unsigned char * const pxx = pbbf + s->look;
	lzo_uint i;
#if defined(SWD_UNALIGNED_OK_2)
	unsigned short ref2;
#else
	unsigned char ref1;
#endif


#ifdef HASH2
	i = HASH2(b,s->bbf);
	i = s->cr2[i];
	if (i == NIL2)
		return m_len;
	assert(memcmp(&b[s->bbf],&b[i],2) == 0);
	if (m_len <= 2)
	{
		m_len = 2;
		s->m_pos = i;
		if (m_len == s->look)
			return m_len;
	}
#endif


	if (cnt == 0)
		return m_len;


#if defined(SWD_UNALIGNED_OK_2)
	ref2 = * (lzo_ushortp) &pbbf[m_len - 1];
#else
	ref1 = pbbf[m_len - 1];
#endif

	do {
		assert(m_len < s->look);

#if defined(SWD_UNALIGNED_OK_2)
		if (* (lzo_ushortp) &b[ptr + m_len - 1] == ref2 &&
		    * (lzo_ushortp) &b[ptr] == * (lzo_ushortp) &pbbf[0])
#else
		if (b[ptr + m_len - 1] == ref1 && b[ptr + m_len] == pbbf[m_len] &&
		    b[ptr] == pbbf[0] && b[ptr + 1] == pbbf[1])
#endif
		{
			register unsigned char *p1 = pbbf + 3;
			register unsigned char *p2 = b + ptr + 3;
			register unsigned char *px = pxx;

			while (p1 < px && *p1 == *p2)
				p1++, p2++;

			i = p1 - pbbf;
			if (i > m_len)
			{
				s->m_pos = ptr;
				if ((m_len = i) == s->look || s->best[ptr] < i)
					return m_len;
#if defined(SWD_UNALIGNED_OK_2)
				ref2 = * (lzo_ushortp) &pbbf[m_len - 1];
#else
				ref1 = pbbf[m_len - 1];
#endif
#if defined(SWD_UNALIGNED_OK_4)
				if (m_len >= 4)
				{
					--cnt;
					break;
				}
#endif
			}
		}
		ptr = s->ll[ptr];
	} while (--cnt > 0);


#if defined(SWD_UNALIGNED_OK_4)
	if (cnt == 0)
		return m_len;

	assert(m_len >= 4); assert(s->look > 4);
	ptr = s->ll[ptr];
	do {
		assert(m_len < s->look);

#if defined(SWD_UNALIGNED_OK_2)
		if (* (lzo_ushortp) &b[ptr + m_len - 1] == ref2 &&
		    * (lzo_uint32p) &b[ptr] == * (lzo_uint32p) &pbbf[0])
#else
		if (b[ptr + m_len - 1] == ref1 && b[ptr + m_len] == pbbf[m_len] &&
		    * (lzo_uint32p) &b[ptr] == * (lzo_uint32p) &pbbf[0])
#endif
		{
			register unsigned char *p1 = pbbf + 4;
			register unsigned char *p2 = b + ptr + 4;
			register unsigned char *px = pxx - 4;

			while (p1 <= px && * (lzo_uint32p) p1 == * (lzo_uint32p) p2)
				p1 += 4, p2 += 4;

			px += 4;
			while (p1 < px && *p1 == *p2)
				p1++, p2++;

			i = p1 - pbbf;
			if (i > m_len)
			{
				s->m_pos = ptr;
				if ((m_len = i) == s->look || s->best[ptr] < i)
					return m_len;
#if defined(SWD_UNALIGNED_OK_2)
				ref2 = * (lzo_ushortp) &pbbf[m_len - 1];
#else
				ref1 = pbbf[m_len - 1];
#endif
			}
		}
		ptr = s->ll[ptr];
	} while (--cnt > 0);
#endif

	return m_len;
}


/***********************************************************************
//
************************************************************************/

static
void swd_findbest(lzo_swd_t *s)
{
	lzo_uint i;
	lzo_uint cnt, ptr, start_len;
	const lzo_uint bbf = s->bbf;

	i = HASH3(s->b,bbf);
	if ((cnt = s->ccnt[i]++) > SWD_MAXCNT)
		cnt = SWD_MAXCNT;
	assert(s->ccnt[i] < SWD_UINT_MAX);
	ptr = s->ll[bbf] = s->cr3[i];
	/* add bbf into HASH3 */
	s->cr3[i] = SWD_UINT(bbf);

	s->swd_char = s->b[bbf];
	s->m_pos = N + 1;
	if ((start_len = s->m_len) >= s->look)
	{
		if (s->look == 0)
			s->swd_char = -1;
		s->best[bbf] = F + 1;
	}
	else
	{
		s->m_len = _swd_findbest(s,cnt,ptr);
		s->best[bbf] = SWD_UINT(s->m_len);
		if (s->m_len > start_len)
		{
			if (s->m_pos < bbf)
				s->m_pos = bbf - s->m_pos - 1;
			else
				s->m_pos = s->b_size - 1 - s->m_pos + bbf;
		}
	}

	_swd_remove_hash(s);

#ifdef HASH2
	/* add bbf into HASH2 */
	i = HASH2(s->b,bbf);
	s->cr2[i] = SWD_UINT(bbf);
#endif
		
	swd_getbyte(s);
}


/*
vi:ts=4
*/

