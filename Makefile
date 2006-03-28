CFLAGS = -c -g

FFLAGS = -C -fast -O4

CC=     g++ $(CFLAGS)
F77=    g77 $(FFLAGS)

BASE = .

PGDIR = /u/whi550/pgplot/
PGPLOTLIB = -L$(PGDIR) -L/usr/X11R6/lib/ -lcpgplot -lpgplot -lX11

#CFITSIOLIB = -lcfitsio 
CFITSIOINC = /DATA/SITAR_1/whi550/cfitsio
CFITSIODIR = /usr/local/lib
CFITSIOLIB = -L$(CFITSIODIR) -lcfitsio

WCSDIR = /DATA/SITAR_1/whi550/wcslib-4.2
WCSLIB = -L$(WCSDIR)/C -lwcs -L$(WCSDIR)/pgsbox/ -lpgsbox

CINC= -I$(PGDIR) -I$(WCSDIR)/C/ -I$(WCSDIR)/pgsbox/ -I$(CFITSIODIR) -I$(BASE)

LIBS =  $(WCSLIB) $(PGPLOTLIB) $(CFITSIOLIB) -lm -lg2c -lstdc++

ATROUSDIR = $(BASE)/ATrous
ATROUS = $(ATROUSDIR)/atrous_1d_reconstruct.o\
	$(ATROUSDIR)/atrous_2d_reconstruct.o\
	$(ATROUSDIR)/atrous_3d_reconstruct.o\
	$(ATROUSDIR)/baselineSubtract.o\
	$(ATROUSDIR)/ReconSearch.o

DETECTIONDIR = $(BASE)/Detection
DETECTION = $(DETECTIONDIR)/areClose.o\
	$(DETECTIONDIR)/growObject.o\
	$(DETECTIONDIR)/lutz_detect.o\
	$(DETECTIONDIR)/mergeIntoList.o\
	$(DETECTIONDIR)/outputDetection.o\
	$(DETECTIONDIR)/sorting.o\
	$(DETECTIONDIR)/thresholding_functions.o

CUBESDIR = $(BASE)/Cubes
CUBES = $(CUBESDIR)/cubicSearch.o\
	$(CUBESDIR)/detectionIO.o\
	$(CUBESDIR)/drawMomentCutout.o\
	$(CUBESDIR)/getImage.o\
	$(CUBESDIR)/Merger.o\
	$(CUBESDIR)/outputSpectra.o\
	$(CUBESDIR)/plotting.o\
	$(CUBESDIR)/removeMW.o\
	$(CUBESDIR)/saveImage.o\
	$(CUBESDIR)/trimImage.o

UTILDIR = $(BASE)/Utils
UTIL =	$(UTILDIR)/cpgwedg_log.o\
	$(UTILDIR)/getStats.o\
	$(UTILDIR)/linear_regression.o\
	$(UTILDIR)/position_related.o\
	$(UTILDIR)/sort.o\
	$(UTILDIR)/wcsFunctions.o\
	$(UTILDIR)/zscale.o

HEADS = $(BASE)/param.o\
	$(DETECTIONDIR)/detection.o\
	$(CUBESDIR)/cubes.o\
	$(ATROUSDIR)/atrous.o

duchamp : mainDuchamp.o $(HEADS) $(CUBES) $(ATROUS) $(DETECTION) $(UTIL)
	$(F77) -o Duchamp mainDuchamp.o $(HEADS) $(CUBES) $(ATROUS) $(DETECTION) $(UTIL) $(LIBS)

.cc.o:
	$(CC) $< $(CINC) -o $@

.c.o:
	$(CC) $< $(CINC) -o $@

clean : 
	rm -f *.o $(ATROUSDIR)/*.o $(CUBESDIR)/*.o $(DETECTIONDIR)/*.o $(UTILDIR)/*.o 
