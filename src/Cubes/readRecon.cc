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
#define WCSLIB_GETWCSTAB // define this so that we don't try to redefine wtbarr
                         // (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  int Cube::readReconCube()
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
      duchampWarning("readReconCube",
		     "reconExists flag is not set. Not reading anything in!\n");
      return FAILURE;
    }
    else if(!this->par.getFlagATrous()){
      duchampWarning("readReconCube",
		     "flagATrous is not set. Don't need to read in recon array!\n");
      return FAILURE;
    }
    else {

      // Check to see whether the parameters reconFile and/or residFile are defined
      bool reconGood = false;
      int exists;
      std::stringstream errmsg;
      if(this->par.getReconFile() != ""){
	reconGood = true;
	fits_file_exists(this->par.getReconFile().c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  errmsg<< "Cannot find requested ReconFile. Trying with parameters.\n"
		<< "Bad reconFile was: "<<this->par.getReconFile() << std::endl;
	  duchampWarning("readReconCube", errmsg.str());
	  reconGood = false;
	}
      }
      else{
	errmsg<< "ReconFile not specified. Working out name from parameters.\n";
      }
  
      if(!reconGood){ // if bad, need to look at parameters

	std::string reconFile = this->par.outputReconFile();
	errmsg << "Trying file " << reconFile << std::endl;
	duchampWarning("readReconCube", errmsg.str() );
	reconGood = true;
	fits_file_exists(reconFile.c_str(),&exists,&status);
	if(exists<=0){
	  fits_report_error(stderr, status);
	  // 	duchampWarning("readReconCube","ReconFile not present.\n");
	  reconGood = false;
	}

	if(reconGood){ 
	  // were able to open this new file -- use this, so reset the 
	  //  relevant parameter
	  this->par.setReconFile(reconFile);
	}
	else { // if STILL bad, give error message and exit.
	  duchampError("readReconCube","Cannot find reconstructed file.\n");
	  return FAILURE;
	}

      }

      // if we get to here, reconGood is true (ie. file is open);

      status=0;
      fitsfile *fptr;
      fits_open_file(&fptr,this->par.getReconFile().c_str(),READONLY,&status);
      short int maxdim=3;
      long *fpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) fpixel[i]=1;
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
	std::stringstream errmsg;
	errmsg << "Reconstructed cube has a different number of axes to original!"
	       << " (" << numAxesNew << " cf. " << this->numDim << ")\n";
	duchampError("readReconCube", errmsg.str());
	return FAILURE;
      }

      for(int i=0;i<numAxesNew;i++){
	if(dimAxesNew[i]!=this->axisDim[i]){
	  std::stringstream errmsg;
	  errmsg << "Reconstructed cube has different axis dimensions to original!"
		 << "\nAxis #" << i+1 << " has size " << dimAxesNew[i] 
		 << " cf. " << this->axisDim[i] <<" in original.\n";	
	  duchampError("readReconCube", errmsg.str());
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
	  duchampError("readReconCube", 
		       "subsection keyword not present in reconFile.\n");
	  return FAILURE;
	}
	else{
	  if(this->par.getSubsection() != subsection){
	    std::stringstream errmsg;
	    errmsg << "subsection keyword in reconFile (" << subsection 
		   << ") does not match that requested (" 
		   << this->par.getSubsection() << ").\n";
	    duchampError("readReconCube", errmsg.str());
	    return FAILURE;
	  }
	}
	delete subsection;
      }

      int scaleMin,filterCode,reconDim;
      float snrRecon;
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_reconDim.c_str(), 
		    &reconDim, comment, &status);
      if(reconDim != this->par.getReconDim()){
	std::stringstream errmsg;
	errmsg << "reconDim keyword in reconFile (" << reconDim
	       << ") does not match that requested (" 
	       << this->par.getReconDim() << ").\n";
	duchampError("readReconCube", errmsg.str());
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_filterCode.c_str(), 
		    &filterCode, comment, &status);
      if(filterCode != this->par.getFilterCode()){
	std::stringstream errmsg;
	errmsg << "filterCode keyword in reconFile (" << filterCode
	       << ") does not match that requested (" 
	       << this->par.getFilterCode() << ").\n";
	duchampError("readReconCube", errmsg.str());
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TFLOAT, (char *)keyword_snrRecon.c_str(), 
		    &snrRecon, comment, &status);
      if(snrRecon != this->par.getAtrousCut()){
	std::stringstream errmsg;
	errmsg << "snrRecon keyword in reconFile (" << snrRecon
	       << ") does not match that requested (" 
	       << this->par.getAtrousCut() << ").\n";
	duchampError("readReconCube", errmsg.str());
	return FAILURE;
      }
      status = 0;
      fits_read_key(fptr, TINT, (char *)keyword_scaleMin.c_str(), 
		    &scaleMin, comment, &status);
      if(scaleMin != this->par.getMinScale()){
	std::stringstream errmsg;
	errmsg << "scaleMin keyword in reconFile (" << scaleMin
	       << ") does not match that requested (" 
	       << this->par.getMinScale() << ").\n";
	duchampError("readReconCube", errmsg.str());
	return FAILURE;
      }
  
      //
      // If we get to here, the reconFile exists and matches the atrous 
      //  parameters the user has requested.

      status = 0;
      fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, 
		    this->recon, &anynul, &status);
  
      status = 0;
      fits_close_file(fptr, &status);
      if (status){
	duchampWarning("readReconCube", "Error closing file: ");
	fits_report_error(stderr, status);
      }

      // We don't want to write out the recon or resid files at the end
      this->par.setFlagOutputRecon(false);
      this->par.setFlagOutputResid(false);

      // The reconstruction is done -- set the appropriate flag
      this->reconExists = true;

      return SUCCESS;
    }
  }

}
