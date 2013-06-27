// -----------------------------------------------------------------------
// ReadAndSearch.cc: Combining both the FITS reading and the searching.
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
#include <iomanip>
#include <string>
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/fitshdr.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and
                         // redefine wtbarr (this is a problem when
                         // using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/Utils/utils.hh>

std::string imageType[4] = {"point", "spectrum", "image", "cube"};

vector<Detection> readAndSearch(Param &par)
{
  vector<Detection> outputList;

  short int maxdim=3;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  std::string fname = par.getImageFile();
  if(par.getFlagSubsection()){
    par.parseSubsection();
    fname+=par.getSubsection();
  }

  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
  }

  status = 0;
  fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
  if(status){
    fits_report_error(stderr, status);
  }
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    DUCHAMPWARN("readAndSearch","Error closing file: ");
    fits_report_error(stderr, status);
  }

  if(numAxes<=3)  
    std::cout << "Dimensions of " << imageType[numAxes] << ": " << dimAxes[0];
  else std::cout << "Dimensions of " << imageType[3] << ": " << dimAxes[0];
  if(numAxes>1) std::cout << "x" << dimAxes[1];
  if(numAxes>2) std::cout << "x" << dimAxes[2];
  std::cout << std::endl;

  int anynul;
  long *fpixel = new long[numAxes];
  long *lpixel = new long[numAxes];
  long *inc = new long[numAxes];    // the data-sampling increment
  for(int i=0;i<numAxes;i++) inc[i]=1;

  //---------------------------------------------------------
  //  Read in spectra one at a time, reconstruct if necessary
  //   and then search, adding found objects to cube's object list.

  fpixel[0] = 1;
  for(int i=2;i<numAxes;i++) fpixel[i] = 1;

  lpixel[0] = dimAxes[0];
  lpixel[2] = dimAxes[2];
  for(int i=3;i<numAxes;i++) lpixel[i] = 1;

  int numDetected = 0;
  
  std::cout << "   0: |";
  printSpace(50);
  std::cout << "| " << std::setw(5) << numDetected << std::flush;
  printBackSpace(64);
  std::cout << std::flush;

  float *bigarray = new float[dimAxes[0] * dimAxes[2]];
  float *array = new float[dimAxes[2]];
  long *specdim = new long[2];
  specdim[0] = dimAxes[2]; specdim[1]=1;
 
  float median,sigma;
  size_t pos;
  int frac;

  Image *spectrum = new Image(specdim);
  spectrum->saveParam(par);
  spectrum->pars().setBeamSize(2.); // only neighbouring channels correlated

  for(int y=0;y<dimAxes[1];y++){

    std::cout << std::setw(4) << y << ": " << std::flush;

    fpixel[1] = lpixel[1] = y+1;  
 
    status = 0;
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      DUCHAMPWARN("readAndSearch","Error opening file: ");
      fits_report_error(stderr, status);
    }
    status = 0;
    fits_read_subset(fptr, TFLOAT, fpixel, lpixel, inc, NULL, bigarray, &anynul, &status);
    if(status){
      DUCHAMPERROR("readAndSearch","There was an error reading in the data array:");
      fits_report_error(stderr, status);
    }
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      DUCHAMPWARN("readAndSearch","Error closing file: ");
      fits_report_error(stderr, status);
    }

    for(int x=0;x<dimAxes[0];x++){

      pos = y*dimAxes[0]+x;
      frac = 100*(pos+1)/(dimAxes[1]*dimAxes[0]);
      if(frac%2 == 0){
	std::cout << "|";
	printHash(frac/2);
	printSpace(50-frac/2);
	std::cout << "| " << std::setw(5) << numDetected;
	printBackSpace(58);
	std::cout << std::flush;
      }
 
      for(int z=0;z<dimAxes[2];z++) array[z] = bigarray[z*dimAxes[0] + x];
 
      median = findMedian(array,dimAxes[2]);
      spectrum->clearDetectionList();
      if(par.getFlagATrous()){
	float *recon = new float[dimAxes[2]];
	atrous1DReconstruct(dimAxes[2],array,recon,par);
	float *resid = new float[dimAxes[2]];
	for(int i=0;i<dimAxes[2];i++) resid[i] = array[i] - recon[i];
	sigma  = findMADFM(recon,dimAxes[2])/correctionFactor;
	spectrum->saveArray(recon,dimAxes[2]);
	delete [] recon;
	delete [] resid; 
	spectrum->setStats(median,sigma,par.getCut());
	if(par.getFlagFDR()) spectrum->setupFDR();
	spectrum->setMinSize(par.getMinChannels());
	spectrum->findSources1D();
      }
      else{
	sigma  = findMADFM(array,dimAxes[2])/correctionFactor;
	spectrum->saveArray(array,dimAxes[2]);
	spectrum->setStats(median,sigma,par.getCut());
	if(par.getFlagFDR()) spectrum->setupFDR();
	spectrum->setMinSize(par.getMinChannels());
	spectrum->findSources1D();
      }

      numDetected += spectrum->getNumObj();
      for(int obj=0;obj<spectrum->getNumObj();obj++){
	Detection *object = new Detection;
	*object = spectrum->getObject(obj);
	for(int pix=0;pix<object->getSize();pix++) {
	  // Fix up coordinates of each pixel to match original array
	  object->setZ(pix, object->getX(pix));
	  object->setX(pix, x);
	  object->setY(pix, y);
	  object->setF(pix, array[object->getZ(pix)]); 
	  // NB: set F to the original value, not the recon value.
	}
	object->setOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
    
    
    } // end of loop over y

    printBackSpace(6);
    std::cout << std::flush;
  }   // end of loop over x

  delete spectrum;
  delete [] bigarray;
  delete [] array;
  delete [] fpixel;
  delete [] lpixel;
  delete [] inc;
  delete [] dimAxes;
  delete [] specdim;

  return outputList;

}
