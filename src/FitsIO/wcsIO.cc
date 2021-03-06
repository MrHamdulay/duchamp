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
#include <string.h>
#include <stdlib.h>
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/fitshdr.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  OUTCOME FitsHeader::defineWCS(std::string fname, Param &par)
  {

    OUTCOME returnValue=SUCCESS;
    fitsfile *fptr;
    int status = 0;
    // Open the FITS file
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      returnValue = FAILURE;
    }
    else {

      returnValue = this->defineWCS(fptr,par);

      // Close the FITS file 
      status = 0;
      fits_close_file(fptr, &status);
      if (status){
	DUCHAMPWARN("Cube Reader","Error closing file: ");
	fits_report_error(stderr, status);
      }
 
    }
    return returnValue;

  }

  OUTCOME FitsHeader::defineWCS(Param &par)
  {
    OUTCOME returnValue=SUCCESS;
    if(this->fptr == 0) {
      returnValue = FAILURE;
      DUCHAMPERROR("Cube Reader", "FITS file not opened.");
    }
    else {
      returnValue = this->defineWCS(this->fptr, par);
    }
    return returnValue;
  }



  OUTCOME FitsHeader::defineWCS(fitsfile *fptr, Param &par)
  {
    ///   A function that reads the WCS header information from the 
    ///    FITS file given by fname
    ///   It will also sort out the spectral axis, and covert to the correct 
    ///    velocity type, or frequency type if need be.
    /// \param fname Fits file to read.
    /// \param par Param set to help fix the units with.

    OUTCOME returnValue=SUCCESS;

    int numAxes=0;
    size_t *dimAxes = 0;
    int noComments = 1; //so that fits_hdr2str will ignore COMMENT, HISTORY etc
    int nExc = 0,nkeys;
    int status = 0;
    char *hdr=0;
    struct wcsprm *localwcs=NULL;
    int localnwcs;

    // Get the dimensions of the FITS file -- number of axes and size of each.
    status = 0;
    if(fits_get_img_dim(fptr, &numAxes, &status)){
      fits_report_error(stderr, status);
      returnValue = FAILURE;
    }

    if(returnValue == SUCCESS){
      dimAxes = new size_t[numAxes];
      for(int i=0;i<numAxes;i++) dimAxes[i]=1;
      status = 0;
      if(fits_get_img_size(fptr, numAxes, (long*)dimAxes, &status)){
	fits_report_error(stderr, status);
	returnValue = FAILURE;
      }
    }

    if(returnValue == SUCCESS){
      // Read in the entire PHU of the FITS file to a std::string.
      // This will be read by the wcslib functions to extract the WCS.
      status = 0;
      fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
      if( status ){
	DUCHAMPWARN("Cube Reader","Error whilst reading FITS header to string: ");
	fits_report_error(stderr, status);
	return FAILURE;
      }
    }
 
    if(returnValue == SUCCESS){
      // Define a temporary, local version of the WCS
      localwcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
      localwcs->flag=-1;
      
      // Initialise this temporary wcsprm structure
      status = wcsini(true, numAxes, localwcs);
      if(status){
	DUCHAMPERROR("Cube Reader","wcsini failed! Code=" << status << ": " << wcs_errmsg[status]);
	returnValue = FAILURE;
      }
      else{
	int relax=1; // for wcspih -- admit all recognised informal WCS extensions
	int ctrl=2;  // for wcspih -- report each rejected card and its reason for rejection
	int nreject;
	// Parse the FITS header to fill in the wcsprm structure
	status=wcspih(hdr, nkeys, relax, ctrl, &nreject, &localnwcs, &localwcs);
	if(status){
	  // if here, something went wrong -- report what.
	  DUCHAMPERROR("Cube Reader","Could not parse header with wcspih!\nWCSLIB error code=" << status << ": " << wcs_errmsg[status]);
	  returnValue = FAILURE;
	}
	else  returnValue = this->defineWCS(localwcs, localnwcs, dimAxes, par);
	
      }
    }

    // clean up allocated memory
    if(localwcs > 0){
      wcsvfree(&localnwcs,&localwcs);
      wcsfree(localwcs);
      free(localwcs);
    }
    if(hdr>0) free(hdr);
    if(dimAxes>0) {
      delete [] dimAxes;
    }

    return returnValue;

  }

  OUTCOME FitsHeader::defineWCS(wcsprm *theWCS, int nWCS, size_t *dimAxes, Param &par)
  {
  
    int status=0;
    int stat[NWCSFIX];
    // Applies all necessary corrections to the wcsprm structure
    //  (missing cards, non-standard units or spectral types, ...)
    status = wcsfix(1, (const int*)dimAxes, theWCS, stat);
    if(status) {
      std::stringstream errmsg;
      errmsg << "wcsfix failed with function status returns of:\n";
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) 
	  errmsg << i+1 << ": WCSFIX error code=" << stat[i] << ": "
		 << wcsfix_errmsg[stat[i]] << std::endl;
      DUCHAMPWARN("Cube Reader", errmsg.str());
      return FAILURE;
    }
    // Set up the wcsprm struct. Report if something goes wrong.
    status = wcsset(theWCS);
    if(status){
      DUCHAMPWARN("Cube Reader","wcsset failed with error code=" << status <<": "<<wcs_errmsg[status]);
      return FAILURE;
    }
    else{

      int stat[NWCSFIX];
      // Re-do the corrections to account for things like NCP projections
      status = wcsfix(1, (const int*)dimAxes, theWCS, stat);
      if(status) {
	std::stringstream errmsg;
	errmsg << "wcsfix failed with function status returns of:\n";
	for(int i=0; i<NWCSFIX; i++)
	  if (stat[i] > 0) 
	    errmsg << i+1 << ": WCSFIX error code=" << stat[i] << ": "
		   << wcsfix_errmsg[stat[i]] << std::endl;
	DUCHAMPWARN("Cube Reader", errmsg.str() );
      }


      char stype[5],scode[5],sname[22],units[8],ptype,xtype;
      int restreq;

      if(par.getSpectralType()!=""){ // User wants to convert the spectral type

	std::string desiredType = par.getSpectralType();
	  
	status = spctyp((char *)desiredType.c_str(),stype,scode,sname,units,&ptype,&xtype,&restreq);
	if(status){
	  DUCHAMPERROR("Cube Reader", "Spectral type " << desiredType << " is not a valid spectral type. No translation done.");
	}
	else{

	  if(desiredType.size()<4){
	    DUCHAMPERROR("Cube Reader", "Spectral Type " << desiredType << " requested, but this is too short. No translation done");
	  }
	  else{
	    if(desiredType.size()<8){
	      if(desiredType.size()==4) desiredType+="-???";
	      else while (desiredType.size()<8) desiredType+="?";
	    }
	  }
	  if(par.getRestFrequency()>0.){
	    theWCS->restfrq = par.getRestFrequency();
	  }
	  status = wcssptr(theWCS, &(theWCS->spec), (char *)desiredType.c_str());
	  if(status){
	    DUCHAMPWARN("Cube Reader","WCSSPTR failed when converting from type \"" << theWCS->ctype[theWCS->spec] 
			<< "\" to type \"" << desiredType << " with code=" << status << ": " << wcs_errmsg[status]);
	  }
	  else if(par.getRestFrequency()>0.) par.setFlagRestFrequencyUsed(true);


	}

      }

      status = spctyp(theWCS->ctype[theWCS->spec],stype,scode,sname,units,&ptype,&xtype,&restreq);

      this->spectralType  = stype;
      this->spectralDescription = sname;

      // Save the wcs to the FitsHeader class that is running this function
      this->setWCS(theWCS);
      this->setNWCS(nWCS);
 
      // Now that the WCS is defined, use it to set the offsets in the Param set
      par.setOffsets(theWCS);

    }

    // work out whether the array is 2dimensional
    if(theWCS->naxis==2) this->flag2D = true;
    else{
      int numDim=0;
      for(int i=0;i<wcs->naxis;i++) if(dimAxes[i]>1) numDim++;
      this->flag2D = (numDim==2);
    }
   
    return SUCCESS;

    
  }


}
