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


//   short int maxdim=3;
//   long *dimAxes = new long[maxdim];
//   for(int i=0;i<maxdim;i++) dimAxes[i]=1;
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
//   status = 0;
//   fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
//   if(status){
//     fits_report_error(stderr, status);
//     return FAILURE;
//   }

  long *fpixel = new long[numAxes];
  for(int i=0;i<numAxes;i++) fpixel[i]=1;

  if(numAxes<=3)  std::cout << "Dimensions of " << imageType[numAxes];
  else std::cout << "Dimensions of " << imageType[3];
  int dim = 0;
  std::cout << ": " << dimAxes[dim];
  while(dim+1<numAxes) std::cout << "x" << dimAxes[++dim];
  std::cout << std::endl;

  int npix = dimAxes[0]*dimAxes[1]*dimAxes[2];
  float *array = new float[npix];
  int anynul;
  char *nullarray = new char[npix];
  for(int i=0;i<npix;i++) nullarray[i] = 0;

  //-------------------------------------------------------------
  // Reading in the pixel array.
  //  The location of any blank pixels (as determined by the header keywords) 
  //   is stored in nullarray (set to 1). Also anynul will be set to 1 to 
  //   indicate the presence of blank pixels. 
  //   If anynul==1 then all pixels are non-blank.

  status = 0;
  fits_read_pixnull(fptr, TFLOAT, fpixel, npix, array, 
		    nullarray, &anynul, &status);
//   fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, array, &anynul, &status);
  if(status){
    duchampError("getCube","There was an error reading in the data array:");
    fits_report_error(stderr, status);
    return FAILURE;
  }

  if(anynul==0){    
    // no blank pixels, so don't bother with any trimming or checking...
    if(this->par.getFlagBlankPix()) {  
      // if user requested fixing, inform them of change.
      duchampWarning("getCube",
		     "No blank pixels, so setting flagBlankPix to false.\n");
    }
    this->par.setFlagBlankPix(false); 
  }
  
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  FitsHeader newHead;

  newHead.readHeaderInfo(fname, this->par);
  this->par.copyHeaderInfo(newHead);
  this->setHead(newHead);


  this->initialiseCube(dimAxes);
  this->saveArray(array,npix);
  //-------------------------------------------------------------
  // Once the array is saved, change the value of the blank pixels from
  // 0 (as they are set by fits_read_pixnull) to the correct blank value
  // as determined by the above code.
  for(int i=0; i<npix;i++){
//     if(nullarray[i]==1) this->array[i] = blank*bscale + bzero;  
    if(isnan(array[i])) this->array[i] = par.getBlankPixVal();
    if(nullarray[i]==1) this->array[i] = this->par.getBlankPixVal();  
  }


  if(!newHead.isWCS()) 
    duchampWarning("getCube","WCS is not good enough to be used.\n");

  //  delete hdr;
  delete [] array;
  delete [] fpixel;
  delete [] dimAxes;

  return SUCCESS;

}

