#!/bin/bash -l

pg=`grep "\-lpgplot" Makefile`
cpg=`grep "\-lcpgplot" Makefile`
Rstring=`grep "\-R" Makefile`

if [ "$pg" == "" ]; then
    if [ "$cpg" != "" ]; then
	perl -pi -e 's/\-lcpgplot/\-lcpgplot -lpgplot/g' Makefile
    fi
fi

if [ "$Rstring" != "" ]; then
    perl -pi -e 's|\-R[a-zA-z0-9/]* ||g' Makefile
fi
