#include <iostream>
#include <string>
#include <stdlib.h>
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
   *   A function that reads the WCS header information from the 
   *    FITS file given by fname
   *   It will also sort out the spectral axis, and covert to the correct 
   *    velocity type, or frequency type if need be.
   *   It calls FitsHeader::readBUNIT so that the Integrated Flux units can
   *    be calculated by FitsHeader::fixUnits.
   * \param fname Fits file to read.
   * \param par Param set to help fix the units with.
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
    duchampWarning("defineWCS","Error closing file: ");
    fits_report_error(stderr, status);
  }
  
  struct wcsprm *localwcs;
  localwcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  localwcs->flag=-1;

  // Initialise the wcsprm structure
  if(status = wcsini(true, numAxes, localwcs)){
    std::stringstream errmsg;
    errmsg << "wcsini failed! Code=" << status
	   << ": " << wcs_errmsg[status] << std::endl;
    duchampError("defineWCS",errmsg.str());
    return FAILURE;
  }

  int relax=1; // for wcspih -- admit all recognised informal WCS extensions
  int ctrl=2;  // for wcspih -- report each rejected card and its reason for
               //               rejection
  int localnwcs, nreject;
  // Parse the FITS header to fill in the wcsprm structure
  if(status=wcspih(hdr, nkeys, relax, ctrl, &nreject, &localnwcs, &localwcs)){
    // if here, something went wrong -- report what.
    std::stringstream errmsg;
    errmsg << "wcspih failed!\nWCSLIB error code=" << status
	   << ": " << wcs_errmsg[status] << std::endl;
    duchampWarning("defineWCS",errmsg.str());
  }
  else{  
    int stat[NWCSFIX];
    // Applies all necessary corrections to the wcsprm structure
    //  (missing cards, non-standard units or spectral types, ...)
    if(status = wcsfix(1, (const int*)dimAxes, localwcs, stat)) {
      std::stringstream errmsg;
      errmsg << "wcsfix failed:\n";
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) 
	  errmsg << "WCSLIB error code=" << status << ": "
		 << wcsfix_errmsg[stat[i]] << std::endl;
      duchampWarning("defineWCS", errmsg.str() );
    }

    // Set up the wcsprm struct. Report if something goes wrong.
    if(status = wcsset(localwcs)){
      std::stringstream errmsg;
      errmsg<<"wcsset failed!\n"
	    <<"WCSLIB error code=" << status 
	    <<": "<<wcs_errmsg[status] << std::endl;
      duchampWarning("defineWCS",errmsg.str());
    }

    if(localwcs->naxis>2){  // if there is a spectral axis

      int index = localwcs->spec;
      string desiredType,specType = localwcs->ctype[index];
// 	      << "  " << localwcs->ctype[localwcs->spec] << "\n";
      if(localwcs->restfrq != 0){
	// Set the spectral axis to a standard specification: VELO-F2V
	desiredType = duchampVelocityType;
	if(localwcs->restwav == 0) 
	  localwcs->restwav = 299792458.0 /  localwcs->restfrq;
	this->spectralDescription = duchampSpectralDescription[VELOCITY];
      }
      else{
	// No rest frequency defined, so put spectral dimension in frequency. 
	// Set the spectral axis to a standard specification: FREQ
	duchampWarning("defineWCS",
       "No rest frequency defined. Using frequency units in spectral axis.\n");
	desiredType = duchampFrequencyType;
 	par.setSpectralUnits("MHz");
	if(strcmp(localwcs->cunit[index],"")==0){
	  duchampWarning("defineWCS",
	  "No frequency unit given. Assuming frequency axis is in Hz.\n");
	  strcpy(localwcs->cunit[index],"Hz");
	}
	this->spectralDescription = duchampSpectralDescription[FREQUENCY];
      }


      bool needToTranslate = false;

      if(strncmp(specType.c_str(),desiredType.c_str(),4)!=0) 
	needToTranslate = true;

      string blankstring = "";
      if(strcmp(localwcs->cunit[localwcs->spec],blankstring.c_str())==0)
	needToTranslate = true;

      if(needToTranslate){

	if(strcmp(localwcs->ctype[localwcs->spec],"VELO")==0)
	  strcpy(localwcs->ctype[localwcs->spec],"VELO-F2V");

	index = localwcs->spec;
	
	if(status = wcssptr(localwcs, &index, (char *)desiredType.c_str())){
	  std::stringstream errmsg;
	  errmsg<< "WCSSPTR failed! Code=" << status << ": "
		<< wcs_errmsg[status] << std::endl
		<< "(wanted to convert from type \"" << specType
		<< "\" to type \"" << desiredType << "\")\n";
	  duchampWarning("defineWCS",errmsg.str());

	}

      }

      /*

      // Check to see if the spectral type (eg VELO-F2V) matches that wanted
      //   from duchamp.hh. Only first four characters checked.

      char *blankstring;
      strcpy(blankstring,"");
      if(strncmp(specType.c_str(),desiredType.c_str(),4)!=0){

// 	index = -1;
	index = localwcs->spec;
	// If not a match, translate the spectral axis to the desired type
	if(status = wcssptr(localwcs, &index, (char *)desiredType.c_str())){
	  std::stringstream errmsg;
	  errmsg<< "WCSSPTR failed! Code=" << status << ": "
		<< wcs_errmsg[status] << std::endl
		<< "(wanted to convert from type \"" << specType
		<< "\" to type \"" << desiredType << "\")\n";
	  duchampWarning("defineWCS",errmsg.str());
	}	

      }
      else if(strcmp(localwcs->cunit[localwcs->spec],blankstring)==0){
	if(strcmp(localwcs->ctype[localwcs->spec],"VELO")==0)
	  strcpy(localwcs->ctype[localwcs->spec],"VELO-F2V");
	string tempType = "FREQ-F2V";
	std::stringstream errmsg;
	status=0;
	index = localwcs->spec;
// 	if(status=wcssptr(localwcs, &index, (char *)tempType.c_str())){
// 	  errmsg<< "First one. Code=" << status << ": "
// 		<< wcs_errmsg[status] << std::endl
// 		<< "(wanted to convert from type \"" 
// 		<< localwcs->ctype[localwcs->spec]
// 		<< "\" to type \"" << tempType << "\")\n";
// 	  duchampWarning("defineWCS 1", errmsg.str());
// 	}
// 	std::cerr<<"after #1: type = " << localwcs->ctype[localwcs->spec] 
// 		 << ", units = " << localwcs->cunit[localwcs->spec] << "\n";
	status=0;
	index = localwcs->spec;
	errmsg.str("");
	if(status=wcssptr(localwcs, &index, (char *)desiredType.c_str())){
	  errmsg<< "Second one. Code=" << status << ": "
		<< wcs_errmsg[status] << std::endl
		<< "(wanted to convert from type \"" 
		<< localwcs->ctype[localwcs->spec]
		<< "\" to type \"" << desiredType << "\")\n";
	  duchampWarning("defineWCS 2", errmsg.str());
	}
// 	std::cerr<<"after #2: type = " << localwcs->ctype[localwcs->spec] 
// 		 << ", units = " << localwcs->cunit[localwcs->spec] << "\n";
      }

      */

    
    } // end of if(numAxes>2)
    
    // Save the wcs to the FitsHeader class that is running this function
    this->setWCS(localwcs);
    this->setNWCS(localnwcs);

  }

  wcsvfree(&localnwcs,&localwcs);
  delete [] dimAxes;

  // Get the brightness unit, so that we can set the units for the 
  //  integrated flux when we go to fixUnits.
  this->readBUNIT(fname);

  this->fixUnits(par);

  return SUCCESS;

}
