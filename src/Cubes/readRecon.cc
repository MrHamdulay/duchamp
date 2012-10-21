// -----------------------------------------------------------------------
// readRecon.cc: Read in an existing wavelet-reconstructed FITS file.
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

  OUTCOME Cube::readReconCube()
  {
    ///  @details
    ///  A way to read in a previously reconstructed array, corresponding 
    ///    to the requested parameters, or in the filename given by reconFile.
    ///   Performs various tests to make sure there are no clashes between 
    ///    the requested parameters and those stored in the header of the 
    ///    FITS file. Also test to make sure that the size (and subsection, 
    ///    if applicable) of the array is the same.

    int status = 0;

    if(!this->par.getFlagReconExists()){
      DUCHAMPWARN("readReconCube", "reconExists flag is not set. Not reading anything in!");
      return FAILURE;
    }
    else if(!this->par.getFlagATrous()){
      DUCHAMPWARN("readReconCube","flagATrous is not set. Don't need to read in recon array!");
      return FAILURE;
    }
    else {

      // Check to see whether the parameters reconFile and/or residFile are defined
      bool reconGood = false;
      int exists;
      if(this->par.getReconFile() != ""){
	reconGood = true;
	fits_file_exists(this->par.getReconFile().c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  DUCHAMPWARN("readReconCube", "Cannot find requested ReconFile. Trying with parameters. Bad reconFile was: "<<this->par.getReconFile());
	  reconGood = false;
	}
      }
      else{
	DUCHAMPWARN("readReconCube", "ReconFile not specified. Working out name from parameters.");
      }
  
      if(!reconGood){ // if bad, need to look at parameters

	std::string reconFile = this->par.outputReconFile();
	DUCHAMPWARN("readReconCube", "Trying file " << reconFile );
	reconGood = true;
	fits_file_exists(reconFile.c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  // 	DUCHAMPWARNING("readReconCube","ReconFile not present.");
	  reconGood = false;
	}

	if(reconGood){ 
	  // were able to open this new file -- use this, so reset the 
	  //  relevant parameter
	  this->par.setReconFile(reconFile);
	}
	else { // if STILL bad, give error message and exit.
	  DUCHAMPERROR("readReconCube","Cannot find reconstructed file.");
	  return FAILURE;
	}

      }

      // if we get to here, reconGood is true (ie. file is open);

      // Identify which axes are the "interesting" ones 
      int lng,lat,spc;
      if(this->head.isWCS() && (this->head.WCS().spec>=0)){
	lng = this->head.WCS().lng;
	lat = this->head.WCS().lat;
	spc = this->head.WCS().spec;
      }
      else{
	lng = 0;
	lat = 1;
	spc = 2;
      }

      status=0;
      fitsfile *fptr;
      fits_open_file(&fptr,this->par.getReconFile().c_str(),READONLY,&status);
      short int maxdim=3;
      long *fpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) fpixel[i]=1;
      long *lpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) lpixel[i]=this->axisDim[i];
      long *inc = new long[maxdim];
      for(int i=0;i<maxdim;i++) inc[i]=1;
      long *dimAxesNew = new long[maxdim];
      for(int i=0;i<maxdim;i++) dimAxesNew[i]=1;
      int bitpix,numAxesNew,anynul;

      status = 0;
      fits_get_img_param(fptr, maxdim, &bitpix, &numAxesNew, dimAxesNew, &status);
      if(status){
	fits_report_error(stderr, status);
	return FAILURE;
      }

      if(numAxesNew != this->numDim){
	DUCHAMPERROR("readReconCube", "Reconstructed cube has a different number of axes to original!" << " (" << numAxesNew << " cf. " << this->numDim << ")");
	return FAILURE;
      }

      for(int i=0;i<numAxesNew;i++){
	if(dimAxesNew[i]!=int(this->axisDim[i])){
	  DUCHAMPERROR("readReconCube", "Reconstructed cube has different axis dimensions to original! Axis #" << i+1 << " has size " << dimAxesNew[i] << " cf. " << this->axisDim[i] <<" in original.");
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
	  DUCHAMPERROR("readReconCube", "subsection keyword not present in reconFile.");
	  return FAILURE;
	}
	else{
	  if(this->par.getSubsection() != subsection){
	    DUCHAMPERROR("readReconCube", "subsection keyword in reconFile (" << subsection << ") does not match that requested (" << this->par.getSubsection() << ").");
	    return FAILURE;
	  }
	}
	delete subsection;
      }

      unsigned int scaleMin;
      int filterCode,reconDim;
      float snrRecon;
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_reconDim.c_str(), 
		    &reconDim, comment, &status);
      if(reconDim != this->par.getReconDim()){
	DUCHAMPERROR("readReconCube", "reconDim keyword in reconFile (" << reconDim << ") does not match that requested (" << this->par.getReconDim() << ").");
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_filterCode.c_str(), 
		    &filterCode, comment, &status);
      if(filterCode != this->par.getFilterCode()){
	DUCHAMPERROR("readReconCube", "filterCode keyword in reconFile (" << filterCode << ") does not match that requested (" << this->par.getFilterCode() << ").");
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TFLOAT, (char *)keyword_snrRecon.c_str(), 
		    &snrRecon, comment, &status);
      if(snrRecon != this->par.getAtrousCut()){
	DUCHAMPERROR("readReconCube", "snrRecon keyword in reconFile (" << snrRecon << ") does not match that requested (" << this->par.getAtrousCut() << ").");
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_scaleMin.c_str(), 
		    &scaleMin, comment, &status);
      if(scaleMin != this->par.getMinScale()){
	DUCHAMPERROR("readReconCube", "scaleMin keyword in reconFile (" << scaleMin << ") does not match that requested (" << this->par.getMinScale() << ").");
	return FAILURE;
      }

      // Read the BUNIT keyword, and translate to standard unit format if needs be
      std::string header("BUNIT");
      char *unit = new char[FLEN_VALUE];
      std::string fluxunits;
      fits_read_key(fptr, TSTRING, (char *)header.c_str(), unit, comment, &status);
      if (status){
	DUCHAMPWARN("Recon Reader","Error reading BUNIT keyword: ");
	fits_report_error(stderr, status);
	return FAILURE;
      }
      else{
	wcsutrn(0,unit);
	fluxunits = unit;
      }
  
      //
      // If we get to here, the reconFile exists and matches the atrous 
      //  parameters the user has requested.

      int colnum = 0;  // want the first dataset in the FITS file
      status = 0;
      // fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, 
      // 		    this->recon, &anynul, &status);
      if(fits_read_subset_flt(fptr, colnum, 3, dimAxesNew,
			      fpixel, lpixel, inc, 
			      this->par.getBlankPixVal(), this->array, &anynul, &status)){
	DUCHAMPERROR("Recon Reader", "There was an error reading in the data array:");
	fits_report_error(stderr, status);
	return FAILURE;
      }

      status = 0;
      fits_close_file(fptr, &status);
      if (status){
	DUCHAMPWARN("readReconCube", "Error closing file: ");
	fits_report_error(stderr, status);
      }

      // We don't want to write out the recon or resid files at the end
      this->par.setFlagOutputRecon(false);
      this->par.setFlagOutputResid(false);

      this->convertFluxUnits(fluxunits,this->par.getNewFluxUnits(),RECON);

      // The reconstruction is done -- set the appropriate flag
      this->reconExists = true;

      return SUCCESS;
    }
  }

}
