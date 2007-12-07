// -----------------------------------------------------------------------
// wcsIO.cc: Get the World Coordinate System for a particular FITS file.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
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
#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  int FitsHeader::defineWCS(std::string fname, Param &par)
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
    int numAxes;
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

    // Read in the entire PHU of the FITS file to a std::string.
    // This will be read by the wcslib functions to extract the WCS.
    status = 0;
    fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
    if( status ){
      duchampWarning("Cube Reader",
		     "Error whilst reading FITS header to string: ");
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // Close the FITS file -- not needed any more in this function.
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }
  
    // Define a temporary, local version of the WCS
    struct wcsprm *localwcs;
    localwcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
    localwcs->flag=-1;

    // Initialise this temporary wcsprm structure
    status = wcsini(true, numAxes, localwcs);
    if(status){
      std::stringstream errmsg;
      errmsg << "wcsini failed! Code=" << status
	     << ": " << wcs_errmsg[status] << std::endl;
      duchampError("Cube Reader",errmsg.str());
      return FAILURE;
    }

    this->naxis=0;
    for(int i=0;i<numAxes;i++)
      if(dimAxes[i]>1) this->naxis++;

    int relax=1; // for wcspih -- admit all recognised informal WCS extensions
    int ctrl=2;  // for wcspih -- report each rejected card and its reason for
    //               rejection
    int localnwcs, nreject;
    // Parse the FITS header to fill in the wcsprm structure
    status=wcspih(hdr, nkeys, relax, ctrl, &nreject, &localnwcs, &localwcs);
    if(status){
      // if here, something went wrong -- report what.
      std::stringstream errmsg;
      errmsg << "wcspih failed!\nWCSLIB error code=" << status
	     << ": " << wcs_errmsg[status] << std::endl;
      duchampWarning("Cube Reader",errmsg.str());
    }
    else{  
      int stat[NWCSFIX];
      // Applies all necessary corrections to the wcsprm structure
      //  (missing cards, non-standard units or spectral types, ...)
      status = wcsfix(1, (const int*)dimAxes, localwcs, stat);
      if(status) {
	std::stringstream errmsg;
	errmsg << "wcsfix failed:\n";
	errmsg << "Function status returns are:\n";
	for(int i=0; i<NWCSFIX; i++)
	  if (stat[i] > 0) 
	    errmsg << i+1 << ": WCSFIX error code=" << stat[i] << ": "
		   << wcsfix_errmsg[stat[i]] << std::endl;
	duchampWarning("Cube Reader", errmsg.str() );
      }

      // Set up the wcsprm struct. Report if something goes wrong.
      status = wcsset(localwcs);
      if(status){
	std::stringstream errmsg;
	errmsg<<"wcsset failed!\n"
	      <<"WCSLIB error code=" << status 
	      <<": "<<wcs_errmsg[status] << std::endl;
	duchampWarning("Cube Reader",errmsg.str());
      }

      if(localwcs->spec>=0){ //if there is a spectral axis

	int index = localwcs->spec;
	std::string desiredType,specType = localwcs->ctype[index];
	std::string shortType = specType.substr(0,4);
	if(shortType=="VELO" || shortType=="VOPT" || shortType=="ZOPT" 
	   || shortType=="VRAD" || shortType=="BETA"){
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
	    duchampWarning("Cube Reader",
			   "No rest frequency defined. Using frequency units in spectral axis.\n");
	    desiredType = duchampFrequencyType;
	    par.setSpectralUnits("MHz");
	    if(strcmp(localwcs->cunit[index],"")==0){
	      duchampWarning("Cube Reader",
			     "No frequency unit given. Assuming frequency axis is in Hz.\n");
	      strcpy(localwcs->cunit[index],"Hz");
	    }
	    this->spectralDescription = duchampSpectralDescription[FREQUENCY];
	  }
	}
	else {
	  desiredType = duchampFrequencyType;
	  par.setSpectralUnits("MHz");
	  if(strcmp(localwcs->cunit[index],"")==0){
	    duchampWarning("Cube Reader",
			   "No frequency unit given. Assuming frequency axis is in Hz.\n");
	    strcpy(localwcs->cunit[index],"Hz");
	  }
	  this->spectralDescription = duchampSpectralDescription[FREQUENCY];
	}	

	// Now we need to make sure the spectral axis has the correct setup.
	//  We use wcssptr to translate it if it is not of the desired type,
	//  or if the spectral units are not defined.

	bool needToTranslate = false;

	//       if(strncmp(specType.c_str(),desiredType.c_str(),4)!=0) 
	// 	needToTranslate = true;

	std::string blankstring = "";
	if(strcmp(localwcs->cunit[localwcs->spec],blankstring.c_str())==0)
	  needToTranslate = true;

	if(needToTranslate){

	  if(strcmp(localwcs->ctype[localwcs->spec],"VELO")==0)
	    strcpy(localwcs->ctype[localwcs->spec],"VELO-F2V");

	  index = localwcs->spec;
	
	  status = wcssptr(localwcs, &index, (char *)desiredType.c_str());
	  if(status){
	    std::stringstream errmsg;
	    errmsg<< "WCSSPTR failed! Code=" << status << ": "
		  << wcs_errmsg[status] << std::endl
		  << "(wanted to convert from type \"" << specType
		  << "\" to type \"" << desiredType << "\")\n";
	    duchampWarning("Cube Reader",errmsg.str());

	  }

	}
    
      } // end of if(localwcs->spec>=0)
    
      // Save the wcs to the FitsHeader class that is running this function
      this->setWCS(localwcs);
      this->setNWCS(localnwcs);

    }

    // clean up allocated memory
    wcsvfree(&localnwcs,&localwcs);
    wcsfree(localwcs);
    free(localwcs);
    free(hdr);
    delete [] dimAxes;

    // Get the brightness unit, so that we can set the units for the 
    //  integrated flux when we go to fixUnits.
    this->readBUNIT(fname);

    if(this->wcs->spec>=0) this->fixUnits(par);

    return SUCCESS;

  }

}
