/* lzo1x_d.sh -- assembler implementation of the LZO1X decompression algorithm

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


/*
 * This file has been adapted from code generously contributed by
 * Laszlo Molnar aka ML1050 <ml1050@hotmail.com>
 *
 * Many thanks, Laszlo !
 *
 * I (Markus) have optimized this a lot, so enjoy...
 */


#if !defined(LZO1X) && !defined(LZO1Y)
#  define LZO1X
#endif


/***********************************************************************
// init
************************************************************************/

        xorl    %eax,%eax
        xorl    %ebx,%ebx       /* high bits 9-32 stay 0 */
        lodsb
        cmpb    $17,%al
        jbe     L01
        subb    $17-3,%al
        jmp     LFLR


/***********************************************************************
// literal run
************************************************************************/

0:      addl    N_255,%eax
        TEST_IP(18(%esi,%eax),%edx)     /* minimum */
1:      movb    (%esi),%bl
        incl    %esi
        orb     %bl,%bl
        jz      0b
        leal    18+3(%eax,%ebx),%eax
        jmp     3f


        ALIGN3
L00:
#ifdef LZO_DEBUG
    andl $0xffffff00,%eax ; jnz L_error
    andl $0xffffff00,%ebx ; jnz L_error
    xorl %eax,%eax ; xorl %ebx,%ebx
    xorl %ecx,%ecx ; xorl %edx,%edx
#endif
        TEST_IP_R(%esi)
        LODSB
L01:
        cmpb    $16,%al
        jae     LMATCH

/* a literal run */
        orb     %al,%al
        jz      1b
        addl    $3+3,%eax
3:
LFLR:
        TEST_OP(-3(%edi,%eax),%edx)
        TEST_IP(-3(%esi,%eax),%edx)
        movl    %eax,%ecx
        NOTL_3(%eax)
        shrl    $2,%ecx
        andl    N_3,%eax
#if 0
        rep
        movsl
#else
        COPYL(%esi,%edi,%edx)
#endif
        subl    %eax,%esi
        subl    %eax,%edi

#ifdef LZO_DEBUG
    andl $0xffffff00,%eax ; jnz L_error
    andl $0xffffff00,%ebx ; jnz L_error
    xorl %eax,%eax ; xorl %ebx,%ebx
    xorl %ecx,%ecx ; xorl %edx,%edx
#endif
        LODSB
        cmpb    $16,%al
        jae     LMATCH


/***********************************************************************
// R1
************************************************************************/

        TEST_OP(3(%edi),%edx)
        shrl    $2,%eax
        movb    (%esi),%bl
#if defined(LZO1X)
        leal    -0x801(%edi),%edx
#elif defined(LZO1Y)
        leal    -0x401(%edi),%edx
#endif
        leal    (%eax,%ebx,4),%eax
        incl    %esi
        subl    %eax,%edx
        TEST_LOOKBEHIND(%edx)
        movl    (%edx),%ecx
        movl    %ecx,(%edi)
        addl    N_3,%edi
        jmp     LMDONE


/***********************************************************************
// M2
************************************************************************/

        ALIGN3
LMATCH:
        cmpb    $64,%al
        jb      LM3MATCH

/* a M2 match */
        movl    %eax,%ecx
        shrl    $2,%eax
        leal    -1(%edi),%edx
#if defined(LZO1X)
        andl    $7,%eax
        movb    (%esi),%bl
        shrl    $5,%ecx
        leal    (%eax,%ebx,8),%eax
#elif defined(LZO1Y)
        andl    N_3,%eax
        movb    (%esi),%bl
        shrl    $4,%ecx
        leal    (%eax,%ebx,4),%eax
#endif
        incl    %esi
        subl    %eax,%edx

#if 0
        TEST_LOOKBEHIND(%edx)
        TEST_OP(1(%edi,%ecx),%eax)
        xchgl   %edx,%esi
#  if defined(LZO1X)
        incl    %ecx
#  elif defined(LZO1Y)
        decl    %ecx
#  endif
        rep
        movsb
        movl    %edx,%esi
        jmp     LMDONE
#elif 0
        TEST_LOOKBEHIND(%edx)
        TEST_OP(1(%edi,%ecx),%eax)
#  if defined(LZO1X)
        decl    %ecx
#  elif defined(LZO1Y)
        subl    N_3,%ecx
#  endif
        MOVSW(%edx,%edi,%al)
        COPYB(%edx,%edi,%al)
        jmp     LMDONE
#else
#  if defined(LZO1X)
        addl    $1+3,%ecx
#  elif defined(LZO1Y)
        addl    $2,%ecx
#  endif
        cmpl    N_3,%eax
        jae     LCOPYLONG
        jmp     LCOPYBYTE
#endif


/***********************************************************************
// M3
************************************************************************/

0:      addl    N_255,%eax
        TEST_IP(3(%esi),%edx)       /* minimum */
1:      movb    (%esi),%bl
        incl    %esi
        orb     %bl,%bl
        jz      0b
        leal    33+3(%eax,%ebx),%ecx
        xorl    %eax,%eax
        jmp     3f


        ALIGN3
LM3MATCH:
        cmpb    $32,%al
        jb      LM4MATCH

/* a M3 match */
        andl    $31,%eax
        jz      1b
        lea     2+3(%eax),%ecx
3:
#ifdef LZO_DEBUG
    andl $0xffff0000,%eax ; jnz L_error
#endif
        movw    (%esi),%ax
        leal    -1(%edi),%edx
        shrl    $2,%eax
        addl    $2,%esi
        subl    %eax,%edx

        cmpl    N_3,%eax
        jb      LCOPYBYTE


/***********************************************************************
// copy match
************************************************************************/

        ALIGN1
LCOPYLONG:                      /* copy match using longwords */
        TEST_LOOKBEHIND(%edx)
        leal    -3(%edi,%ecx),%eax
        shrl    $2,%ecx
        TEST_OP_R(%eax)
        COPYL(%edx,%edi,%ebx)
        movl    %eax,%edi
        xorl    %ebx,%ebx

LMDONE:
        movb    -2(%esi),%al
        andl    N_3,%eax
        jz      L00
        TEST_OP((%edi,%eax),%edx)
        TEST_IP((%esi,%eax),%edx)
        movl    (%esi),%edx
        addl    %eax,%esi
        movl    %edx,(%edi)
        addl    %eax,%edi
#ifdef LZO_DEBUG
    andl $0xffffff00,%eax ; jnz L_error
    andl $0xffffff00,%ebx ; jnz L_error
    xorl %eax,%eax ; xorl %ebx,%ebx
    xorl %ecx,%ecx ; xorl %edx,%edx
#endif
        LODSB
        jmp     LMATCH


        ALIGN3
LCOPYBYTE:                      /* copy match using bytes */
        TEST_LOOKBEHIND(%edx)
        TEST_OP(-3(%edi,%ecx),%eax)
#if 0
        subl    $5,%ecx
        MOVSW(%edx,%edi,%al)
        COPYB(%edx,%edi,%al)
#elif 0
        subl    N_3,%ecx
        COPYB(%edx,%edi,%al)
#else
        xchgl   %edx,%esi
        subl    N_3,%ecx
        rep
        movsb
        movl    %edx,%esi
#endif
        jmp     LMDONE


/***********************************************************************
// M4
************************************************************************/

0:      addl    N_255,%ecx
        TEST_IP(3(%esi),%edx)       /* minimum */
1:      movb    (%esi),%bl
        incl    %esi
        orb     %bl,%bl
        jz      0b
        leal    9+3(%ebx,%ecx),%ecx
        jmp     3f


        ALIGN3
LM4MATCH:
        cmpb    $16,%al
        jb      LM1MATCH

/* a M4 match */
        movl    %eax,%ecx
        andl    $8,%eax
        shll    $13,%eax        /* save in bit 16 */
        andl    $7,%ecx
        jz      1b
        addl    $2+3,%ecx
3:
#ifdef LZO_DEBUG
    movl %eax,%edx ; andl $0xfffe0000,%edx ; jnz L_error
#endif
        movw    (%esi),%ax
        addl    $2,%esi
        leal    -0x4000(%edi),%edx
        shrl    $2,%eax
        jz      LEOF
        subl    %eax,%edx
        jmp     LCOPYLONG


/***********************************************************************
// M1
************************************************************************/

        ALIGN3
LM1MATCH:
/* a M1 match */
        TEST_OP(2(%edi),%edx)
        shrl    $2,%eax
        movb    (%esi),%bl
        leal    -1(%edi),%edx
        leal    (%eax,%ebx,4),%eax
        incl    %esi
        subl    %eax,%edx
        TEST_LOOKBEHIND(%edx)

        movb    (%edx),%al      /* we must use this because edx can be edi-1 */
        movb    %al,(%edi)
        movb    1(%edx),%bl
        movb    %bl,1(%edi)
        addl    $2,%edi
        jmp     LMDONE


/***********************************************************************
//
************************************************************************/

LEOF:
/****   xorl    %eax,%eax          eax=0 from above */

        cmpl    $6,%ecx         /* ecx must be 6 */
        setnz   %al


/*
vi:ts=4
*/

