#include <iostream>
#include <string>
#include <string.h>
#include <wcs.h>
#include <wcshdr.h>
#include <fitshdr.h>
#include <wcsfix.h>
#include <wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int Cube::getFITSdata(string fname)
{
  /**
   *  Cube::getFITSdata(string fname)
   *
   *   This routine will read in the data from the FITS file.
   *   It will initialise the cube, set the dim and data arrays,
   *    and make sure that the order of the axes are correct.
   *
   */

  fitsfile *fptr;
  int bitpix,numAxes;
  int noComments = 1; //so that fits_hdr2str will ignore COMMENT, HISTORY etc
  int nExc = 0,nkeys;
  char *hdr;
  int status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  status = 0;
  if(fits_get_img_dim(fptr, &numAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  long *dimAxes = new long[numAxes];
  for(int i=0;i<numAxes;i++) dimAxes[i]=1;
  status = 0;
  if(fits_get_img_size(fptr, numAxes, dimAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }




}

