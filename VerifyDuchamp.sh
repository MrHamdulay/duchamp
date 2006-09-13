#!/bin/csh

#if( -e verification/verificationCube.fits ) then
#    echo "Test FITS file exists. Moving to testing."
#else
#    echo "Test FITS file has not been created. Doing so now."
#    make createTestImage
#    createTestImage.x
#    rm -f createTestImage.x
#    echo "Done. Moving to testing."
#endif

echo " "
echo "Running the first Duchamp test:"
echo "  [This is a simple sigma-clipping search]"
rm -f /tmp/duchamptest
Duchamp -p verification/input1 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
echo "Differences with Results:"
diff -I"Results of the Duchamp source finder:" verification/results1.txt verification/stdResults1.txt
echo "Differences with Log:"
diff -I"New run of the Duchamp sourcefinder" verification/log1.txt verification/stdLog1.txt

echo " "
echo "Running the second Duchamp test:"
echo "  [This uses the FDR method]"
rm -f /tmp/duchamptest
Duchamp -p verification/input2 > /tmp/duchamptest
echo "Done. Comparison to standard result:"
echo "Differences with Results:"
diff -I"Results of the Duchamp source finder:" verification/results2.txt verification/stdResults2.txt
echo "Differences with Log:"
diff -I"New run of the Duchamp sourcefinder" verification/log2.txt verification/stdLog2.txt

echo " "
echo "Running the third Duchamp test:"
echo "  [This reconstructs the cube, then searches with simple sigma-clipping]"
echo "  [It should take a bit longer than the first two.]"
rm -f /tmp/duchamptest
Duchamp -p verification/input3 > /tmp/duchamptest
rm -f /tmp/duchamptest
echo "Done. Comparison to standard result:"
echo "Differences with Results:"
diff -I"Results of the Duchamp source finder:" verification/results3.txt verification/stdResults3.txt
echo "Differences with Log:"
diff -I"New run of the Duchamp sourcefinder" verification/log3.txt verification/stdLog3.txt

echo "No differences? Then everything worked as expected\!"
