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
   * Read in a cube from the file fname (assumed to be in FITS format).
   *  Function is a front end to the I/O functions in the FitsIO/ directory.
   *  This function will check that the file exists, report the dimensions
   *   and then get other functions to read the data, WCS, and necessary 
   *   header keywords.
   *  \par fname A string with name of FITS file.
   *  \return SUCCESS or FAILURE.
   */

  long nelements;
  int bitpix,numAxes;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  int exists;
  fits_file_exists(fname.c_str(),&exists,&status);
  if(exists<=0){
    fits_report_error(stderr, status);
    std::stringstream errmsg;
    errmsg << "Requested image (" << fname << ") does not exist!\n";
    duchampError("getCube", errmsg.str());
    return FAILURE;
  }

  // Open the FITS file -- make sure it exists
  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    if(((status==URL_PARSE_ERROR)||(status==BAD_NAXIS))
       &&(this->pars().getFlagSubsection()))
      duchampError("getCube",
		   "It may be that the subsection you've entered is invalid.\n\
Either it has the wrong number of axes, or one axis has too large a range.\n");
    return FAILURE;
  }

  // Read the size information -- number and lengths of all axes.
  // Just use this for reporting purposes.
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
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  // Report the size of the FITS file
  std::cout << "Dimensions of FITS file: ";
  int dim = 0;
  std::cout << dimAxes[dim];
  while(dim+1<numAxes) std::cout << "x" << dimAxes[++dim];
  std::cout << std::endl;

  delete [] dimAxes;

  // Get the WCS information
  this->head.defineWCS(fname, this->par);

  if(!this->head.isWCS()) 
    duchampWarning("getCube","WCS is not good enough to be used.\n");

  // Get the data array from the FITS file.
  // Report the dimensions of the data array that was read (this can be
  //   different to the full FITS array).
  std::cerr << "Reading data ... ";
  this->getFITSdata(fname);
  std::cerr << "Done. Data array has dimensions: ";
  std::cerr << this->axisDim[0] <<"x" 
	    << this->axisDim[1] <<"x" 
	    << this->axisDim[2] << std::endl;

  // Read the necessary header information, and copy some of it into the Param.
  this->head.readHeaderInfo(fname, this->par);

  return SUCCESS;

}

