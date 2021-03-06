#!/bin/bash

fullComp=false

if [ $# -gt 0 ]; then

    if [ $1 == "-f" ]; then
        fullComp=true
    else
        echo "VerifyDuchamp.sh: A script to run multiple Duchamp runs, testing different algorithms."
        echo "   Options: -f - Full comparison. This may throw up differences arising merely from precision"
        echo "                 The default is a basic comparison that won't be affected by precision changes"
        echo "            -h - This help text."
        exit 0
    fi

fi

version=`grep VERSION src/config.h | sed '/\"/s///' | sed '/\"/s///' | awk '{print $3}'`

progname=./Duchamp-$version
dir=./verification

numErrors=0

ndet=(5 5 5 5 1 1 5 5 5)
number=("first" "second" "third" "fourth" "fifth" "sixth" "seventh" "eighth" "ninth")
explanation=("This is a simple sigma-clipping search" 
"This uses the FDR method" 
"This spatially smoothes the cube, then searches with simple sigma-clipping" 
"This reconstructs the cube, then searches with simple sigma-clipping" 
"This just uses a small subsection of the full cube, with simple searching" 
"This reconstructs the subsection, then searches with simple sigma-clipping" 
"This does the same as the first, and grows detections" 
"This does the same as the first, but with regular statistics instead of robust"
"This does the same as the first, but changing the spectral type, units, and rest frequency")
explanation2=("-" "-" "It should take a bit longer than the first two." "It should take a bit longer again" "-" "-" "-" "-" "-")

##################################################################
# Run the tests.
# First, test the results file, just looking at the number of sources 
#  and their pixel positions.
# This is to avoid precision problems with the WCS positions.
# Second, test the entire log files.

for (( i=0; i<${#number[@]}; i++ )); do
    
    N=`expr $i + 1`
    in=${dir}/input${N}
    res=${dir}/results${N}.txt
    Sres=${dir}/stdResults${N}.txt
    log=${dir}/log${N}.txt
    Slog=${dir}/stdLog${N}.txt
    vot=${dir}/results${N}.xml
    Svot=${dir}/stdResults${N}.xml
    karma=${dir}/results${N}.ann
    Skarma=${dir}/stdResults${N}.ann
    ds9=${dir}/results${N}.reg
    Sds9=${dir}/stdResults${N}.reg

    temp1=/tmp/duchampRes
    temp2=/tmp/duchampComp

    echo "Running the ${number[i]} Duchamp test:"
    echo "  [${explanation[i]}]"
    if  (( ( $i == "2" ) || ( $i == "3" ) )); then
	echo "  [${explanation2[i]}]"
    fi
    rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
    $progname -p $in > /tmp/duchamptest
    echo "Done. Comparison to standard result:"
    numDet=`grep "Total number" $res | cut -f 7 -d " "`
    if [ $numDet == ${ndet[i]} ]; then
	if [ $fullComp == true ]; then
	    numdiff=`diff -I"Duchamp" $res $Sres | wc -l`
	else
	    tail -${numDet} $res | awk '{print $1,$3,$4,$5}' > $temp1
	    tail -${numDet} $Sres | awk '{print $1,$3,$4,$5}' > $temp2
	    numdiff=`diff $temp1 $temp2 | wc -l`
	fi
	if [ $numdiff != 0 ]; then
	    if [ $fullComp == true ]; then
		echo "  Found correct number of sources, but differences exist in the results table."
		echo "  ERROR: Differences in results:"
	    else
		echo "  Found correct number of sources, but positions differ."
		echo "  ERROR: Differences in positions of sources:"
	    fi
	    diff -I"Duchamp" $res $Sres
	    numErrors=`expr $numErrors + 1`
	else
	    echo "  All detections correct."
	fi
    else
	echo "  ERROR. Wrong number of sources found."
	echo "Differences in results:"
	diff  -I"Duchamp" $res $Sres
	numErrors=$numErrors+1
    fi

#Test the log files.
    if [ `diff -I"flagXOutput" -I"Duchamp" -I"spectraFile" $log $Slog | wc -l` == 0 ]; then
	echo "  Logfiles correct."
    else
	echo "  ERROR: Differences in the log files:"
	diff -I"flagXOutput" -I"Duchamp" -I"spectraFile" $log $Slog
	numErrors=`expr $numErrors + 1`
    fi

#Test the VOTable results.
    if [ $fullComp == true ]; then
	numdiff=`diff $vot $Svot | wc -l`
    else
	head -`grep -n "<TABLEDATA>" $vot | sed -e 's/://g' | cut -f 1 -d ' '` $vot > $temp1
	head -`grep -n "<TABLEDATA>" $Svot | sed -e 's/://g' | cut -f 1 -d ' '` $Svot > $temp2
	numdiff=`diff $temp1 $temp2 | wc -l`
    fi
    if [ $numdiff == 0 ]; then
        echo "  VOTables correct."
    else
        echo "  ERROR: Differences in the VOTables:"
	if [ $fullComp == true ]; then
            diff $vot $Svot
	else
	    diff $temp1 $temp2
	fi
        numErrors=`expr $numErrors + 1`
    fi

#Test the Karma annotation files
    if [ $fullComp == true ]; then
	numdiff=`diff -I"Duchamp" $karma $Skarma | wc -l`
    else
	grep -v -e "^#" $karma > $temp1
	grep -v -e "^#" $Skarma > $temp2
	numdiff=`diff $temp1 $temp2 | wc -l`
    fi
    if [ $numdiff == 0 ]; then
        echo "  Karma annotation files correct."
    else
        echo "  ERROR: Differences in the Karma annotation files:"
	if [ $fullComp == true ]; then
            diff $karma $Skarma
	else
	    diff $temp1 $temp2
	fi
        numErrors=`expr $numErrors + 1`
    fi

#Test the DS9 annotation files
    if [ $fullComp == true ]; then
	numdiff=`diff -I"Duchamp" $ds9 $Sds9 | wc -l`
    else
	grep -v -e "^#" $ds9 > $temp1
	grep -v -e "^#" $Sds9 > $temp2
	numdiff=`diff $temp1 $temp2 | wc -l`
    fi
    if [ $numdiff == 0 ]; then
        echo "  DS9 annotation files correct."
    else
        echo "  ERROR: Differences in the DS9 annotation files:"
	if [ $fullComp == true ]; then
            diff $ds9 $Sds9
	else
	    diff $temp1 $temp2
	fi
        numErrors=`expr $numErrors + 1`
    fi

    echo ""
    echo ""
    
done

##################################################################
# Summarise the results
# Either give the OK, or give some advice about what to do.

echo " "
if [ $numErrors == 0 ]; then
    echo "No errors! Everything is working fine."
    echo " "
    echo "Happy Finding! If you ever need to report a bug or request an upgrade, go to"
    echo "  http://svn.atnf.csiro.au/trac/duchamp/newticket"
else
    echo "There were some differences in the results compared to the standards."
    echo "Have a look at the diffs to see if they are trivial or not (eg. precision errors)."
    echo "If they are more serious (e.g. wrong number of sources found), submit a bug report"
    echo "  at http://svn.atnf.csiro.au/trac/duchamp/newticket"
fi
