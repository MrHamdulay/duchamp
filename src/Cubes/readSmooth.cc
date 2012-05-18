// -----------------------------------------------------------------------
// readSmooth.cc: Read in an existing smoothed FITS file.
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
#include <sstream>
#include <string>
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try to redefine wtbarr
                         // (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  OUTCOME Cube::readSmoothCube()
  {
    ///  @details
    ///   A way to read in a previously smoothed array, corresponding 
    ///    to the requested parameters, or in the filename given by smoothFile.
    ///   Performs various tests to make sure there are no clashes between 
    ///    the requested parameters and those stored in the header of the 
    ///    FITS file. Also test to make sure that the size (and subsection, 
    ///    if applicable) of the array is the same.


    int status = 0;

    if(!this->par.getFlagSmoothExists()){
      DUCHAMPWARN("readSmoothCube","flagSmoothExists is not set. Not reading anything in!");
      return FAILURE;
    }
    else if(!this->par.getFlagSmooth()){
      DUCHAMPWARN("readSmoothCube", "flagSmooth is not set. Don't need to read in smoothed array!");
      return FAILURE;
    }
    else {

      // if kernMin is negative (not defined), make it equal to kernMaj
      if(this->par.getKernMin() < 0) 
	this->par.setKernMin(this->par.getKernMaj());

      // Check to see whether the parameter smoothFile is defined
      bool smoothGood = false;
      int exists;
      if(this->par.getSmoothFile() != ""){
	smoothGood = true;
	fits_file_exists(this->par.getSmoothFile().c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  DUCHAMPWARN("readSmoothCube", "Cannot find requested SmoothFile. Trying with parameters. Bad smoothFile was: "<<this->par.getSmoothFile());
	  smoothGood = false;
	}
      }
      else{
	DUCHAMPWARN("readSmoothCube", "SmoothFile not specified. Working out name from parameters.");
      }
  
      if(!smoothGood){ // if bad, need to look at parameters

	std::string smoothFile = this->par.outputSmoothFile();
	DUCHAMPWARN("readSmoothCube", "Trying file " << smoothFile );
	smoothGood = true;
	fits_file_exists(smoothFile.c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  // 	DUCHAMPWARN("readSmoothCube","SmoothFile not present.\n");
	  smoothGood = false;
	}

	if(smoothGood){ 
	  // were able to open this new file -- use this, so reset the 
	  //  relevant parameter
	  this->par.setSmoothFile(smoothFile);
	}
	else { // if STILL bad, give error message and exit.
	  DUCHAMPERROR("readSmoothCube","Cannot find Smoothed file.");
	  return FAILURE;
	}

      }

      // if we get to here, smoothGood is true (ie. file is open);

      status=0;
      fitsfile *fptr;
      fits_open_file(&fptr,this->par.getSmoothFile().c_str(),READONLY,&status);
      short int maxdim=3;
      long *fpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) fpixel[i]=1;
      long *dimAxesNew = new long[maxdim];
      for(int i=0;i<maxdim;i++) dimAxesNew[i]=1;
      int bitpix,numAxesNew,anynul;

      status = 0;
      fits_get_img_param(fptr,maxdim,&bitpix,&numAxesNew,dimAxesNew,&status);
      if(status){
	fits_report_error(stderr, status);
	return FAILURE;
      }

      if(numAxesNew != this->numDim){
	DUCHAMPERROR("readSmoothCube", "Smoothed cube has a different number of axes to original!" << " (" << numAxesNew << " cf. " << this->numDim << ")");
	return FAILURE;
      }

      for(int i=0;i<numAxesNew;i++){
	if(dimAxesNew[i]!=int(this->axisDim[i])){
	  DUCHAMPERROR("readSmoothCube", "Smoothed cube has different axis dimensions to original! Axis #" << i+1 << " has size " << dimAxesNew[i] << " cf. " << this->axisDim[i] <<" in original.");
	  return FAILURE;
	}
      }

      char *comment = new char[80];

      if(this->par.getFlagSubsection()){
	char *subsection = new char[80];
	status = 0;
	fits_read_key(fptr, TSTRING, (char *)keyword_subsection.c_str(), 
		      subsection, comment, &status);
	if(status){
	  DUCHAMPERROR("readSmoothCube", "subsection keyword not present in smoothFile.");
	  return FAILURE;
	}
	else{
	  if(this->par.getSubsection() != subsection){
	    DUCHAMPERROR("readSmoothCube", "subsection keyword in smoothFile (" << subsection << ") does not match that requested (" << this->par.getSubsection() << ").");
	    return FAILURE;
	  }
	}
	delete subsection;
      }

      if(this->par.getSmoothType()=="spectral"){

	int hannWidth;
	status = 0;
	fits_read_key(fptr, TINT, (char *)keyword_hanningwidth.c_str(), 
		      &hannWidth, comment, &status);
	if(hannWidth != this->par.getHanningWidth()){
	  DUCHAMPERROR("readSmoothCube", keyword_hanningwidth << " keyword in smoothFile (" << hannWidth << ") does not match the hanningWidth parameter (" << this->par.getHanningWidth() << ").");
	  return FAILURE;
	}

      }
      else if(this->par.getSmoothType()=="spatial"){

	float maj,min,pa;
	status = 0;
	fits_read_key(fptr, TFLOAT, (char *)keyword_kernmaj.c_str(), 
		      &maj, comment, &status);
	if(maj != this->par.getKernMaj()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernmaj << " keyword in smoothFile (" << maj << ") does not match the kernMaj parameter (" << this->par.getKernMaj() << ").");
	  return FAILURE;
	}
	status = 0;
	fits_read_key(fptr, TFLOAT, (char *)keyword_kernmin.c_str(), 
		      &min, comment, &status);
	if(min != this->par.getKernMin()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernmin << " keyword in smoothFile (" << maj << ") does not match the kernMin parameter (" << this->par.getKernMin() << ").");
	  return FAILURE;
	}
	status = 0;
	fits_read_key(fptr, TFLOAT, (char *)keyword_kernpa.c_str(), 
		      &pa, comment, &status);
	if(pa != this->par.getKernPA()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernpa << " keyword in smoothFile (" << maj << ") does not match the kernPA parameter (" << this->par.getKernPA() << ").");
	  return FAILURE;
	}

      }

      // Read the BUNIT keyword, and translate to standard unit format if needs be
      std::string header("BUNIT");
      char *unit = new char[FLEN_VALUE];
      std::string fluxunits;
      fits_read_key(fptr, TSTRING, (char *)header.c_str(), unit, comment, &status);
      if (status){
	DUCHAMPWARN("Cube Reader","Error reading BUNIT keyword: ");
	fits_report_error(stderr, status);
	return FAILURE;
      }
      else{
	wcsutrn(0,unit);
	fluxunits = unit;
      }
  
      //
      // If we get to here, the smoothFile exists and the smoothing
      // parameters match those requested.

      status = 0;
      fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, 
		    this->recon, &anynul, &status);
  
      status = 0;
      fits_close_file(fptr, &status);
      if (status){
	DUCHAMPWARN("readSmoothCube", "Error closing file: ");
	fits_report_error(stderr, status);
      }

      // We don't want to write out the smoothed files at the end
      this->par.setFlagOutputSmooth(false);

      this->convertFluxUnits(fluxunits,this->par.getNewFluxUnits(),RECON);

      // The reconstruction is done -- set the appropriate flag
      this->reconExists = true;

      return SUCCESS;
    }
  }

}
