/* acconfig.h -- autoheader configuration file

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


#ifndef __LZO_CONFIG_AC
#define __LZO_CONFIG_AC

/* $TOP$ */
@TOP@

/* acconfig.h

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* See src/lzo_conf.h, config/config.lzo and configure.in.  */
#undef LZO_ALIGNED_OK_2

/* See src/lzo_conf.h, config/config.lzo and configure.in.  */
#undef LZO_ALIGNED_OK_4

/* See src/lzo_conf.h, config/config.lzo and configure.in.  */
#undef LZO_UNALIGNED_OK_2

/* See src/lzo_conf.h, config/config.lzo and configure.in.  */
#undef LZO_UNALIGNED_OK_4

/* Define for machines where ".align 4" means align to a 4 byte boundary.  */
#undef MFX_ASM_ALIGN_BYTES

/* Define for machines where ".align 4" means align to 2**4 boundary.  */
#undef MFX_ASM_ALIGN_PTWO

/* See src/lzo_conf.h.  */
#undef MFX_BYTE_ORDER

/* Define if your memcmp is broken.  */
#undef MFX_MEMCMP_BROKEN

/* Define to `long' if <stddef.h> doesn't define.  */
#undef ptrdiff_t

/* The number of bytes in a ptrdiff_t.  */
#undef SIZEOF_PTRDIFF_T

/* The number of bytes in a size_t.  */
#undef SIZEOF_SIZE_T

/* Define to `int' if <stddef.h> doesn't define.  */
#undef wchar_t

/* The following comments are here to suppress an error message
   from autoheader. */
/* PACKAGE */
/* VERSION */



/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */



@BOTTOM@

/* $BOTTOM$ */

#if defined(HAVE_ZLIB_H) && !defined(HAVE_LIBZ)
#  undef /**/ HAVE_ZLIB_H
#endif

#endif /* already included */

/*
vi:ts=4
*/
