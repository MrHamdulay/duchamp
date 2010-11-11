#!/bin/bash 

version=`grep VERSION src/config.h | sed '/\"/s///' | sed '/\"/s///' | awk '{print $3}'`

progname=./Duchamp-$version

numErrors=0

##################################################################
# Run the first test, looking at simple sigma-clipped search
# First, test the results file, just looking at the number of sources 
#  and their pixel positions.
# This is to avoid precision problems with the WCS positions.
# Second, test the entire log files.

echo "Running the first Duchamp test:"
echo "  [This is a simple sigma-clipping search]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input1 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results1.txt | cut -f 6 -d " "`
if [ $numDet == 5 ]; then
    tail -5 verification/results1.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults1.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results1.txt verification/stdResults1.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results1.txt verification/stdResults1.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log1.txt verification/stdLog1.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log1.txt verification/stdLog1.txt
    numErrors=$numErrors+1
fi

##################################################################
# The next test uses the FDR method, but is otherwise the same.

echo " "
echo "Running the second Duchamp test:"
echo "  [This uses the FDR method]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input2 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results2.txt | cut -f 6 -d " "`
if [ $numDet == 5 ]; then
    tail -5 verification/results2.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults2.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results2.txt verification/stdResults2.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results2.txt verification/stdResults2.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log2.txt verification/stdLog2.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log2.txt verification/stdLog2.txt
    numErrors=$numErrors+1
fi

##################################################################
# The third test smoothes the cube and uses sigma-clipping

echo " "
echo "Running the third Duchamp test:"
echo "  [This spatially smoothes the cube, then searches with simple sigma-clipping]"
echo "  [It should take a bit longer than the first two.]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input3 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results3.txt | cut -f 6 -d " "`
if [ $numDet == 5 ]; then
    tail -5 verification/results3.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults3.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results3.txt verification/stdResults3.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results3.txt verification/stdResults3.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log3.txt verification/stdLog3.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log3.txt verification/stdLog3.txt
    numErrors=$numErrors+1
fi

##################################################################
# The final test reconstructs the cube and uses sigma-clipping

echo " "
echo "Running the fourth Duchamp test:"
echo "  [This reconstructs the cube, then searches with simple sigma-clipping]"
echo "  [It should take a bit longer again]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input4 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results4.txt | cut -f 6 -d " "`
if [ $numDet == 5 ]; then
    tail -5 verification/results4.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults4.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results4.txt verification/stdResults4.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results4.txt verification/stdResults4.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log4.txt verification/stdLog4.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log4.txt verification/stdLog4.txt
    numErrors=$numErrors+1
fi

##################################################################
# We now do two tests using the subsection functionality.
# First just using the basic search

echo " "
echo "Running the fifth Duchamp test:"
echo "  [This just uses a small subsection of the full cube, with simple searching]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input5 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results5.txt | cut -f 6 -d " "`
if [ $numDet == 1 ]; then
    tail -5 verification/results5.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults5.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results4.txt verification/stdResults4.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results4.txt verification/stdResults4.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log5.txt verification/stdLog5.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log5.txt verification/stdLog5.txt
    numErrors=$numErrors+1
fi

##################################################################
# This test uses the subsection, but does the reconstruction as well

echo " "
echo "Running the sixth Duchamp test:"
echo "  [This reconstructs the subsection, then searches with simple sigma-clipping]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input6 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results6.txt | cut -f 6 -d " "`
if [ $numDet == 1 ]; then
    tail -5 verification/results6.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults6.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results6.txt verification/stdResults6.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results6.txt verification/stdResults6.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log6.txt verification/stdLog6.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log6.txt verification/stdLog6.txt
    numErrors=$numErrors+1
fi
##################################################################
# This test does the same as the first, but with growing to a lower threshold

echo " "
echo "Running the seventh Duchamp test:"
echo "  [This does the same as the first, and grows detections]"
rm -f /tmp/duchampRes /tmp/duchampComp /tmp/duchamptest
$progname -p verification/input7 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
numDet=`grep "Total number" verification/results7.txt | cut -f 6 -d " "`
if [ $numDet == 5 ]; then
    tail -5 verification/results7.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampRes
    tail -5 verification/stdResults7.txt | awk '{print $1,$3,$4,$5}' > /tmp/duchampComp
    if [ `diff /tmp/duchampRes /tmp/duchampComp | wc -l` != 0 ]; then
	echo "  Found correct number of sources, but positions differ."
	echo "  ERROR: Differences in positions of sources:"
	diff  -I"Results of the Duchamp source finder:" verification/results7.txt verification/stdResults7.txt
	numErrors=$numErrors+1
    else
	echo "  All detections correct."
    fi
else
    echo "  ERROR. Wrong number of sources found."
    echo "Differences in results:"
    diff  -I"Results of the Duchamp source finder:" verification/results7.txt verification/stdResults7.txt
    numErrors=$numErrors+1
fi
#Test the log files.
if [ `diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log7.txt verification/stdLog7.txt | wc -l` == 0 ]; then
    echo "  Logfiles correct."
else
    echo "  ERROR: Differences in the log files:"
    diff -I"flagXOutput" -I"New run of the Duchamp source" -I"Executing statement" -I"Duchamp completed:" verification/log7.txt verification/stdLog7.txt
    numErrors=$numErrors+1
fi

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
