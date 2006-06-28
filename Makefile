CFLAGS = -c

FFLAGS = -C -fast -O4

CC=     g++ -c
CPP =   g++
F77=    g77 $(FFLAGS)

BASE = .

INSTALLDIR = .

EXEC = Duchamp

PGDIR = /u/whi550/pgplot/
PGPLOTLIB = -L$(PGDIR) -L/usr/X11R6/lib/ -lcpgplot -lpgplot -lX11

CFITSIOINC = /DATA/SITAR_1/whi550/cfitsio
#CFITSIODIR = /usr/local/lib
CFITSIODIR = /DATA/SITAR_1/whi550/cfitsio
CFITSIOLIB = -L$(CFITSIODIR) -lcfitsio

WCSDIR = /DATA/SITAR_1/whi550/wcslib-4.2
WCSLIB = -L$(WCSDIR)/C -lwcs -L$(WCSDIR)/pgsbox/ -lpgsbox

CINC= -I$(PGDIR) -I$(WCSDIR)/C/ -I$(WCSDIR)/pgsbox/ -I$(CFITSIOINC) -I$(BASE)

LIBS =  $(WCSLIB) $(PGPLOTLIB) $(CFITSIOLIB) -lm -lg2c -lstdc++

ATROUSDIR = $(BASE)/ATrous
DETECTIONDIR = $(BASE)/Detection
CUBESDIR = $(BASE)/Cubes
UTILDIR = $(BASE)/Utils

HEADS = $(BASE)/duchamp.hh\
	$(BASE)/param.hh\
	$(ATROUSDIR)/atrous.hh\
	$(DETECTIONDIR)/detection.hh\
	$(DETECTIONDIR)/columns.hh\
	$(CUBESDIR)/cubes.hh\
	$(CUBESDIR)/plots.hh\
	$(UTILDIR)/utils.hh

OBJECTS = $(BASE)/param.o\
	$(ATROUSDIR)/atrous.o\
	$(ATROUSDIR)/atrous_1d_reconstruct.o\
	$(ATROUSDIR)/atrous_2d_reconstruct.o\
	$(ATROUSDIR)/atrous_3d_reconstruct.o\
	$(ATROUSDIR)/baselineSubtract.o\
	$(ATROUSDIR)/ReconSearch.o\
	$(DETECTIONDIR)/detection.o\
	$(DETECTIONDIR)/columns.o\
	$(DETECTIONDIR)/areClose.o\
	$(DETECTIONDIR)/growObject.o\
	$(DETECTIONDIR)/lutz_detect.o\
	$(DETECTIONDIR)/mergeIntoList.o\
	$(DETECTIONDIR)/outputDetection.o\
	$(DETECTIONDIR)/sorting.o\
	$(DETECTIONDIR)/spectrumDetect.o\
	$(DETECTIONDIR)/thresholding_functions.o\
	$(CUBESDIR)/cubes.o\
	$(CUBESDIR)/baseline.o\
	$(CUBESDIR)/cubicSearch.o\
	$(CUBESDIR)/detectionIO.o\
	$(CUBESDIR)/drawMomentCutout.o\
	$(CUBESDIR)/getImage.o\
	$(CUBESDIR)/invertCube.o\
	$(CUBESDIR)/Merger.o\
	$(CUBESDIR)/outputSpectra.o\
	$(CUBESDIR)/plotting.o\
	$(CUBESDIR)/readRecon.o\
	$(CUBESDIR)/saveImage.o\
	$(CUBESDIR)/trimImage.o\
	$(UTILDIR)/cpgwedg_log.o\
	$(UTILDIR)/getStats.o\
	$(UTILDIR)/linear_regression.o\
	$(UTILDIR)/position_related.o\
	$(UTILDIR)/sort.o\
	$(UTILDIR)/wcsFunctions.o\
	$(UTILDIR)/zscale.o

$(OBJECTS) : $(HEADS)

duchamp : mainDuchamp.o $(OBJECTS)
	$(CPP) -o $(INSTALLDIR)/$(EXEC) mainDuchamp.o $(OBJECTS) $(LIBS)

.cc.o:
	$(CC) $< $(CINC) -o $@

.c.o:
	$(CC) $< $(CINC) -o $@

clean : 
	rm -f *.o $(ATROUSDIR)/*.o $(CUBESDIR)/*.o $(DETECTIONDIR)/*.o $(UTILDIR)/*.o 
