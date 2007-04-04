#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <wcs.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <param.hh>
#include <duchamp.hh>
#include <Utils/Section.hh>

void Param::setOffsets(struct wcsprm *wcs)
{
  /** 
   * If there is a subsection being used, set the offset values
   * according to the correct dimensions given by the WCS struct.  
   * If not, set the offsets to zero.
   * \param wcs The WCSLIB wcsprm struct that defines which axis is which.
   */
  if(this->flagSubsection){ // if so, then the offsets array is defined.
    this->xSubOffset = this->pixelSec.getStart(wcs->lng);
    this->ySubOffset = this->pixelSec.getStart(wcs->lat);
    this->zSubOffset = this->pixelSec.getStart(wcs->spec);
  }
  else{// else they should be 0
    this->xSubOffset = this->ySubOffset = this->zSubOffset = 0;
  }
}

int Param::verifySubsection()
{
  /**
   *   Checks that the subsection string is in the appropriate format, with
   *    the correct number of entries (one for each axis).
   *   This involves reading the individual substrings and converting to
   *    integers, and storing in the offsets array.
   *   Steps in the subsection string are not dealt with -- a warning message
   *    is written to the screen, and the step values are removed from the 
   *    subsection string.
   */

  // First open the requested FITS file and check its existence and 
  //  number of axes.
  int numAxes,status = 0;  /* MUST initialize status */
  fitsfile *fptr;         

  // Make sure the FITS file exists
  int exists;
  fits_file_exists(this->imageFile.c_str(),&exists,&status);
  if(exists<=0){
    fits_report_error(stderr, status);
    duchampWarning("verifySubsection", "Requested image does not exist!\n");
    return FAILURE;
  }
  // Open the FITS file
  if( fits_open_file(&fptr,this->imageFile.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  // Read the size information -- number of axes and their sizes
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
  // Close the FITS file.
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("verifySubsection","Error closing file: ");
    fits_report_error(stderr, status);
  }

  ///////////////////
  // Now parse the subsection and make sure all that works.
  
  std::vector<long> dim(numAxes);
  for(int i=0;i<numAxes;i++) dim[i] = dimAxes[i];
  delete [] dimAxes;
  
  if(this->pixelSec.parse(dim)==FAILURE) return FAILURE;
  if(this->statSec.parse(dim)==FAILURE)  return FAILURE;
  
  return SUCCESS;
}
