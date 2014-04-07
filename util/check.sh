#! /bin/sh

#
# usage: util/check.sh [directory]
#
# This script runs lzo_test with all algorithms on a complete directory tree.
# It is not suitable for accurate timings.
#
# Copyright (C) 1996, 1997 Markus Franz Xaver Johannes Oberhumer
#

dir=${1-"."}

LZO_FLAGS="-q -n2"

TMPFILE="./lzo_test.tmp"
rm -f $TMPFILE
find $dir -type f -print > $TMPFILE

for i in 21 31 1 2 3 4 5 6 7 8 9 11 12 13 14 15 16 17 18 19 61 71
do
	cat $TMPFILE | ./lzo_test -m$i -@ $LZO_FLAGS
	err=$?; if test $err != 0; then exit $err; fi
done
for i in 921 931 901 911  902 912 942 962 972 982
do
	cat $TMPFILE | ./lzo_test -m$i -@ $LZO_FLAGS
	err=$?; if test $err != 0; then exit $err; fi
done

rm -f $TMPFILE
exit 0

