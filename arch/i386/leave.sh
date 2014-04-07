/* leave.sh -- LZO assembler stuff

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

        movl    INP,%edx
        addl    INS,%edx
        cmpl    %edx,%esi       /* check compressed size */
        setnz   %bl
        orb     %bl,%al

#if defined(LZO_TEST_DECOMPRESS_OVERRUN_OUTPUT)
        cmpl    OUTEND,%edi     /* check uncompressed size */
        seta    %bl
        orb     %bl,%al
#endif

L_leave:
        subl    OUTP,%edi       /* write back the uncompressed size */
        movl    OUTS,%edx
        movl    %edi,(%edx)

        negl    %eax
        addl    $8,%esp
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %esi
        popl    %edi
        popl    %ebp
        ret


#if defined(LZO_TEST_DECOMPRESS_OVERRUN_INPUT)
L_input_overrun:
        movl    $4,%eax         /* LZO_E_INPUT_OVERRUN */
        jmp     L_leave
#endif

#if defined(LZO_TEST_DECOMPRESS_OVERRUN_OUTPUT)
L_output_overrun:
        movl    $5,%eax         /* LZO_E_OUTPUT_OVERRUN */
        jmp     L_leave
#endif

#if defined(LZO_TEST_DECOMPRESS_OVERRUN_LOOKBEHIND)
L_lookbehind_overrun:
        movl    $6,%eax         /* LZO_E_LOOKBEHIND_OVERRUN */
        jmp     L_leave
#endif

#if defined(LZO_DEBUG)
L_error:
        movl    $99,%eax
        jmp     L_leave
#endif


/*
vi:ts=4
*/

