#! /usr/bin/perl
##
## vi:ts=4
##
##---------------------------------------------------------------------------##
##  Author:
##      Markus F.X.J. Oberhumer         markus.oberhumer@jk.uni-linz.ac.at
##  Copyright:
##      Copyright (C) 1995, 1996 Markus F.X.J. Oberhumer
##  Description:
##      Select files for tar/zip distribution. Reads a list of files
##      from stdin and writes those to stdout that match an expression
##      from the file 'file-specs'.
##      Interpretation of 'file-specs' should be compatible to the RAR
##      archiver.
##---------------------------------------------------------------------------##

$VERSION = '1.01.5';
$PROG = $0;

sub usage {
	print <<"EOF";
usage: $PROG [-neqzpsVD] file-specs [prefix]
  -n      sort by name
  -e      sort by extension
  -b      sort by basename      (not yet implemented)
  -q      be quiet
  -z      remove leading '../'
  -p dir  add directory prefix 'dir'
  -s      change '/' to local directory separator
  -V      print version
  -D      print debug infos
EOF
	exit(1);
}


#
# init
#

# config
$dirsep = '/';						# directory separator
$dirsep = '\\' if $ENV{'COMSPEC'};
$dirsep = '\\' if $ENV{'OS2_SHELL'};

# options
require 'getopts.pl';
&Getopts('neqzp:sVD') || &usage();
print STDERR "$PROG version $VERSION\n" if ($opt_V || $opt_D);
if (!$opt_V) {
	$#ARGV >= 0 || &usage();
}
$#ARGV >= 0 || exit(0);

# get filespec
$filespec = shift;

# get prefix
$prefix = $opt_p;
$prefix =~ s%\\%/%g;
$prefix .= '/' if $prefix;		# add trailing '/'


#
# read filespec
#

@Files = ();
open(FILESPEC,"$filespec") || die "$PROG: cannot open '$filespec': $!\n";
while (<FILESPEC>) {
	chop;
	next if /^\s*$/;				# skip empty lines
	next if /^\s*\#/;				# skip comment lines
	s/^\s+//;						# remove leading whitespace
	s/\s+$//;						# remove trailing whitespace
	s%\\%/%g;
	push(@Files,$_);
}
close(FILESPEC);

die "$PROG: no file-specs found in '$filespec'\n" unless $#Files >= 0;


#
# Make a Perl regexp. The RAR spec should always contain a '.'.
#
#   RAR spec    UNIX        Perl
#   ------      -------     ------
#   x.c         x.c         x\.c
#   *.c         *.c         .*\.c
#   *.*         *           .*
#   x.*         x.*         x\..*
#
# to be implemented:
#   *           [^.]*
#   x*          x[^.]*
#

for (@Files) {
	s%\\%/%g;					# convert to forward slashes
	s%/+%/%g;					# remove duplicate '/'
	s%([\/\-\!+~:])%\\$1%g;		# quote all '/-!+~:'

	# order of substituions is important
	s/\*\.\*/\*/g;				# change all '*.*' to '*'
	s/\./\\\./g;				# quote all '.'
	s/\*/\.\*/g;				# change all '*' to '.*'
}

#
$Files = join(')|(', @Files);
$Files = '(^|[\\\/])((' . $Files . '))$';
eval { /$Files/, 1 } || die "$PROG: bad pattern: $@";
print STDERR "$Files\n" if $opt_D;		# debug


#
# process files or stdin
#

@f = ();

$i = 0;
while (<>) {
	chop;

	s%\\%/%g;
	s/^(\.\.\/)+// if $opt_z;		# remove leading '../'
	s/^(\.\/)+//;					# remove leading './'

	next if /^\s*$/;				# skip empty lines
	next if /^\s*\#/;				# skip comment lines
	next if /^\.{1,2}$/;			# skip '.' and '..'

	next unless (/$Files/io);		# ignore case in filename matching !

	push(@f,${prefix} . $_);
	$i++;
}

# print info message
print STDERR "$PROG: $i files\n" unless $opt_q;


#
# sort
#

sub ext_cmp {
	local ($aa, $bb);

	#        |dir| |file  | |  ext?  | | ext  |
	$a =~ m%^(.*/)?([^./]+)?([^./]*\.)*([^./]*)$%;
	$aa = $4 . '.' . $2 . '.' . $1;
	$aa =~ tr/A-Z/a-z/;
	$b =~ m%^(.*/)?([^./]+)?([^./]*\.)*([^./]*)$%;
	$bb = $4 . '.' . $2 . '.' . $1;
	$bb =~ tr/A-Z/a-z/;
	## print STDERR "$a='$aa'   $b='$bb'\n";

	$aa cmp $bb;
}

# sort by extension
@f = sort(ext_cmp @f) if $opt_e;

# sort by name
@f = sort(@f) if $opt_n;


#
# print to stdout
#

binmode(STDOUT);
for (@f) {
	s%/+%/%g;					# remove duplicate '/'
	s%/%$dirsep%g if $opt_s;
	s/${dirsep}+/$dirsep/g if $opt_s;
	print "$_\n";
}

exit(0);
