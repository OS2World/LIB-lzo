# Local additions to Autoconf 2.10 macros.
#
# Copyright (C) 1992, 1994, 1995 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 1992.
#
# Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
# Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer

## ------------------------------- ##
## Check for function prototypes.  ##
## ------------------------------- ##

AC_DEFUN(fp_C_PROTOTYPES,
[AC_REQUIRE([fp_PROG_CC_STDC])
AC_MSG_CHECKING([for function prototypes])
if test "$ac_cv_prog_cc_stdc" != no; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(PROTOTYPES)
  U= ANSI2KNR=
else
  AC_MSG_RESULT(no)
  U=_ ANSI2KNR=./ansi2knr
fi
AC_SUBST(U)dnl
AC_SUBST(ANSI2KNR)dnl
])


## ----------------------------------------- ##
## ANSIfy the C compiler whenever possible.  ##
## ----------------------------------------- ##

# @defmac AC_PROG_CC_STDC
# @maindex PROG_CC_STDC
# @ovindex CC
# If the C compiler in not in ANSI C mode by default, try to add an option
# to output variable @code{CC} to make it so.  This macro tries various
# options that select ANSI C on some system or another.  It considers the
# compiler to be in ANSI C mode if it defines @code{__STDC__} to 1 and
# handles function prototypes correctly.
#
# If you use this macro, you should check after calling it whether the C
# compiler has been set to accept ANSI C; if not, the shell variable
# @code{ac_cv_prog_cc_stdc} is set to @samp{no}.  If you wrote your source
# code in ANSI C, you can make an un-ANSIfied copy of it by using the
# program @code{ansi2knr}, which comes with Ghostscript.
# @end defmac

AC_DEFUN(fp_PROG_CC_STDC,
[AC_MSG_CHECKING(for ${CC-cc} option to accept ANSI C)
AC_CACHE_VAL(ac_cv_prog_cc_stdc,
[ac_cv_prog_cc_stdc=no
ac_save_CFLAGS="$CFLAGS"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX                   -qlanglvl=ansi
# Ultrix and OSF/1      -std1
# HP-UX                 -Aa -D_HPUX_SOURCE
# SVR4                  -Xc
for ac_arg in "" -qlanglvl=ansi -std1 "-Aa -D_HPUX_SOURCE" -Xc
do
  CFLAGS="$ac_save_CFLAGS $ac_arg"
  AC_TRY_COMPILE(
[#if !defined(__STDC__) || __STDC__ != 1
choke me
#endif
], [int test (int i, double x);
struct s1 {int (*f) (int a);};
struct s2 {int (*f) (double a);};],
[ac_cv_prog_cc_stdc="$ac_arg"; break])
done
CFLAGS="$ac_save_CFLAGS"
])
AC_MSG_RESULT($ac_cv_prog_cc_stdc)
case "x$ac_cv_prog_cc_stdc" in
  x|xno) ;;
  *) CC="$CC $ac_cv_prog_cc_stdc" ;;
esac
])


## --------------------------------------------------------- ##
## Use AC_PROG_INSTALL, supplementing it with INSTALL_SCRIPT ##
## substitution.                                             ##
## --------------------------------------------------------- ##

AC_DEFUN(fp_PROG_INSTALL,
[AC_PROG_INSTALL
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL} -m 755'
AC_SUBST(INSTALL_SCRIPT)dnl
])


## --------------------------------------------------------- ##
## Includes <stddef.h> if available.                         ##
## Adapted from AC_CHECK_TYPE.                               ##
##                                                           ##
## Not needed since Autoconf 2.11                            ##
## --------------------------------------------------------- ##

dnl mfx_CHECK_TYPE(TYPE, DEFAULT)
AC_DEFUN(mfx_CHECK_TYPE,
[AC_REQUIRE([AC_HEADER_STDC])dnl
AC_MSG_CHECKING(for $1)
AC_CACHE_VAL(ac_cv_type_$1,
[AC_EGREP_CPP($1, [#include <sys/types.h>
#if STDC_HEADERS
#include <stdlib.h>
#endif
#if HAVE_STDDEF_H
#include <stddef.h>
#endif], ac_cv_type_$1=yes, ac_cv_type_$1=no)])dnl
AC_MSG_RESULT($ac_cv_type_$1)
if test $ac_cv_type_$1 = no; then
  AC_DEFINE($1, $2)
fi
])


## --------------------------------------------------------- ##
## Includes <stdlib.h> and <stddef.h> if available.          ##
## Adapted from AC_CHECK_SIZEOF (Autoconf 2.12).             ##
## --------------------------------------------------------- ##

dnl mfx_CHECK_SIZEOF(TYPE [, CROSS-SIZE])
AC_DEFUN(mfx_CHECK_SIZEOF,
[changequote(<<, >>)dnl
dnl The name to #define.
define(<<AC_TYPE_NAME>>, translit(sizeof_$1, [a-z *], [A-Z_P]))dnl
dnl The cache variable name.
define(<<AC_CV_NAME>>, translit(ac_cv_sizeof_$1, [ *], [_p]))dnl
changequote([, ])dnl
AC_MSG_CHECKING(size of $1)
AC_CACHE_VAL(AC_CV_NAME,
[AC_TRY_RUN([#include <stdio.h>
#if STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif
main()
{
  FILE *f=fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%d\n", sizeof($1));
  exit(0);
}], AC_CV_NAME=`cat conftestval`, AC_CV_NAME=0, ifelse([$2], , , AC_CV_NAME=$2))])dnl
AC_MSG_RESULT($AC_CV_NAME)
AC_DEFINE_UNQUOTED(AC_TYPE_NAME, $AC_CV_NAME)
undefine([AC_TYPE_NAME])dnl
undefine([AC_CV_NAME])dnl
])


## --------------------------------------------------------- ##
## Check if gcc accepts '-pipe'.                             ##
## Adapted from AC_PROG_CC and AC_PROG_GCC_TRADITIONAL.      ##
## --------------------------------------------------------- ##

AC_DEFUN(mfx_PROG_GCC_PIPE,
[AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
if test $ac_cv_prog_gcc = yes; then
AC_CACHE_CHECK(whether ${CC-cc} accepts -pipe, mfx_cv_prog_gcc_pipe,
[echo 'int main() { return 0; }' > conftest.c
if test -z "`${CC-cc} -pipe -c conftest.c 2>&1`"; then
  mfx_cv_prog_gcc_pipe=yes
else
  mfx_cv_prog_gcc_pipe=no
fi
rm -f conftest*
])
  if test $mfx_cv_prog_gcc_pipe = yes; then
    CC="$CC -pipe"
  fi
fi
])


## --------------------------------------------------------- ##
## Check if gcc suffers the '-fstrength-reduce' bug.         ##
## --------------------------------------------------------- ##

AC_DEFUN(mfx_PROG_GCC_SR_BROKEN,
[AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
if test $ac_cv_prog_gcc = yes; then
mfx_save_cflags=$CFLAGS
CFLAGS="$CFLAGS -O2 -fstrength-reduce"
AC_CACHE_CHECK(whether ${CC-cc} suffers the -fstrength-reduce bug,
mfx_cv_prog_gcc_sr_broken,
[AC_TRY_RUN([
/* gcc strength-reduction optimization bug on Intel platforms.
 * Adapted from bug-report by John E. Davis <davis@space.mit.edu>
 * Compile and run it using gcc -O2 -fno-strength-reduce and
 * gcc -O2 -fstrength-reduce.
 */
int a[3];
unsigned an = 3;
int strength_reduce_works_ok();
main()
{
  unsigned j;
  for (j = 0; j < an; j++)
    a[j] = (int)j - 3;
  exit(strength_reduce_works_ok() ? 0 : 1);
}
int strength_reduce_works_ok()
{
  return a[0] == -3 && a[1] == -2 && a[2] == -1;
}], mfx_cv_prog_gcc_sr_broken=no, mfx_cv_prog_gcc_sr_broken=yes)])
CFLAGS=$mfx_save_cflags
fi
MFX_PROG_GCC_SR_BROKEN=$mfx_cv_prog_gcc_sr_broken
AC_SUBST(MFX_PROG_GCC_SR_BROKEN)dnl
])


## --------------------------------------------------------- ##
## Check for 8-bit clean memcmp.                             ##
## Adapted from AC_FUNC_MEMCMP.                              ##
## --------------------------------------------------------- ##

AC_DEFUN(mfx_FUNC_MEMCMP,
[AC_CACHE_CHECK(for 8-bit clean memcmp, mfx_cv_func_memcmp,
[AC_TRY_RUN([
main()
{
  char c0 = 0x40, c1 = 0x80, c2 = 0x81;
  exit(memcmp(&c0, &c2, 1) < 0 && memcmp(&c1, &c2, 1) < 0 ? 0 : 1);
}
], mfx_cv_func_memcmp=yes, mfx_cv_func_memcmp=no, mfx_cv_func_memcmp=no)])
if test "$mfx_cv_func_memcmp" = no; then
  AC_DEFINE(MFX_MEMCMP_BROKEN)
fi
])


## --------------------------------------------------------- ##
## Check the byte order.                                     ##
## Adapted from AC_C_BIGENDIAN.                              ##
## --------------------------------------------------------- ##

AC_DEFUN(mfx_C_BYTE_ORDER,
[AC_CACHE_CHECK(the byte order, mfx_cv_c_byte_order,
[mfx_cv_c_byte_order=unknown
AC_TRY_RUN([main () {
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  exit (u.c[sizeof (long) - 1] == 1);
}], mfx_cv_c_byte_order=1234, mfx_cv_c_byte_order=4321)
])
if test $mfx_cv_c_byte_order = 1234; then
  AC_DEFINE_UNQUOTED(MFX_BYTE_ORDER,1234)
fi
if test $mfx_cv_c_byte_order = 4321; then
  AC_DEFINE_UNQUOTED(MFX_BYTE_ORDER,4321)
fi
])


## --------------------------------------------------------- ##
## Check the alignment of the assembler.                     ##
## Adapted from AC_CHECK_SIZEOF.                             ##
## --------------------------------------------------------- ##

AC_DEFUN(mfx_ASM_ALIGN,
[AC_CACHE_CHECK(the alignment of the assembler, mfx_cv_asm_align,
[AC_TRY_RUN([#include <stdio.h>
main()
{
#if defined(__GNUC__)
  extern char mfx_a1 __asm__("mfx_a1");
  extern char mfx_a2 __asm__("mfx_a2");
  long l = &mfx_a2 - &mfx_a1;
  FILE *f = fopen("conftestval", "w");
  if (!f) exit(1);
  fprintf(f, "%ld\n", l);
  exit(0);
  __asm__ __volatile__ ("
  .align 4
mfx_a1:
  .byte 0
  .align 4
mfx_a2:
  ");
#else
  exit(2);
#endif
}], mfx_cv_asm_align=`cat conftestval`, mfx_cv_asm_align=0)
])
if test $mfx_cv_asm_align = 16; then
  AC_DEFINE(MFX_ASM_ALIGN_PTWO)
else
  AC_DEFINE(MFX_ASM_ALIGN_BYTES)
fi
])

