#include <iostream>
#include <sstream>
#include <string>
#include <wcs.h>
#include <wcshdr.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine wtbarr 
                         // (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int Cube::readReconCube()
{
  /** 
   *  Cube::readReconCube()
   *   
   *   A way to read in a previously reconstructed array, corresponding to the 
   *    requested parameters, or in the filename given by reconFile.
   *   Performs various tests to make sure there are no clashes between the requested
   *    parameters and those stored in the header of the FITS file.
   */


  fitsfile *fptr;
  int status = 0;

  // Check to see whether the parameters reconFile and/or residFile are defined.
  bool reconGood = false;
  bool residGood = false;
  int exists;
  if(this->par.getReconFile() != ""){
    reconGood = true;
    fits_file_exists(this->par.getReconFile().c_str(),&exists,&status);
    if(exists<=0){
      fits_report_error(stderr, status);
      std::cerr << 
	"  WARNING <readReconCube> : Cannot find requested ReconFile. Trying with parameters.\n";
      std::cerr << 
	"                            Bad reconFile was: "<<this->par.getReconFile()<<std::endl;
      reconGood = false;
    }
  }
  else{
    std::cerr << 
      "  WARNING <readReconCube> : ReconFile not specified. Working out name from parameters.\n";
  }
  
  if(!reconGood){ // if bad, need to look at parameters

    string reconFile = this->par.outputReconFile();
    std::cerr << "                          : Trying file " << reconFile << std::endl;
    reconGood = true;
    fits_file_exists(reconFile.c_str(),&exists,&status);
    if(exists<=0){
      fits_report_error(stderr, status);
      std::cerr << 
	"  WARNING <readReconCube> : ReconFile not present\n";
      reconGood = false;
    }

    if(reconGood){ // were able to open this file -- use this, so reset the relevant parameter
      this->par.setReconFile(reconFile);
    }
    else { // if STILL bad, give error message and exit.
      std::cerr << "  ERROR <readReconCube> : Cannot find reconstructed file.\n";
      return FAILURE;
    }

  }

  // if we get to here, reconGood is true (ie. file is open);
  status=0;
  fits_open_file(&fptr,this->par.getReconFile().c_str(),READONLY,&status);
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
    std::cerr << "  ERROR <readReconCube> : "
	      << "  Reconstructed cube has a different number of axes to original! ("
	      << numAxesNew << " cf. " << this->numDim << ")\n";
    return FAILURE;
  }

  for(int i=0;i<numAxesNew;i++){
    if(dimAxesNew[i]!=this->axisDim[i]){
      std::cerr << "  ERROR <readReconCube> : "
		<< "  Reconstructed cube has different axis dimensions to original!\n"
		<< "        Axis #" << i << " has size " << dimAxesNew[i] 
		<< "   cf. " << this->axisDim[i] <<" in original.\n";	
      return FAILURE;
    }
  }      

  float *reconArray = new float[this->numPixels];
  //  fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, this->recon, &anynul, &status);
  status = 0;
  fits_read_pix(fptr, TFLOAT, fpixel, this->numPixels, NULL, reconArray, &anynul, &status);
  this->saveRecon(reconArray,this->numPixels);
  
  if(!this->par.getFlagATrous()){
    this->par.setFlagATrous(true);
    std::cerr << "  WARNING <readReconCube> : Setting flagAtrous from false to true, as the reconstruction exists.\n";
  }

  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    std::cerr << "  WARNING <readReconCube> : Error closing file: ";
    fits_report_error(stderr, status);
    //    return FAILURE;
  }

  // We don't want to write out the recon or resid files at the end
  this->par.setFlagOutputRecon(false);
  this->par.setFlagOutputResid(false);

  // The reconstruction is done -- set the appropriate flag
  this->reconExists = true;

  return SUCCESS;
}
