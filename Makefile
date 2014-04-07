#
#  Makefile -- GNU makefile for the LZO library
#
#  This file is part of the LZO real-time data compression library.
#
#  Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
#  Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer
#
#  The LZO library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of
#  the License, or (at your option) any later version.
#
#  The LZO library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the LZO library; see the file COPYING.
#  If not, write to the Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  Markus F.X.J. Oberhumer
#  markus.oberhumer@jk.uni-linz.ac.at
#


#
# This makefile requires GNU make !
#
# Do the following:
#   1) sh ./configure  (UNIX only)
#   2) make
#   3) make test
#   4) run util/check.sh on a larger directory tree
#   5) make install
#
# You should not have to change anything here.
# Edit config/Makedefs.* instead.
#


# /***********************************************************************
# // configuration (with autoconf support)
# ************************************************************************/

# LZO_DEBUG = 1			# turn assertions on, include debug info
# LZO_BOUNDS_CHECKING = 1	# enable bounds-checking
# LZO_CPLUSPLUS = 1		# compile as C++ source files
#				# (interface still has C linkage)

SRCDIRS := src extra
SRCDIR  := $(strip $(firstword $(SRCDIRS)))


#
# figure out operating system we are running on
#

_MFX_MAKEFILE =
_MFX_MAKEDEFS =

ifeq (/etc/passwd /dev/null,$(wildcard /etc/passwd /dev/null))
mfx_host_os = UNIX
_MFX_MAKEDEFS = Makedefs.ac
ifeq (Makefile.ac,$(wildcard Makefile.ac))
_MFX_MAKEFILE = Makefile.ac
endif
MFX_USE_AUTOCONF = yes
else

ifneq ($(strip $(OS2_SHELL)),)
mfx_host_os = OS2
ifeq (config/Makedefs.os2,$(wildcard config/Makedefs.os2))
_MFX_MAKEDEFS = config/Makedefs.os2
_MFX_MAKEFILE = config/Makefile.in
else
_MFX_MAKEDEFS = config/makedefs.os2
_MFX_MAKEFILE = config/makefile.in
endif
else

ifneq ($(strip $(COMSPEC)),)
mfx_host_os = MSDOS
ifeq (config/Makedefs.dos,$(wildcard config/Makedefs.dos))
_MFX_MAKEDEFS = config/Makedefs.dos
_MFX_MAKEFILE = config/Makefile.in
else
_MFX_MAKEDEFS = config/makedefs.dos
_MFX_MAKEFILE = config/makefile.in
endif
endif
endif
endif


ifeq ($(strip $(MFX_MAKEDEFS)),)
override MFX_MAKEDEFS = $(_MFX_MAKEDEFS)
endif
ifeq ($(strip $(MFX_MAKEFILE)),)
override MFX_MAKEFILE = $(_MFX_MAKEFILE)
endif


# /***********************************************************************
# // get the Makefile definitions
# ************************************************************************/

ifeq ($(MFX_MAKEDEFS),$(wildcard $(MFX_MAKEDEFS)))
include $(MFX_MAKEDEFS)
else
MFX_MAKEFILE =
ifeq ($(strip $(MFX_USE_AUTOCONF)),yes)
.PHONY: run-configure
run-configure:
	/bin/sh ./configure
	@echo
	@echo Configuration finished. Now run 'make' again to build LZO.
	@echo
else
.PHONY: no-configure
no-configure:
	@echo
	@echo Configuration problem. Stop.
	@echo
endif
endif



# /***********************************************************************
# // get the real makefile
# ************************************************************************/

ifneq ($(strip $(MFX_MAKEFILE)),)
include $(MFX_MAKEFILE)
else

# these targets are for the maintainer only
ifeq (config/Makefile.in,$(wildcard config/Makefile.in))
.PHONY: autoconf autoconf-clean configure-clean grep
autoconf autoconf-clean configure-clean grep:
	$(MAKE) -f config/Makefile.in $@
else
ifeq (config/makefile.in,$(wildcard config/makefile.in))
.PHONY: rename
rename:
	$(MAKE) -f config/makefile.in $@
endif
endif

endif

