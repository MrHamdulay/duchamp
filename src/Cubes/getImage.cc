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

string imageType[4] = {"point", "spectrum", "image", "cube"};

int Cube::getCube(string fname)
{
  /**
   * Cube::getCube(string )
   *  Read in a cube from the file fname (assumed to be in FITS format).
   *  Function reads in the following:
   *      - axis dimensions
   *      - pixel array
   *      - WCS information (in form of WCSLIB wcsprm structure)
   *      - Header keywords: BUNIT (brightness unit), 
   *                         BLANK, BZERO, BSCALE (for blank pixel value)
   *                         BMAJ, BMIN, CDELT1, CDELT2 (for beam size)
   */

  long nelements;
  int bitpix,numAxes;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  status = 0;
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
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  if(numAxes<=3)  std::cout << "Dimensions of FITS file: ";
  int dim = 0;
  std::cout << dimAxes[dim];
  while(dim+1<numAxes) std::cout << "x" << dimAxes[++dim];
  std::cout << std::endl;

  delete [] dimAxes;

  this->head.defineWCS(fname, this->par);

  if(!this->head.isWCS()) 
    duchampWarning("getCube","WCS is not good enough to be used.\n");

  std::cerr << "Reading data ... ";
  this->getFITSdata(fname);
  std::cerr << "Done. Data array has dimensions: ";
  std::cerr << this->axisDim[0] <<"x" 
	    << this->axisDim[1] <<"x" 
	    << this->axisDim[2] << std::endl;

  this->head.readHeaderInfo(fname, this->par);
  this->par.copyHeaderInfo(this->head);

  return SUCCESS;

}

