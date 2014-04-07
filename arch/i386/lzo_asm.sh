/* lzo_asm.sh -- LZO assembler stuff

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

#if !defined(__i386__)
#  error
#endif

#if defined(LZO_HAVE_CONFIG_AC)
#  include <config.ac>
#endif

#if !defined(MFX_ASM_CAN_USE_EBP)
#  if defined(__linux__) || defined(__DJGPP__)
#    define MFX_ASM_CAN_USE_EBP
#  endif
#endif

#if !defined(MFX_ASM_ALIGN_BYTES) && !defined(MFX_ASM_ALIGN_PTWO)
#  if defined(__DJGPP__) || defined(__EMX__)
#    define MFX_ASM_ALIGN_PTWO
#  else
#    define MFX_ASM_ALIGN_BYTES
#  endif
#endif

#if !defined(LZO_ASM_ALIGN)
#  if defined(MFX_ASM_ALIGN_PTWO)
#    define LZO_ASM_ALIGN(x)    .align x
#  elif defined(MFX_ASM_ALIGN_BYTES)
#    define LZO_ASM_ALIGN(x)    .align (1 << (x))
#  else
#    error LZO_ASM_ALIGN
#  endif
#endif

#define ALIGN1              LZO_ASM_ALIGN(1)
#define ALIGN2              LZO_ASM_ALIGN(2)
#define ALIGN3              LZO_ASM_ALIGN(3)


#if defined(__ELF__)
#  define LZO_PUBLIC(func)      ALIGN3 ; .globl func ; func:
#  define LZO_PUBLIC_END(func)
#else
#  define LZO_PUBLIC(func)      ALIGN3 ; .globl _ ## func ; _ ## func:
#  define LZO_PUBLIC_END(func)
#endif


/***********************************************************************
//
************************************************************************/

#if defined(MFX_ASM_CAN_USE_EBP)
#  if 1 && !defined(N_3_EBP) && !defined(N_255_EBP)
#    define N_3_EBP
#  endif
#  if 0 && !defined(N_3_EBP) && !defined(N_255_EBP)
#    define N_255_EBP
#  endif
#endif

#if defined(N_3_EBP) && defined(N_255_EBP)
#  error
#endif
#if !defined(MFX_ASM_CAN_USE_EBP)
#  if defined(N_3_EBP) || defined(N_255_EBP)
#    error
#  endif
#endif

#if !defined(N_3)
#  if defined(N_3_EBP)
#    define N_3         %ebp
#  else
#    define N_3         $3
#  endif
#endif

#if !defined(N_255)
#  if defined(N_255_EBP)
#    define N_255       %ebp
#    define NOTL_3(r)   xorl %ebp,r
#  else
#    define N_255       $255
#  endif
#endif

#if !defined(NOTL_3)
#  define NOTL_3(r)     xorl N_3,r
#endif


#ifndef INP
#define INP      4+32(%esp)
#define INS      8+32(%esp)
#define OUTP    12+32(%esp)
#define OUTS    16+32(%esp)
#endif

#define INEND         4(%esp)
#define OUTEND        (%esp)


#if defined(LZO_TEST_DECOMPRESS_OVERRUN_INPUT)
#  define TEST_IP_R(r)      cmpl r,INEND ; jb L_input_overrun
#  define TEST_IP(addr,r)   leal addr,r ; TEST_IP_R(r)
#else
#  define TEST_IP_R(r)
#  define TEST_IP(addr,r)
#endif

#if defined(LZO_TEST_DECOMPRESS_OVERRUN_OUTPUT)
#  define TEST_OP_R(r)      cmpl r,OUTEND ; jb L_output_overrun
#  define TEST_OP(addr,r)   leal addr,r ; TEST_OP_R(r)
#else
#  define TEST_OP_R(r)
#  define TEST_OP(addr,r)
#endif

#if defined(LZO_TEST_DECOMPRESS_OVERRUN_LOOKBEHIND)
#  define TEST_LOOKBEHIND(r)    cmpl OUTP,r ; jb L_lookbehind_overrun
#else
#  define TEST_LOOKBEHIND(r)
#endif


/***********************************************************************
//
************************************************************************/

#define LODSB           movb (%esi),%al ; incl %esi

#define MOVSB(r1,r2,x)  movb (r1),x ; incl r1 ; movb x,(r2) ; incl r2
#define MOVSW(r1,r2,x)  movb (r1),x ; movb x,(r2) ; \
                        movb 1(r1),x ; addl $2,r1 ; \
                        movb x,1(r2) ; addl $2,r2
#define MOVSL(r1,r2,x)  movl (r1),x ; addl $4,r1 ; movl x,(r2) ; addl $4,r2

#if defined(LZO_DEBUG)
#define COPYB_C(r1,r2,x,rc) \
                        cmpl $0,rc ; jz L_error; \
                        cmpl INS,rc ; jae L_error; \
                        99: MOVSB(r1,r2,x) ; decl rc ; jnz 99b
#define COPYL_C(r1,r2,x,rc) \
                        cmpl $0,rc ; jz L_error; \
                        cmpl INS,rc ; jae L_error; \
                        99: MOVSL(r1,r2,x) ; decl rc ; jnz 99b
#else
#define COPYB_C(r1,r2,x,rc) \
                        99: MOVSB(r1,r2,x) ; decl rc ; jnz 99b
#define COPYL_C(r1,r2,x,rc) \
                        99: MOVSL(r1,r2,x) ; decl rc ; jnz 99b
#endif

#define COPYB(r1,r2,x)  COPYB_C(r1,r2,x,%ecx)
#define COPYL(r1,r2,x)  COPYL_C(r1,r2,x,%ecx)


/***********************************************************************
//  not used
************************************************************************/

#if 0

#if 0
#define REP_MOVSB(x)    rep ; movsb
#define REP_MOVSL(x)    shrl $2,%ecx ; rep ; movsl
#elif 1
#define REP_MOVSB(x)    COPYB(%esi,%edi,x)
#define REP_MOVSL(x)    shrl $2,%ecx ; COPYL(%esi,%edi,x)
#else
#define REP_MOVSB(x)    rep ; movsb
#define REP_MOVSL(x)    jmp 96f ; 95: movsb ; decl %ecx ; \
                        96: testl $3,%edi ; jnz 95b ; \
                        movl %ecx,x ; shrl $2,%ecx ; andl $3,x ; \
                        rep ; movsl ; movl x,%ecx ; rep ; movsb
#endif

#if 1
#define NEGL(x)         negl x
#else
#define NEGL(x)         xorl $-1,x ; incl x
#endif

#endif



/*
vi:ts=4
*/

