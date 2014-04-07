#! /bin/sh

#
# This is the 'official' LZO benchmark driver.
#
# usage: util/bench.sh Calgary-Corpus-directory
#
#
# Benchmark cookbook:
# -------------------
# 1) verify that LZO works (see PLATFORM.TXT)
# 2) check that your machine is idle
# 3) run 'sh util/bench.sh Calgary-Corpus-directory > bench.log'
#    note: this *must* take 20 minutes or more !
# 4) run 'perl util/table.pl -b bench.log' to get your benchmark values
#
#
# If the execution time of this script is less than 20 minutes,
# then increase the value of the variable 'n' below.
#
#
# Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer
#


# n=8 takes about 21 minutes on my Intel i486 DX2/66 (djgpp v2 + gcc 2.7.2)
n=8


d=$[ $n*4 ]
dir=${1-"."}

./lzo_test -mbench -b262144 -s1 $dir -c$n -d$d
err=$?; if test $err != 0; then exit $err; fi

exit 0

