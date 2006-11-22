#include <iostream>
#include <sstream>
#include <string>
#include <wcs.h>
#include <wcshdr.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try to redefine wtbarr
                         // (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int Cube::readSmoothCube()
{
  /** 
   *  Cube::readSmoothCube()
   *   
   *   A way to read in a previously Hanning-smoothed array, corresponding 
   *    to the requested parameters, or in the filename given by smoothFile.
   *   Performs various tests to make sure there are no clashes between 
   *    the requested parameters and those stored in the header of the 
   *    FITS file. Also test to make sure that the size (and subsection, 
   *    if applicable) of the array is the same.
   */


  int status = 0;

  if(!this->par.getFlagSmoothExists()){
    duchampWarning("readSmoothCube",
		   "flagSmoothExists is not set. Not reading anything in!\n");
    return FAILURE;
  }
  else if(!this->par.getFlagSmooth()){
    duchampWarning("readSmoothCube",
		   "flagSmooth is not set. Don't need to read in smoothed array!\n");
    return FAILURE;
  }
  else {

    // Check to see whether the parameter smoothFile is defined
    bool smoothGood = false;
    int exists;
    std::stringstream errmsg;
    if(this->par.getSmoothFile() != ""){
      smoothGood = true;
      fits_file_exists(this->par.getSmoothFile().c_str(),&exists,&status);
      if(exists<=0){
	fits_report_error(stderr, status);
	errmsg<< "Cannot find requested SmoothFile. Trying with parameters.\n"
	      << "Bad smoothFile was: "<<this->par.getSmoothFile() <<std::endl;
	duchampWarning("readSmoothCube", errmsg.str());
	smoothGood = false;
      }
    }
    else{
      errmsg<< "SmoothFile not specified. Working out name from parameters.\n";
    }
  
    if(!smoothGood){ // if bad, need to look at parameters

      string smoothFile = this->par.outputSmoothFile();
      errmsg << "Trying file " << smoothFile << std::endl;
      duchampWarning("readSmoothCube", errmsg.str() );
      smoothGood = true;
      fits_file_exists(smoothFile.c_str(),&exists,&status);
      if(exists<=0){
	fits_report_error(stderr, status);
	duchampWarning("readSmoothCube","SmoothFile not present.\n");
	smoothGood = false;
      }

      if(smoothGood){ 
	// were able to open this new file -- use this, so reset the 
	//  relevant parameter
	this->par.setSmoothFile(smoothFile);
      }
      else { // if STILL bad, give error message and exit.
	duchampError("readSmoothCube","Cannot find Hanning-smoothed file.\n");
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
    long nelements;
    int bitpix,numAxesNew,anynul;

    status = 0;
    fits_get_img_param(fptr, maxdim, &bitpix, &numAxesNew, dimAxesNew, &status);
    if(status){
      fits_report_error(stderr, status);
      return FAILURE;
    }

    if(numAxesNew != this->numDim){
      std::stringstream errmsg;
      errmsg << "Smoothed cube has a different number of axes to original!"
	     << " (" << numAxesNew << " cf. " << this->numDim << ")\n";
      duchampError("readSmoothCube", errmsg.str());
      return FAILURE;
    }

    for(int i=0;i<numAxesNew;i++){
      if(dimAxesNew[i]!=this->axisDim[i]){
	std::stringstream errmsg;
	errmsg << "Smoothed cube has different axis dimensions to original!"
	       << "\nAxis #" << i+1 << " has size " << dimAxesNew[i] 
	       << " cf. " << this->axisDim[i] <<" in original.\n";	
	duchampError("readSmoothCube", errmsg.str());
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
	duchampError("readSmoothCube", 
		     "subsection keyword not present in smoothFile.\n");
	return FAILURE;
      }
      else{
	if(this->par.getSubsection() != subsection){
	  std::stringstream errmsg;
	  errmsg << "subsection keyword in smoothFile (" << subsection 
		 << ") does not match that requested (" 
		 << this->par.getSubsection() << ").\n";
	  duchampError("readSmoothCube", errmsg.str());
	  return FAILURE;
	}
      }
      delete subsection;
    }

    int hannWidth;
    status = 0;
    fits_read_key(fptr, TINT, (char *)keyword_hanningwidth.c_str(), 
		  &hannWidth, comment, &status);
    if(hannWidth != this->par.getHanningWidth()){
      std::stringstream errmsg;
      errmsg << "hanningWidth keyword in smoothFile (" << hannWidth
	     << ") does not match that requested (" 
	     << this->par.getHanningWidth() << ").\n";
      duchampError("readSmoothCube", errmsg.str());
      return FAILURE;
    }

    //
    // If we get to here, the smoothFile exists and the hanningWidth
    //  parameter matches that requested.

    status = 0;
    fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, 
		  this->recon, &anynul, &status);
  
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("readSmoothCube", "Error closing file: ");
      fits_report_error(stderr, status);
    }

    // We don't want to write out the smoothed files at the end
    this->par.setFlagOutputSmooth(false);

    // The reconstruction is done -- set the appropriate flag
    this->reconExists = true;

    return SUCCESS;
  }
}
