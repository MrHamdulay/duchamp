#include <iostream>
#include <string>
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

int FitsHeader::defineWCS(string fname, Param &par)
{
  /**
   *  FitsHeader::defineWCS(wcsprm *wcs, char *hdr, Param &par) 
   *   A function that reads the WCS header information from the 
   *    FITS file given by fptr.
   *   It will also sort out the spectral axis, and covert to the correct 
   *    velocity type, or frequency type if need be.
   *   It calls FitsHeader::getBUNIT so that the Integrated Flux units can
   *    be calculated by FitsHeader::fixUnits.
   */

  fitsfile *fptr;
  int bitpix,numAxes;
  int noComments = 1; //so that fits_hdr2str will ignore COMMENT, HISTORY etc
  int nExc = 0,nkeys;
  char *hdr;

  // Open the FITS file
  int status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }

  // Get the dimensions of the FITS file -- number of axes and size of each.
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

  // Read in the entire PHU of the FITS file to a string.
  // This will be read by the wcslib functions to extract the WCS.
  status = 0;
  fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
  if( status ){
    duchampWarning("defineWCS","Error whilst reading FITS header to string: ");
    fits_report_error(stderr, status);
    return FAILURE;
  }

  // Close the FITS file -- not needed any more in this function.
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }
  
  wcsprm *wcs = new wcsprm;

  // Initialise the wcsprm structure
  int flag;
  if(flag = wcsini(true,numAxes,wcs)){
    std::stringstream errmsg;
    errmsg << "wcsini failed! Code=" << flag
	   << ": " << wcs_errmsg[flag] << std::endl;
    duchampError("defineWCS",errmsg.str());
    return FAILURE;
  }
  wcs->flag=-1;

  int relax=1; // for wcspih -- admit all recognised informal WCS extensions
  int ctrl=2;  // for wcspih -- report each rejected card and its reason for
               //               rejection
  int nwcs, nreject;
  // Parse the FITS header to fill in the wcsprm structure
  if(flag = wcspih(hdr, nkeys, relax, ctrl, &nreject, &nwcs, &wcs)) {
    // if here, something went wrong -- report what.
    std::stringstream errmsg;
    errmsg << "wcspih failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    duchampWarning("defineWCS",errmsg.str());
  }
  else{  
    int stat[6];
    int axes[3]={dimAxes[wcs->lng],dimAxes[wcs->lat],dimAxes[wcs->spec]};

    // Applies all necessary corrections to the wcsprm structure
    //  (missing cards, non-standard units or spectral types, ...)
    if(flag=wcsfix(1,axes,wcs,stat)) {
      std::stringstream errmsg;
      errmsg << "wcsfix failed:\n";
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) 
	  errmsg <<" flag="<<flag<<": "<< wcsfix_errmsg[stat[i]]<<std::endl;
      duchampWarning("defineWCS", errmsg.str() );
    }

    // Set up the wcsprm struct. Report if something goes wrong.
    if(flag=wcsset(wcs)){
      std::stringstream errmsg;
      errmsg<<"wcsset failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<std::endl;
      duchampWarning("defineWCS",errmsg.str());
    }

    if(wcs->naxis>2){  // if there is a spectral axis

      int index = wcs->spec;
      string desiredType,specType = wcs->ctype[index];
      if(wcs->restfrq != 0){
	// Set the spectral axis to a standard specification: VELO-F2V
	desiredType = duchampVelocityType;
	if(wcs->restwav == 0) wcs->restwav = 299792458.0 / wcs->restfrq;
      }
      else{
	// No rest frequency defined, so put spectral dimension in frequency. 
	// Set the spectral axis to a standard specification: FREQ
	duchampWarning("defineWCS",
       "No rest frequency defined. Using frequency units in spectral axis.\n");
	desiredType = duchampFrequencyType;
 	par.setSpectralUnits("MHz");
	if(strcmp(wcs->cunit[index],"")==0){
	  duchampWarning("defineWCS",
	  "No frequency unit given. Assuming frequency axis is in Hz.\n");
	  strcpy(wcs->cunit[index],"Hz");
	}
      }

      // Check to see if the spectral type (eg VELO-F2V) matches that wanted
      //   from duchamp.hh. Only first four characters checked.
      if(strncmp(specType.c_str(),desiredType.c_str(),4)!=0){
	index = -1;
	// If not a match, translate the spectral axis to the desired type
	if( flag = wcssptr(wcs, &index, (char *)desiredType.c_str())){
	  std::stringstream errmsg;
	  errmsg<<"wcssptr failed! Code="<<flag <<": "
		<<wcs_errmsg[flag]<<std::endl;
	  duchampWarning("defineWCS",errmsg.str());
	}	
      }
    
    } // end of if(numAxes>2)
    
    // Save the wcs to the FitsHeader class that is running this function
    this->setWCS(wcs);
    this->setNWCS(nwcs);

    wcsfree(wcs);

  }

  // Get the brightness unit, so that we can set the units for the 
  //  integrated flux when we go to fixUnits.

  this->getBUNIT(fname);

  this->fixUnits(par);

  return SUCCESS;

}
